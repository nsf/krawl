#include "crawlc.hpp"
#include <stdlib.h>

//------------------------------------------------------------------------------
// parser_t
//------------------------------------------------------------------------------

// lemon parser definitions
void *ParseAlloc(void*(*)(size_t));
void ParseFree(void*, void(*)(void*));
void Parse(void*, int, token_t*, parser_t*);
void ParseTrace(FILE*, char*);

parser_t::parser_t(source_group_t *srcinfo, diagnostic_t *diag):
	srcinfo(srcinfo), diag(diag), ast(0)
{
	lemon = ParseAlloc(::malloc);
	//ParseTrace(stderr, (char*)"");
}

parser_t::~parser_t()
{
	ParseFree(lemon, ::free);
}

node_t *parser_t::parse()
{
	lex.set_input(srcinfo->files.back());

	while (1) {
		token_t *t = lex.next_token();
		if (!t) {
			break;
		}
		last_token = t->type;
		last_pos = t->pos;

		Parse(lemon, t->type, t, this);
	}

	Parse(lemon, 0, 0, this);
	return ast;
}

void parser_t::set_input(const char *filename, std::vector<char> *buf)
{
	srcinfo->add_file(filename, buf);
}

//------------------------------------------------------------------------------
// nametype_vector_t to field_vector_t conversion
//
// Basically it's a bunch of weird code with does the following:
// converts nametype form:  [a], [b], [c int], [d], [e], [f float]
// to field form:           [a, b, c int], [d, e, f float]
// Issuing error messages on the way.
//------------------------------------------------------------------------------

static field_t *nametype_to_field(nametype_vector_t *accum,
				  nametype_t *d,
				  nametype_vector_t *invalid)
{
	// At the end of this function, every value from 'accum' and 'd' end up
	// somewhere in the 'invalid' buffer and in the newly created field_t.
	nametype_vector_t::iterator it, end;
	ident_expr_vector_t v;

	for (it = accum->begin(), end = accum->end(); it != end; ++it) {
		if (it->type->type == node_t::IDENT_EXPR)
			v.push_back((ident_expr_t*)it->type);
		else
			invalid->push_back(*it);
	}

	// at least one correct argument always exists (syntax forces that)
	v.push_back(d->name);

	field_t *f = new field_t(0, d->type);
	f->names.swap(v);
	return f;
}

field_vector_t *nametypev_to_fieldv(diagnostic_t *diag, nametype_vector_t *nv,
				    bool ellipsis)
{
	nametype_vector_t invalid_names;
	nametype_vector_t accum;
	field_vector_t *fv = new field_vector_t;
	nametype_vector_t::iterator it, end;

	for (it = nv->begin(), end = nv->end(); it != end; ++it) {
		switch (it->classify()) {
		case nametype_t::SINGLE:
			accum.push_back(*it);
			break;
		case nametype_t::DOUBLE:
			field_t *f = nametype_to_field(&accum, &*it, &invalid_names);
			fv->push_back(f);
			accum.clear();
			break;
		}
	}

	// If there are some invalid names, issue an error and free that list.
	if (!invalid_names.empty()) {
		std::vector<source_loc_range_t> ranges;
		ranges.reserve(invalid_names.size());
		for (size_t i = 0, n = invalid_names.size(); i < n; ++i) {
			switch (invalid_names[i].classify()) {
			case nametype_t::SINGLE:
				ranges.push_back(invalid_names[i].type->source_loc_range());
				break;
			case nametype_t::DOUBLE:
				ranges.push_back(invalid_names[i].name->source_loc_range());
				break;
			}
		}

		const char *msg = "bad name for a function argument";
		if (invalid_names.size() > 1)
			msg = "bad names for function arguments";

		message_t *m;
		m = new_message(MESSAGE_ERROR,
				ranges.front().beg, false,
				&ranges[0], ranges.size(),
				msg);
		diag->report(m);
		for (size_t i = 0, n = invalid_names.size(); i < n; ++i) {
			switch (invalid_names[i].classify()) {
			case nametype_t::SINGLE:
				delete invalid_names[i].type;
				break;
			case nametype_t::DOUBLE:
				delete invalid_names[i].name;
				delete invalid_names[i].type;
				break;
			}
		}
	}

	// If 'fv' contains valid fields at this point, it's the case with
	// named fields:
	//	"a, b, c int"
	// But if 'accum' is not empty, it means there are names without type,
	// issue an error and free the accum buffer. Everything else takes
	// place in the 'fv' (trying to leave in AST as much as possible).
	if (!accum.empty() && !fv->empty()) {
		std::vector<source_loc_range_t> ranges;
		ranges.reserve(accum.size());
		for (size_t i = 0, n = accum.size(); i < n; ++i)
			ranges.push_back(accum[i].type->source_loc_range());

		message_t *m;
		m = new_message(MESSAGE_ERROR,
				ranges.back().end, false,
				&ranges[0], ranges.size(),
				"missing type (or mixing named/unnamed arguments)");
		diag->report(m);
		for (size_t i = 0, n = accum.size(); i < n; ++i)
			delete accum[i].type;
	}

	if (fv->empty()) {
		// unnamed type list
		for (it = nv->begin(), end = nv->end(); it != end; ++it)
			fv->push_back(new field_t(0, it->type));
	}

	if (ellipsis)
		fv->push_back(0);

	delete nv;
	return fv;
}

//------------------------------------------------------------------------------
// additional syntax checks
//------------------------------------------------------------------------------

void syntax_check_const_decl(diagnostic_t *diag, const_decl_t *d)
{
	// first (or the only) constant spec must have a specified value
	value_spec_t *first = d->specs[0];
	if (first->values.empty()) {
		std::vector<source_loc_range_t> ranges;
		ranges.reserve(first->names.size());
		for (size_t i = 0, n = first->names.size(); i < n; ++i)
			ranges.push_back(first->names[i]->source_loc_range());

		const char *msg = "constant specification must have "
			"a specified value";
		if (d->specs.size() > 1)
			msg = "first constant specification must have "
				"a specified value";

		message_t *m;
		m = new_message(MESSAGE_ERROR,
				ranges.back().end, false,
				&ranges[0], ranges.size(),
				msg);
		diag->report(m);
	}

	// a number of names should match to a number of values
	for (size_t i = 0, n = d->specs.size(); i < n; ++i) {
		value_spec_t *s = d->specs[i];
		if (s->type && s->values.empty()) {
			std::vector<source_loc_range_t> ranges;
			ranges.reserve(s->names.size() + s->values.size());
			for (size_t i = 0, n = s->names.size(); i < n; ++i)
				ranges.push_back(s->names[i]->source_loc_range());
			source_loc_t pos = s->type->source_loc_range().beg;
			message_t *m;
			m = new_message(MESSAGE_ERROR,
					pos, true,
					&ranges[0], ranges.size(),
					"constant declaration cannot have type "
					"without initializer expression");
			diag->report(m);
		}
		if (!s->values.empty() && s->names.size() != s->values.size()) {
			std::vector<source_loc_range_t> ranges;
			ranges.reserve(s->names.size() + s->values.size());
			for (size_t i = 0, n = s->names.size(); i < n; ++i)
				ranges.push_back(s->names[i]->source_loc_range());
			for (size_t i = 0, n = s->values.size(); i < n; ++i)
				ranges.push_back(s->values[i]->source_loc_range());

			message_t *m;
			m = new_message(MESSAGE_ERROR,
					s->pos, true,
					&ranges[0], ranges.size(),
					"a number of names should match to a "
					"number of values");
			diag->report(m);
		}
	}
}

void syntax_check_var_decl(diagnostic_t *diag, var_decl_t *d)
{
	for (size_t i = 0, n = d->specs.size(); i < n; ++i) {
		value_spec_t *s = d->specs[i];
		if (s->values.empty() && s->type == 0) {
			std::vector<source_loc_range_t> ranges;
			ranges.reserve(s->names.size());
			for (size_t i = 0, n = s->names.size(); i < n; ++i)
				ranges.push_back(s->names[i]->source_loc_range());

			message_t *m;
			m = new_message(MESSAGE_ERROR,
					ranges.front().beg, false,
					&ranges[0], ranges.size(),
					"type is missing in value specification "
					"and cannot be inferred");
			diag->report(m);
		}
	}
	// a number of names should match to a number of values
	for (size_t i = 0, n = d->specs.size(); i < n; ++i) {
		value_spec_t *s = d->specs[i];
		if (!s->values.empty()) {
			if (s->values.size() == 1 &&
			    s->names.size() > 1)
			{
				if (!is_call_expr(s->values[0])) {
					std::vector<source_loc_range_t> ranges;
					ranges.reserve(s->names.size() + s->values.size());
					for (size_t i = 0, n = s->names.size(); i < n; ++i)
						ranges.push_back(s->names[i]->source_loc_range());
					for (size_t i = 0, n = s->values.size(); i < n; ++i)
						ranges.push_back(s->values[i]->source_loc_range());

					message_t *m;
					m = new_message(MESSAGE_ERROR,
							s->pos, true,
							&ranges[0], ranges.size(),
							"assignment count mismatch: "
							"%d = %d", s->names.size(),
							s->values.size());
					diag->report(m);
				}
				continue;
			}

			if (s->values.size() == s->names.size())
				continue;

			std::vector<source_loc_range_t> ranges;
			ranges.reserve(s->names.size() + s->values.size());
			for (size_t i = 0, n = s->names.size(); i < n; ++i)
				ranges.push_back(s->names[i]->source_loc_range());
			for (size_t i = 0, n = s->values.size(); i < n; ++i)
				ranges.push_back(s->values[i]->source_loc_range());

			message_t *m;
			m = new_message(MESSAGE_ERROR,
					s->pos, true,
					&ranges[0], ranges.size(),
					"assignment count mismatch: %d = %d",
					s->names.size(),
					s->values.size());
			diag->report(m);
		}
	}
}
