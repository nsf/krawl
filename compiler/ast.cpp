#include "crawlc.hpp"
#include <ctype.h>

static std::string istr(int level)
{
	std::string out;
	for (int i = 0; i < level; ++i)
		out += "  ";
	return out;
}

//------------------------------------------------------------------------------
// value_t
//------------------------------------------------------------------------------

value_t::value_t():
	type(VALUE_NONE)
{
}

value_t::value_t(value_type_t t):
	type(t)
{
}

value_t::value_t(unsigned int v):
	type(VALUE_INT)
{
	mpz_init_set_ui(xint, v);
}

value_t::value_t(const std::string &s):
	type(VALUE_STRING)
{
	xstr = strdup(s.c_str());
}

value_t::value_t(const char *s, int tok):
	type(VALUE_NONE)
{
	switch (tok) {
	case TOK_STRING:
	case TOK_CHAR:
	case TOK_IDENT:
		xstr = strdup(s);
		type = VALUE_STRING;
		break;
	case TOK_INT:
		mpz_init_set_str(xint, s, 0);
		type = VALUE_INT;
		break;
	case TOK_FLOAT:
		mpfr_init_set_str(xfloat, s, 10, MPFR_RNDN);
		type = VALUE_FLOAT;
		break;
	default:
		break;
	}
}

value_t::value_t(const char *s, value_type_t t):
	type(VALUE_NONE)
{
	type = t;
	switch (t) {
	case VALUE_STRING:
		xstr = strdup(s);
		break;
	case VALUE_INT:
		mpz_init_set_str(xint, s, 0);
		break;
	case VALUE_FLOAT:
		mpfr_init_set_str(xfloat, s, 10, MPFR_RNDN);
		break;
	default:
		break;
	}
}

value_t::value_t(const value_t &r):
	type(r.type)
{
	switch (type) {
	case VALUE_STRING:
		xstr = strdup(r.xstr);
		break;
	case VALUE_INT:
		mpz_init_set(xint, r.xint);
		break;
	case VALUE_FLOAT:
		mpfr_init_set(xfloat, r.xfloat, MPFR_RNDN);
		break;
	default:
		break;
	}
}

value_t::~value_t()
{
	free();
}

void value_t::free()
{
	switch (type) {
	case VALUE_STRING:
		::free(xstr);
		break;
	case VALUE_INT:
		mpz_clear(xint);
		break;
	case VALUE_FLOAT:
		mpfr_clear(xfloat);
		break;
	default:
		break;
	}
}

value_t &value_t::operator=(const value_t &r)
{
	free();

	type = r.type;

	switch (type) {
	case VALUE_STRING:
		xstr = strdup(r.xstr);
		break;
	case VALUE_INT:
		mpz_init_set(xint, r.xint);
		break;
	case VALUE_FLOAT:
		mpfr_init_set(xfloat, r.xfloat, MPFR_RNDN);
		break;
	default:
		break;
	}

	return *this;
}

value_t value_t::to_float()
{
	// we can only promote INT to FLOAT
	CRAWL_QASSERT(type == VALUE_INT);
	value_t out;
	out.type = VALUE_FLOAT;
	mpfr_init_set_z(out.xfloat, xint, MPFR_RNDN);
	return out;
}

value_t value_t::to_int()
{
	// we can only convert FLOAT to INT
	CRAWL_QASSERT(type == VALUE_FLOAT);
	value_t out;
	out.type = VALUE_INT;
	mpz_init(out.xint);
	mpfr_get_z(out.xint, xfloat, MPFR_RNDN);
	return out;
}

std::string value_t::to_string()
{
	std::string out;
	char *s;
	switch (type) {
	case VALUE_NONE:
		out = "<NONE>";
		break;
	case VALUE_ERROR:
		out = "<ERROR>";
		break;
	case VALUE_STRING:
		out = xstr;
		break;
	case VALUE_INT:
		s = mpz_get_str(0, 10, xint);
		out = s;
		::free(s);
		break;
	case VALUE_FLOAT:
		mpfr_asprintf(&s, "%Re", xfloat);
		out = s;
		mpfr_free_str(s);
		break;
	}
	return out;
}

bool value_t::to_bool()
{
	CRAWL_QASSERT(type == VALUE_INT);
	return mpz_get_ui(xint) != 0;
}

static std::string escape_string(const char *s)
{
	std::string out;
	while (*s) {
		if (isprint(*s))
			out.append(1, *s);
		else
			cppsprintf(&out, "\\x%.2X", *s);
		s++;
	}
	return out;
}

std::string value_t::to_literal()
{
	if (type == VALUE_STRING)
		return std::string("\"") + escape_string(xstr) + "\"";
	return to_string();
}

value_t value_t::none(VALUE_NONE);
value_t value_t::error(VALUE_ERROR);

//------------------------------------------------------------------------------
// token_t
//------------------------------------------------------------------------------

token_t::token_t(const token_t &r):
	type(r.type), val(r.val), pos(r.pos), beg(r.beg), end(r.end)
{
}

token_t::token_t(const char *s, int t, source_loc_t p, char *beg, char *end):
	type(t), val(s, t), pos(p), beg(beg), end(end)
{
}

token_t::~token_t()
{
}

//------------------------------------------------------------------------------
// node_t
//------------------------------------------------------------------------------

node_t::node_t(type_t t): type(t) {}
node_t::~node_t() {}

bool node_t::is_expr()
{
	return type >= COMPOUND_LIT && type <= TYPE_CAST_EXPR;
}

bool node_t::is_terminator()
{
	return type == RETURN_STMT || type == FLOW_STMT;
}

void print_ast(node_t *n)
{
	std::string s = n->to_string(0);
	printf("%s", s.c_str());
}

//------------------------------------------------------------------------------
// program_t
//------------------------------------------------------------------------------

program_t::program_t(node_vector_t *d):
	node_t(node_t::PROGRAM)
{
	decls.swap(*d);
	delete d;
}

program_t::~program_t()
{
	for (size_t i = 0, n = decls.size(); i < n; ++i)
		delete decls[i];
}

std::string program_t::to_string(int indent)
{
	std::string out;
	for (size_t i = 0, n = decls.size(); i < n; ++i) {
		if (!decls[i])
			continue;
		out += istr(indent) + "-\n";
		out += decls[i]->to_string(indent+1);
	}
	return out;
}

source_loc_range_t program_t::source_loc_range()
{
	source_loc_range_t slr = {0, 0};
	if (decls.empty())
		return slr;

	if (decls.size() == 1)
		return decls[0]->source_loc_range();

	source_loc_range_t b = decls.front()->source_loc_range();
	source_loc_range_t e = decls.back()->source_loc_range();
	slr.beg = b.beg;
	slr.end = e.end;
	return slr;
}

//------------------------------------------------------------------------------
// compound_lit_t
//------------------------------------------------------------------------------

compound_lit_t::compound_lit_t(node_vector_t *elts, node_t *etype,
			       token_t *l, token_t *r):
	node_t(node_t::COMPOUND_LIT), etype(etype),
	pos_lp(l->pos), pos_rp(r->pos)
{
	if (elts) {
		this->elts.swap(*elts);
		delete elts;
	}
	delete l;
	delete r;
}

compound_lit_t::~compound_lit_t()
{
	delete etype;
}

std::string compound_lit_t::to_string(int indent)
{
	std::string out;
	out += istr(indent) + "node: COMPOUND_LIT\n";
	out += istr(indent) + "elements:\n";
	for (size_t i = 0, n = elts.size(); i < n; ++i) {
		out += istr(indent+1) + "-\n";
		out += elts[i]->to_string(indent+2);
	}
	if (etype) {
		out += istr(indent) + "type:\n";
		out += etype->to_string(indent+1);
	}
	return out;
}

source_loc_range_t compound_lit_t::source_loc_range()
{
	source_loc_range_t slk = {pos_lp, pos_rp};
	return slk;
}

//------------------------------------------------------------------------------
// expr_stmt_t
//------------------------------------------------------------------------------

expr_stmt_t::expr_stmt_t(node_t *ae):
	node_t(node_t::EXPR_STMT), expr(ae)
{
}

expr_stmt_t::~expr_stmt_t()
{
	delete expr;
}

std::string expr_stmt_t::to_string(int indent)
{
	return expr->to_string(indent); // is it ok?
}

source_loc_range_t expr_stmt_t::source_loc_range()
{
	return expr->source_loc_range();
}

//------------------------------------------------------------------------------
// decl_stmt_t
//------------------------------------------------------------------------------

decl_stmt_t::decl_stmt_t(node_t *ad):
	node_t(node_t::DECL_STMT), decl(ad)
{
}

decl_stmt_t::~decl_stmt_t()
{
	delete decl;
}

std::string decl_stmt_t::to_string(int indent)
{
	return decl->to_string(indent); // is it ok?
}

source_loc_range_t decl_stmt_t::source_loc_range()
{
	return decl->source_loc_range();
}

//------------------------------------------------------------------------------
// incdec_stmt_t
//------------------------------------------------------------------------------

incdec_stmt_t::incdec_stmt_t(node_t *e, token_t *t):
	node_t(node_t::INCDEC_STMT), tok(t->type), expr(e), pos(t->pos)
{
	delete t;
}

incdec_stmt_t::~incdec_stmt_t()
{
	delete expr;
}

std::string incdec_stmt_t::to_string(int indent)
{
	std::string out;
	out += istr(indent) + "node: INCDEC_STMT\n";
	out += istr(indent) + "token: ";
	switch (tok) {
	case TOK_INC:
		out += "TOK_INC";
		break;
	case TOK_DEC:
		out += "TOK_DEC";
		break;
	default:
		out += "???";
		break;
	}
	out += "\n";
	out += istr(indent) + "expr:\n" + expr->to_string(indent+1);
	return out;
}

source_loc_range_t incdec_stmt_t::source_loc_range()
{
	source_loc_range_t b = expr->source_loc_range();
	source_loc_range_t slr = {b.beg, pos};
	return slr;
}

//------------------------------------------------------------------------------
// assign_stmt_t
//------------------------------------------------------------------------------

assign_stmt_t::assign_stmt_t(node_vector_t *alhs, node_vector_t *arhs, token_t *t):
	node_t(node_t::ASSIGN_STMT), tok(t->type), pos(t->pos)
{
	lhs.swap(*alhs);
	rhs.swap(*arhs);
	delete alhs;
	delete arhs;
	delete t;
}

assign_stmt_t::~assign_stmt_t()
{
	for (size_t i = 0, n = lhs.size(); i < n; ++i)
		delete lhs[i];
	for (size_t i = 0, n = rhs.size(); i < n; ++i)
		delete rhs[i];
}

std::string assign_stmt_t::to_string(int indent)
{
	std::string out;
	out += istr(indent) + "node: ASSIGN_STMT\n";
	out += istr(indent) + "token: ";
	switch (tok) {
	case TOK_ASSIGN: out += "TOK_ASSIGN";       break;
	case TOK_DECLARIZE: out += "TOK_DECLARIZE"; break;
	default:
		out += "???";
		break;
	}
	out += "\n";
	out += istr(indent) + "lhs:\n";
	for (size_t i = 0, n = lhs.size(); i < n; ++i)
		out += istr(indent+1) + "-\n" + lhs[i]->to_string(indent+2);
	out += istr(indent) + "rhs:\n";
	for (size_t i = 0, n = rhs.size(); i < n; ++i)
		out += istr(indent+1) + "-\n" + rhs[i]->to_string(indent+2);
	return out;
}

source_loc_range_t assign_stmt_t::source_loc_range()
{
	source_loc_range_t b = lhs.front()->source_loc_range();
	source_loc_range_t e = rhs.back()->source_loc_range();
	source_loc_range_t slr = {b.beg, e.end};
	return slr;
}

//------------------------------------------------------------------------------
// block_stmt_t
//------------------------------------------------------------------------------

block_stmt_t::block_stmt_t(node_vector_t *s, token_t *l, token_t *r):
	node_t(node_t::BLOCK_STMT), pos_lcb(l->pos), pos_rcb(r->pos)
{
	if (s) {
		stmts.swap(*s);
		delete s;
	}
	delete l;
	delete r;
}

block_stmt_t::~block_stmt_t()
{
	for (size_t i = 0, n = stmts.size(); i < n; ++i)
		delete stmts[i];
}

std::string block_stmt_t::to_string(int indent)
{
	std::string out;
	out += istr(indent) + "node: BLOCK_STMT\n";
	if (!stmts.empty())
		out += istr(indent) + "statements:\n";
	for (size_t i = 0, n = stmts.size(); i < n; ++i)
		out += istr(indent+1) + "-\n" + stmts[i]->to_string(indent+2);
	return out;
}

source_loc_range_t block_stmt_t::source_loc_range()
{
	source_loc_range_t slr = {pos_lcb, pos_rcb};
	return slr;
}

//------------------------------------------------------------------------------
// return_stmt_t
//------------------------------------------------------------------------------

return_stmt_t::return_stmt_t(node_vector_t *r, token_t *t):
	node_t(node_t::RETURN_STMT), pos(t->pos)
{
	if (r) {
		returns.swap(*r);
		delete r;
	}
	delete t;
}

return_stmt_t::~return_stmt_t()
{
	for (size_t i = 0, n = returns.size(); i < n; ++i)
		delete returns[i];
}

std::string return_stmt_t::to_string(int indent)
{
	std::string out;
	out += istr(indent) + "node: RETURN_STMT\n";
	if (!returns.empty())
		out += istr(indent) + "returns:\n";
	for (size_t i = 0, n = returns.size(); i < n; ++i) {
		out += istr(indent+1) + "-\n";
		out += returns[i]->to_string(indent+2);
	}
	return out;
}

source_loc_range_t return_stmt_t::source_loc_range()
{
	source_loc_range_t slk = {pos, 0};
	if (returns.empty()) {
		slk.end = pos + 6; // return keyword
		return slk;
	}

	source_loc_range_t e = returns.back()->source_loc_range();
	slk.end = e.end;
	return slk;
}

//------------------------------------------------------------------------------
// ifelse_stmt_t
//------------------------------------------------------------------------------

ifelse_stmt_t::ifelse_stmt_t(node_t *c, block_stmt_t *b, token_t *i,
			     token_t *et, node_t *e):
	node_t(node_t::IFELSE_STMT), cond(c), body(b), elsestmt(0), pos_if(i->pos)
{
	delete i;
	if (et) {
		pos_else = et->pos;
		elsestmt = e;
		delete et;
	}
}

ifelse_stmt_t::~ifelse_stmt_t()
{
	delete cond;
	delete body;
	delete elsestmt;
}

std::string ifelse_stmt_t::to_string(int indent)
{
	std::string out;
	out += istr(indent) + "node: IFELSE_STMT\n";
	out += istr(indent) + "condition:\n";
	out += cond->to_string(indent+1);
	out += istr(indent) + "body:\n";
	out += body->to_string(indent+1);
	if (elsestmt) {
		out += istr(indent) + "else:\n";
		out += elsestmt->to_string(indent+1);
	}
	return out;
}

source_loc_range_t ifelse_stmt_t::source_loc_range()
{
	source_loc_range_t slk = {pos_if, body->pos_rcb};
	if (elsestmt) {
		source_loc_range_t e = elsestmt->source_loc_range();
		slk.end = e.end;
	}
	return slk;
}

//------------------------------------------------------------------------------
// for_stmt_t
//------------------------------------------------------------------------------

for_stmt_t::for_stmt_t(node_t *i, node_t *c, node_t *p,
		       block_stmt_t *b, token_t *t):
	node_t(node_t::FOR_STMT), init(i), cond(c), post(p), body(b), pos(t->pos)
{
	delete t;
}

for_stmt_t::~for_stmt_t()
{
	delete init;
	delete cond;
	delete post;
	delete body;
}

std::string for_stmt_t::to_string(int indent)
{
	std::string out;
	out += istr(indent) + "node: FOR_STMT\n";
	if (init) {
		out += istr(indent) + "init:\n";
		out += init->to_string(indent+1);
	}
	if (cond) {
		out += istr(indent) + "condition:\n";
		out += cond->to_string(indent+1);
	}
	if (post) {
		out += istr(indent) + "post:\n";
		out += post->to_string(indent+1);
	}
	out += istr(indent) + "body:\n";
	out += body->to_string(indent+1);
	return out;
}

source_loc_range_t for_stmt_t::source_loc_range()
{
	source_loc_range_t slk = {pos, body->pos_rcb};
	return slk;
}

//------------------------------------------------------------------------------
// switch_stmt_case_t
//------------------------------------------------------------------------------

switch_stmt_case_t::switch_stmt_case_t(node_vector_t *exprs, node_vector_t *stmts,
				       token_t *t, token_t *c):
	node_t(node_t::SWITCH_STMT_CASE), pos_tok(t->pos), pos_col(c->pos)
{
	if (exprs) {
		this->exprs.swap(*exprs);
		delete exprs;
	}
	if (stmts) {
		this->stmts.swap(*stmts);
		delete stmts;
	}
	delete t;
	delete c;
}

switch_stmt_case_t::~switch_stmt_case_t()
{
	for (size_t i = 0, n = exprs.size(); i < n; ++i)
		delete exprs[i];
	for (size_t i = 0, n = stmts.size(); i < n; ++i)
		delete stmts[i];
}

std::string switch_stmt_case_t::to_string(int indent)
{
	std::string out;
	out += istr(indent) + "node: SWITCH_STMT_CASE\n";
	out += istr(indent) + "type: " +
		(exprs.empty() ? "TOK_DEFAULT" : "TOK_CASE");
	out += "\n";

	if (!exprs.empty()) {
		out += istr(indent) + "exprs:\n";
		for (size_t i = 0, n = exprs.size(); i < n; ++i) {
			out += istr(indent+1) + "-\n";
			out += exprs[i]->to_string(indent+2);
		}
	}
	if (!stmts.empty()) {
		out += istr(indent) + "stmts:\n";
		for (size_t i = 0, n = stmts.size(); i < n; ++i) {
			out += istr(indent+1) + "-\n";
			out += stmts[i]->to_string(indent+2);
		}
	}
	return out;
}

source_loc_range_t switch_stmt_case_t::source_loc_range()
{
	source_loc_range_t slk = {pos_tok, pos_col};
	if (!stmts.empty()) {
		source_loc_range_t range = stmts.back()->source_loc_range();
		slk.end = range.end;
	}
	return slk;
}

//------------------------------------------------------------------------------
// switch_stmt_t
//------------------------------------------------------------------------------

switch_stmt_t::switch_stmt_t(node_t *expr, block_stmt_t *body, token_t *t):
	node_t(node_t::SWITCH_STMT), expr(expr), body(body), pos(t->pos),
	constant(true)
{
	delete t;
}

switch_stmt_t::~switch_stmt_t()
{
	delete expr;
	delete body;
}

std::string switch_stmt_t::to_string(int indent)
{
	std::string out;
	out += istr(indent) + "node: SWITCH_STMT\n";
	out += istr(indent) + "expr:\n";
	out += expr->to_string(indent+1);
	out += istr(indent) + "body:\n";
	out += body->to_string(indent+1);
	return out;
}

source_loc_range_t switch_stmt_t::source_loc_range()
{
	source_loc_range_t slk = {pos, body->pos_rcb};
	return slk;
}

size_t switch_stmt_t::num_cases()
{
	size_t num = 0;
	for (size_t i = 0, n = body->stmts.size(); i < n; ++i) {
		CRAWL_QASSERT(body->stmts[i]->type == node_t::SWITCH_STMT_CASE);
		switch_stmt_case_t *c = (switch_stmt_case_t*)body->stmts[i];

		if (c->exprs.empty()) {
			num++;
			continue;
		}

		num += c->exprs.size();
	}
	return num;
}

//------------------------------------------------------------------------------
// flow_stmt_t
//------------------------------------------------------------------------------

flow_stmt_t::flow_stmt_t(token_t *t):
	node_t(node_t::FLOW_STMT), tok(t->type), pos(t->pos)
{
}

std::string flow_stmt_t::to_string(int indent)
{
	std::string out;
	out += istr(indent) + "node: FLOW_STMT\n";
	out += istr(indent) + "token: " + token_to_string(tok);
	out += "\n";
	return out;
}

source_loc_range_t flow_stmt_t::source_loc_range()
{
	source_loc_range_t slr = {pos, 0};
	switch (tok) {
	case TOK_BREAK:
		slr.end = pos + 5;
		break;
	case TOK_CONTINUE:
		slr.end = pos + 8;
		break;
	case TOK_FALLTHROUGH:
		slr.end = pos + 11;
		break;
	default:
		CRAWL_QASSERT(!"unreachable");
	}
	return slr;
}

//------------------------------------------------------------------------------
// binary_expr_t
//------------------------------------------------------------------------------

binary_expr_t::binary_expr_t(node_t *l, node_t *r, token_t *t):
	node_t(node_t::BINARY_EXPR), tok(t->type), lhs(l), rhs(r), pos(t->pos)
{
	delete t;
}

binary_expr_t::~binary_expr_t()
{
	delete lhs;
	delete rhs;
}

std::string binary_expr_t::to_string(int indent)
{
	std::string out;
	out += istr(indent) + "node: BINARY_EXPR\n";
	out += istr(indent) + "token: ";

	switch (tok) {
	case TOK_DIVIDE: out += "TOK_DIVIDE"; break;
	case TOK_TIMES:  out += "TOK_TIMES";  break;
	case TOK_MOD:    out += "TOK_MOD";    break;
	case TOK_SHIFTL: out += "TOK_SHIFTL"; break;
	case TOK_SHIFTR: out += "TOK_SHIFTR"; break;
	case TOK_AND:    out += "TOK_AND";    break;
	case TOK_ANDNOT: out += "TOK_ANDNOT"; break;
	case TOK_PLUS:   out += "TOK_PLUS";   break;
	case TOK_MINUS:  out += "TOK_MINUS";  break;
	case TOK_OR:     out += "TOK_OR";     break;
	case TOK_XOR:    out += "TOK_XOR";    break;
	case TOK_EQ:     out += "TOK_EQ";     break;
	case TOK_NEQ:    out += "TOK_NEQ";    break;
	case TOK_LT:     out += "TOK_LT";     break;
	case TOK_LE:     out += "TOK_LE";     break;
	case TOK_GT:     out += "TOK_GT";     break;
	case TOK_GE:     out += "TOK_GE";     break;
	case TOK_ANDAND: out += "TOK_ANDAND"; break;
	case TOK_OROR:   out += "TOK_OROR";   break;
	default:
		out += "???";
		break;
	}
	out += "\n";
	out += istr(indent) + "lhs:\n" + lhs->to_string(indent+1);
	out += istr(indent) + "rhs:\n" + rhs->to_string(indent+1);
	return out;
}

source_loc_range_t binary_expr_t::source_loc_range()
{
	source_loc_range_t b = lhs->source_loc_range();
	source_loc_range_t e = rhs->source_loc_range();
	source_loc_range_t slk = {b.beg, e.end};
	return slk;
}

//------------------------------------------------------------------------------
// unary_expr_t
//------------------------------------------------------------------------------

unary_expr_t::unary_expr_t(node_t *e, token_t *t):
	node_t(node_t::UNARY_EXPR), tok(t->type), expr(e), pos(t->pos)
{
	delete t;
}

unary_expr_t::~unary_expr_t()
{
	delete expr;
}

std::string unary_expr_t::to_string(int indent)
{
	std::string out;
	out += istr(indent) + "node: UNARY_EXPR\n";
	out += istr(indent) + "token: ";
	switch (tok) {
	case TOK_PLUS: out += "TOK_PLUS";           break;
	case TOK_MINUS: out += "TOK_MINUS";         break;
	case TOK_NOT: out += "TOK_NOT";             break;
	case TOK_XOR: out += "TOK_XOR";             break;
	case TOK_AND: out += "TOK_AND";             break;
	case TOK_TIMES: out += "TOK_TIMES";         break;
	default:
		out += "???";
		break;
	}
	out += "\n";
	out += istr(indent) + "expr:\n" + expr->to_string(indent+1);
	return out;
}

source_loc_range_t unary_expr_t::source_loc_range()
{
	source_loc_range_t end = expr->source_loc_range();
	source_loc_range_t slk = {pos, end.end};
	return slk;
}

//------------------------------------------------------------------------------
// basic_lit_expr_t
//------------------------------------------------------------------------------

basic_lit_expr_t::basic_lit_expr_t(token_t *t):
	node_t(node_t::BASIC_LIT_EXPR), tok(t->type), val(t->val),
	pos_beg(t->pos), pos_end(t->pos+(t->end - t->beg - 1))
{
	delete t;
}

basic_lit_expr_t::~basic_lit_expr_t()
{
}

std::string basic_lit_expr_t::to_string(int indent)
{
	char *s;
	std::string out;
	out += istr(indent) + "node: BASIC_LIT_EXPR\n";
	out += istr(indent) + "token: ";

	switch (tok) {
	case TOK_CHAR:
		out += "TOK_CHAR\n";
		out += istr(indent) + "value: " + val.xstr;
		break;
	case TOK_STRING:
		out += "TOK_STRING\n";
		out += istr(indent) + "value: " + val.xstr;
		break;
	case TOK_INT:
		s = mpz_get_str(0, 10, val.xint);
		out += "TOK_INT\n";
		out += istr(indent) + "value: " + s;
		free(s);
		break;
	case TOK_FLOAT:
		mpfr_asprintf(&s, "%Rf", val.xfloat);
		out += "TOK_FLOAT\n";
		out += istr(indent) + "value: " + s;
		mpfr_free_str(s);
		break;
	default:
		out += "???\n";
		out += istr(indent) + "value: ???";
		break;
	}
	out += "\n";
	return out;
}

source_loc_range_t basic_lit_expr_t::source_loc_range()
{
	source_loc_range_t slk = {pos_beg, pos_end};
	return slk;
}

//------------------------------------------------------------------------------
// ident_expr_t
//------------------------------------------------------------------------------

ident_expr_t::ident_expr_t(token_t *t):
	node_t(node_t::IDENT_EXPR), val(t->val.xstr), pos(t->pos), sdecl(0)
{
	delete t;
}

std::string ident_expr_t::to_string(int indent)
{
	std::string out;
	out += istr(indent) + "node: IDENT_EXPR\n";
	out += istr(indent) + "value: " + val + "\n";
	return out;
}

source_loc_range_t ident_expr_t::source_loc_range()
{
	source_loc_range_t slk = {pos, pos + val.size() - 1};
	return slk;
}

//------------------------------------------------------------------------------
// selector_expr_t
//------------------------------------------------------------------------------

selector_expr_t::selector_expr_t(node_t *e, ident_expr_t *ident):
	node_t(node_t::SELECTOR_EXPR), expr(e), sel(ident), idx(0)
{
}

selector_expr_t::~selector_expr_t()
{
	delete expr;
	delete sel;
}

std::string selector_expr_t::to_string(int indent)
{
	std::string out;
	out += istr(indent) + "node: SELECTOR_EXPR\n";
	out += istr(indent) + "selectable:\n" + expr->to_string(indent+1);
	out += istr(indent) + "selector:\n" + sel->to_string(indent+1);
	return out;
}

source_loc_range_t selector_expr_t::source_loc_range()
{
	source_loc_range_t b = expr->source_loc_range();
	source_loc_range_t e = sel->source_loc_range();
	source_loc_range_t slk = {b.beg, e.end};
	return slk;
}

//------------------------------------------------------------------------------
// index_expr_t
//------------------------------------------------------------------------------

index_expr_t::index_expr_t(node_t *e, node_t *i, token_t *l, token_t *r):
	node_t(node_t::INDEX_EXPR), expr(e), index(i),
	pos_lsb(l->pos), pos_rsb(r->pos)
{
	delete l;
	delete r;
}

index_expr_t::~index_expr_t()
{
	delete expr;
	delete index;
}

std::string index_expr_t::to_string(int indent)
{
	std::string out;
	out += istr(indent) + "node: INDEX_EXPR\n";
	out += istr(indent) + "expr:\n" + expr->to_string(indent+1);
	out += istr(indent) + "index:\n" + index->to_string(indent+1);
	return out;
}

source_loc_range_t index_expr_t::source_loc_range()
{
	source_loc_range_t b = expr->source_loc_range();
	source_loc_range_t slk = {b.beg, pos_rsb};
	return slk;
}

//------------------------------------------------------------------------------
// call_expr_t
//------------------------------------------------------------------------------

call_expr_t::call_expr_t(node_t *e, node_vector_t *aargs, token_t *l, token_t *r):
	node_t(node_t::CALL_EXPR), expr(e), pos_lp(l->pos), pos_rp(r->pos),
	typeerror(false), mexpected(0)
{
	if (aargs) {
		args.swap(*aargs);
		delete aargs;
	}
	delete l;
	delete r;
}

call_expr_t::~call_expr_t()
{
	for (size_t i = 0, n = args.size(); i < n; ++i)
		delete args[i];
	delete expr;
}

std::string call_expr_t::to_string(int indent)
{
	std::string out;
	out += istr(indent) + "node: CALL_EXPR\n";
	out += istr(indent) + "callee:\n" + expr->to_string(indent+1);
	if (!args.empty()) {
		out += istr(indent) + "args:\n";
		for (size_t i = 0; i < args.size(); ++i) {
			out += istr(indent+1) + "-\n" +
				args[i]->to_string(indent+2);
		}
	}
	return out;
}

source_loc_range_t call_expr_t::source_loc_range()
{
	source_loc_range_t b = expr->source_loc_range();
	source_loc_range_t slk = {b.beg, pos_rp};
	return slk;
}

//------------------------------------------------------------------------------
// paren_expr_t
//------------------------------------------------------------------------------

paren_expr_t::paren_expr_t(node_t *e, token_t *l, token_t *r):
	node_t(node_t::PAREN_EXPR), expr(e), pos_lp(l->pos), pos_rp(r->pos)
{
	delete l;
	delete r;
}

paren_expr_t::~paren_expr_t()
{
	delete expr;
}

std::string paren_expr_t::to_string(int indent)
{
	return expr->to_string(indent);
}

source_loc_range_t paren_expr_t::source_loc_range()
{
	source_loc_range_t slk = {pos_lp, pos_rp};
	return slk;
}

//------------------------------------------------------------------------------
// type_cast_expr_t
//------------------------------------------------------------------------------

type_cast_expr_t::type_cast_expr_t(node_t *e, node_t *t, token_t *dot,
				   token_t *l, token_t *r):
	node_t(node_t::TYPE_CAST_EXPR), expr(e), ctype(t),
	pos_dot(dot->pos), pos_lp(l->pos), pos_rp(r->pos)
{
	delete dot;
	delete l;
	delete r;
}

type_cast_expr_t::~type_cast_expr_t()
{
	delete expr;
	delete ctype;
}

std::string type_cast_expr_t::to_string(int indent)
{
	std::string out;
	out += istr(indent) + "node: TYPE_CAST_EXPR\n";
	out += istr(indent) + "type:\n" + ctype->to_string(indent+1);
	out += istr(indent) + "expr:\n" + expr->to_string(indent+1);
	return out;
}

source_loc_range_t type_cast_expr_t::source_loc_range()
{
	source_loc_range_t beg = expr->source_loc_range();
	source_loc_range_t slk = {beg.beg, pos_rp};
	return slk;
}

//------------------------------------------------------------------------------
// type_expr_t
//------------------------------------------------------------------------------

type_expr_t::type_expr_t(node_t *etype, token_t *tok):
	node_t(node_t::TYPE_EXPR), etype(etype), pos(tok->pos)
{
	delete tok;
}

type_expr_t::~type_expr_t()
{
	delete etype;
}

std::string type_expr_t::to_string(int indent)
{
	std::string out;
	out += istr(indent) + "node: TYPE_EXPR\n";
	out += istr(indent) + "type:\n" + etype->to_string(indent+1);
	return out;
}

source_loc_range_t type_expr_t::source_loc_range()
{
	source_loc_range_t end = etype->source_loc_range();
	source_loc_range_t slk = {pos, end.end};
	return slk;
}

//------------------------------------------------------------------------------
// array_type_t
//------------------------------------------------------------------------------

array_type_t::array_type_t(node_t *s, node_t *et, token_t *l, token_t *r):
	node_t(node_t::ARRAY_TYPE), size(s), etype(et),
	pos_lsb(l->pos), pos_rsb(r->pos)
{
	delete l;
	delete r;
}

array_type_t::~array_type_t()
{
	delete size;
	delete etype;
}

std::string array_type_t::to_string(int indent)
{
	std::string out;
	out += istr(indent) + "node: ARRAY_TYPE\n";
	if (size)
		out += istr(indent) + "size:\n" + size->to_string(indent+1);
	out += istr(indent) + "element:\n" + etype->to_string(indent+1);
	return out;
}

source_loc_range_t array_type_t::source_loc_range()
{
	source_loc_range_t e = etype->source_loc_range();
	source_loc_range_t slk = {pos_lsb, e.end};
	return slk;
}

//------------------------------------------------------------------------------
// pointer_type_t
//------------------------------------------------------------------------------

pointer_type_t::pointer_type_t(node_t *pt, token_t *t):
	node_t(node_t::POINTER_TYPE), ptype(pt), pos(t->pos)
{
	delete t;
}

pointer_type_t::~pointer_type_t()
{
	delete ptype;
}

std::string pointer_type_t::to_string(int indent)
{
	std::string out;
	out += istr(indent) + "node: POINTER_TYPE\n";
	out += istr(indent) + "type:\n" + ptype->to_string(indent+1);
	return out;
}

source_loc_range_t pointer_type_t::source_loc_range()
{
	source_loc_range_t end = ptype->source_loc_range();
	source_loc_range_t slk = {pos, end.end};
	return slk;
}

//------------------------------------------------------------------------------
// field_t
//------------------------------------------------------------------------------

field_t::field_t(ident_expr_vector_t *n, node_t *t):
	type(t)
{
	if (n) {
		names.swap(*n);
		delete n;
	}
}

field_t::~field_t()
{
	for (size_t i = 0, n = names.size(); i < n; ++i)
		delete names[i];
	delete type;
}

std::string field_t::to_string(int indent)
{
	std::string out;
	if (!names.empty()) {
		out += istr(indent) + "names:\n";
		for (size_t i = 0, n = names.size(); i < n; ++i) {
			out += istr(indent+1) + "-\n";
			out += names[i]->to_string(indent+2);
		}
	}
	out += istr(indent) + "type:\n";
	out += type->to_string(indent+1);
	return out;
}

int nametype_t::classify()
{
	if (name == 0)
		return nametype_t::SINGLE;
	return nametype_t::DOUBLE;
}

//------------------------------------------------------------------------------
// struct_type_t
//------------------------------------------------------------------------------

struct_type_t::struct_type_t(field_vector_t *f, token_t *t, token_t *l, token_t *r):
	node_t(node_t::STRUCT_TYPE), pos(t->pos), pos_lcb(l->pos), pos_rcb(r->pos)
{
	if (f) {
		fields.swap(*f);
		delete f;
	}
	delete t;
	delete l;
	delete r;
}

struct_type_t::~struct_type_t()
{
	for (size_t i = 0, n = fields.size(); i < n; ++i)
		delete fields[i];
}

std::string struct_type_t::to_string(int indent)
{
	std::string out;
	out += istr(indent) + "node: STRUCT_TYPE\n";
	if (!fields.empty())
		out += istr(indent) + "fields:\n";
	for (size_t i = 0, n = fields.size(); i < n; ++i) {
		out += istr(indent+1) + "-\n";
		out += fields[i]->to_string(indent+2);
	}
	return out;
}

source_loc_range_t struct_type_t::source_loc_range()
{
	source_loc_range_t slk = {pos, pos_rcb};
	return slk;
}
//------------------------------------------------------------------------------
// func_type_t
//------------------------------------------------------------------------------

func_type_t::func_type_t(field_vector_t *a, field_vector_t *r, token_t *t):
	node_t(node_t::FUNC_TYPE), pos(t->pos), varargs(false)
{
	if (a) {
		// check for ellipsis
		if (a->back() == 0) {
			a->pop_back();
			varargs = true;
		}
		args.swap(*a);
		delete a;
	}
	if (r) {
		results.swap(*r);
		delete r;
	}
	delete t;
}

func_type_t::~func_type_t()
{
	for (size_t i = 0, n = args.size(); i < n; ++i)
		delete args[i];
	for (size_t i = 0, n = results.size(); i < n; ++i)
		delete results[i];
}

std::string func_type_t::to_string(int indent)
{
	std::string out;
	out += istr(indent) + "node: FUNC_TYPE\n";
	if (!args.empty())
		out += istr(indent) + "args:\n";
	for (size_t i = 0, n = args.size(); i < n; ++i)
		out += istr(indent+1) + "-\n" + args[i]->to_string(indent+2);
	if (!results.empty())
		out += istr(indent) + "results:\n";
	for (size_t i = 0, n = results.size(); i < n; ++i)
		out += istr(indent+1) + "-\n" + results[i]->to_string(indent+2);
	return out;
}

source_loc_range_t func_type_t::source_loc_range()
{
	// TODO
	source_loc_range_t slk = {pos, pos + 4}; // func keyword
	return slk;
}

bool func_type_t::named_results()
{
	for (size_t i = 0, n = results.size(); i < n; ++i) {
		if (!results[i]->names.empty())
			return true;
	}
	return false;
}

//------------------------------------------------------------------------------
// import_decl_t
//------------------------------------------------------------------------------

import_spec_t::import_spec_t(token_t *i, token_t *p):
	name(0)
{
	if (i)
		name = new ident_expr_t(i);
	path = new basic_lit_expr_t(p);
}

import_spec_t::~import_spec_t()
{
	delete name;
	delete path;
}

std::string import_spec_t::to_string(int indent)
{
	std::string out;
	if (name)
		out += istr(indent) + "name:\n" + name->to_string(indent+1);
	out += istr(indent) + "path:\n" + path->to_string(indent+1);
	return out;
}

import_decl_t::import_decl_t(import_spec_vector_t *s, token_t *t,
			     token_t *l, token_t *r):
	node_t(node_t::IMPORT_DECL), pos(t->pos), pos_lp(l->pos), pos_rp(r->pos)
{
	specs.swap(*s);
	delete s;
	delete t;
	delete l;
	delete r;
}

import_decl_t::import_decl_t(import_spec_t *is, token_t *t):
	node_t(node_t::IMPORT_DECL), pos(t->pos)
{
	specs.push_back(is);
	delete t;
}

import_decl_t::~import_decl_t()
{
	for (size_t i = 0, n = specs.size(); i < n; ++i)
		delete specs[i];
}

std::string import_decl_t::to_string(int indent)
{
	std::string out;
	out += istr(indent) + "node: IMPORT_DECL\n";
	out += istr(indent) + "specs:\n";
	for (size_t i = 0, n = specs.size(); i < n; ++i) {
		out += istr(indent+1) + "-\n";
		out += specs[i]->to_string(indent+2);
	}
	return out;
}

source_loc_range_t import_decl_t::source_loc_range()
{
	source_loc_range_t slk = {pos, 0};
	if (specs.size() == 1) {
		source_loc_range_t e = specs[0]->path->source_loc_range();
		slk.end = e.end;
	} else
		slk.end = pos_rp;
	return slk;
}

//------------------------------------------------------------------------------
// type_decl_t
//------------------------------------------------------------------------------

type_spec_t::type_spec_t(token_t *i, node_t *t):
	type(t)
{
	name = new ident_expr_t(i);
}

type_spec_t::~type_spec_t()
{
	delete name;
	delete type;
}

std::string type_spec_t::to_string(int indent)
{
	std::string out;
	out += istr(indent) + "name:\n" + name->to_string(indent+1);
	out += istr(indent) + "type:\n" + type->to_string(indent+1);
	return out;
}

type_decl_t::type_decl_t(type_spec_vector_t *s, token_t *t,
			 token_t *l, token_t *r):
	node_t(node_t::TYPE_DECL), pos(t->pos), pos_lp(l->pos), pos_rp(r->pos)
{
	specs.swap(*s);
	delete s;
	delete t;
	delete l;
	delete r;
}

type_decl_t::type_decl_t(type_spec_t *ts, token_t *t):
	node_t(node_t::TYPE_DECL), pos(t->pos)
{
	specs.push_back(ts);
	delete t;
}

type_decl_t::~type_decl_t()
{
	for (size_t i = 0, n = specs.size(); i < n; ++i)
		delete specs[i];
}

std::string type_decl_t::to_string(int indent)
{
	std::string out;
	out += istr(indent) + "node: TYPE_DECL\n";
	out += istr(indent) + "specs:\n";
	for (size_t i = 0, n = specs.size(); i < n; ++i)
		out += istr(indent+1) + "-\n" + specs[i]->to_string(indent+2);
	return out;
}

source_loc_range_t type_decl_t::source_loc_range()
{
	source_loc_range_t slk = {pos, 0};
	if (specs.size() == 1) {
		source_loc_range_t e = specs[0]->type->source_loc_range();
		slk.end = e.end;
	} else
		slk.end = pos_rp;
	return slk;
}

//------------------------------------------------------------------------------
// value_spec_t
//------------------------------------------------------------------------------

value_spec_t::value_spec_t(ident_expr_vector_t *n, node_t *t, node_vector_t *v,
			   token_t *assign):
	type(0)
{
	names.swap(*n);
	delete n;
	if (t)
		type = t;
	if (v) {
		values.swap(*v);
		delete v;
		pos = assign->pos;
		delete assign;
	}
}

value_spec_t::~value_spec_t()
{
	for (size_t i = 0, n = names.size(); i < n; ++i)
		delete names[i];
	for (size_t i = 0, n = values.size(); i < n; ++i)
		delete values[i];
	delete type;
}

std::string value_spec_t::to_string(int indent)
{
	std::string out;
	if (!names.empty()) {
		out += istr(indent) + "names:\n";
		for (size_t i = 0, n = names.size(); i < n; ++i) {
			out += istr(indent+1) + "-\n";
			out += names[i]->to_string(indent+2);
		}
	}
	if (type)
		out += istr(indent) + "type:\n" + type->to_string(indent+1);
	if (!values.empty()) {
		out += istr(indent) + "values:\n";
		for (size_t i = 0, n = values.size(); i < n; ++i) {
			out += istr(indent+1) + "-\n";
			out += values[i]->to_string(indent+2);
		}
	}
	return out;
}

static std::string value_decl_to_string(value_spec_vector_t &specs, int indent)
{
	std::string out;
	out += istr(indent) + "specs:\n";
	for (size_t i = 0, n = specs.size(); i < n; ++i) {
		out += istr(indent+1) + "-\n";
		out += specs[i]->to_string(indent+2);
	}
	return out;
}

source_loc_t value_spec_t::source_loc_end()
{
	if (!values.empty()) {
		source_loc_range_t e = values.back()->source_loc_range();
		return e.end;
	}
	if (type) {
		source_loc_range_t e = type->source_loc_range();
		return e.end;
	}

	source_loc_range_t e = names.back()->source_loc_range();
	return e.end;
}

//------------------------------------------------------------------------------
// const_decl_t
//------------------------------------------------------------------------------

const_decl_t::const_decl_t(value_spec_vector_t *s, token_t *t,
			   token_t *l, token_t *r):
	node_t(node_t::CONST_DECL), pos(t->pos), pos_lp(l->pos), pos_rp(r->pos)
{
	specs.swap(*s);
	delete s;
	delete t;
	delete l;
	delete r;
}

const_decl_t::const_decl_t(value_spec_t *vs, token_t *t):
	node_t(node_t::CONST_DECL), pos(t->pos)
{
	specs.push_back(vs);
	delete t;
}

const_decl_t::~const_decl_t()
{
	for (size_t i = 0, n = specs.size(); i < n; ++i)
		delete specs[i];
}

std::string const_decl_t::to_string(int indent)
{
	std::string out;
	out += istr(indent) + "node: CONST_DECL\n";
	out += value_decl_to_string(specs, indent);
	return out;
}

source_loc_range_t const_decl_t::source_loc_range()
{
	source_loc_range_t slk = {pos, 0};
	if (specs.size() == 1) {
		source_loc_t e = specs[0]->source_loc_end();
		slk.end = e;
	} else
		slk.end = pos_rp;
	return slk;
}

//------------------------------------------------------------------------------
// var_decl_t
//------------------------------------------------------------------------------

var_decl_t::var_decl_t(value_spec_vector_t *s, token_t *t,
		       token_t *l, token_t *r):
	node_t(node_t::VAR_DECL), pos(t->pos), pos_lp(l->pos), pos_rp(r->pos)
{
	specs.swap(*s);
	delete s;
	delete t;
	delete l;
	delete r;
}

var_decl_t::var_decl_t(value_spec_t *vs, token_t *t):
	node_t(node_t::VAR_DECL), pos(t->pos)
{
	specs.push_back(vs);
	delete t;
}

var_decl_t::~var_decl_t()
{
	for (size_t i = 0, n = specs.size(); i < n; ++i)
		delete specs[i];
}

std::string var_decl_t::to_string(int indent)
{
	std::string out;
	out += istr(indent) + "node: VAR_DECL\n";
	out += value_decl_to_string(specs, indent);
	return out;
}

source_loc_range_t var_decl_t::source_loc_range()
{
	source_loc_range_t slk = {pos, 0};
	if (specs.size() == 1) {
		source_loc_t e = specs[0]->source_loc_end();
		slk.end = e;
	} else
		slk.end = pos_rp;
	return slk;
}

//------------------------------------------------------------------------------
// func_decl_t
//------------------------------------------------------------------------------

func_decl_t::func_decl_t(ident_expr_t *n, func_type_t *t, block_stmt_t *b):
	node_t(node_t::FUNC_DECL), ftype(t), name(n), body(b)
{
}

func_decl_t::~func_decl_t()
{
	delete ftype;
	delete name;
	delete body;
}

std::string func_decl_t::to_string(int indent)
{
	std::string out;
	out += istr(indent) + "node: FUNC_DECL\n";
	out += istr(indent) + "name:\n";
	out += name->to_string(indent+1);
	if (!ftype->args.empty())
		out += istr(indent) + "args:\n";
	for (size_t i = 0, n = ftype->args.size(); i < n; ++i) {
		out += istr(indent+1) + "-\n";
		out += ftype->args[i]->to_string(indent+2);
	}
	if (!ftype->results.empty())
		out += istr(indent) + "results:\n";
	for (size_t i = 0, n = ftype->results.size(); i < n; ++i) {
		out += istr(indent+1) + "-\n";
		out += ftype->results[i]->to_string(indent+2);
	}

	if (body)
		out += istr(indent) + "body:\n" + body->to_string(indent+1);
	return out;
}

source_loc_range_t func_decl_t::source_loc_range()
{
	source_loc_range_t slk = {ftype->pos, body->pos_rcb};
	return slk;
}

//==============================================================================

ast_visitor_t::~ast_visitor_t() {}

void ast_visitor_t::traverse(node_t *ast)
{
	if (!ast)
		return;

	ast_visitor_t *v = visit(ast);
	if (!v)
		return;

#define NODE(type) type node = (type)ast
#define TFIELD(field) v->traverse(node->field)
#define TARRAY(array) \
do {								\
	for (size_t i = 0, n = node->array.size(); i < n; ++i)	\
		v->traverse(node->array[i]);			\
} while(0)

	switch (ast->type) {
	case node_t::PROGRAM:
	{
		NODE(program_t*);
		TARRAY(decls);
		break;
	}
	case node_t::COMPOUND_LIT:
	{
		NODE(compound_lit_t*);
		TARRAY(elts);
		break;
	}
	case node_t::EXPR_STMT:
	{
		NODE(expr_stmt_t*);
		TFIELD(expr);
		break;
	}
	case node_t::DECL_STMT:
	{
		NODE(decl_stmt_t*);
		TFIELD(decl);
		break;
	}
	case node_t::INCDEC_STMT:
	{
		NODE(incdec_stmt_t*);
		TFIELD(expr);
		break;
	}
	case node_t::ASSIGN_STMT:
	{
		NODE(assign_stmt_t*);
		TARRAY(lhs);
		TARRAY(rhs);
		break;
	}
	case node_t::BLOCK_STMT:
	{
		NODE(block_stmt_t*);
		TARRAY(stmts);
		break;
	}
	case node_t::RETURN_STMT:
	{
		NODE(return_stmt_t*);
		TARRAY(returns);
		break;
	}
	case node_t::IFELSE_STMT:
	{
		NODE(ifelse_stmt_t*);
		TFIELD(cond);
		TFIELD(body);
		TFIELD(elsestmt);
		break;
	}
	case node_t::FOR_STMT:
	{
		NODE(for_stmt_t*);
		TFIELD(init);
		TFIELD(cond);
		TFIELD(post);
		TFIELD(body);
		break;
	}
	case node_t::SWITCH_STMT_CASE:
	{
		NODE(switch_stmt_case_t*);
		TARRAY(exprs);
		TARRAY(stmts);
		break;
	}
	case node_t::SWITCH_STMT:
	{
		NODE(switch_stmt_t*);
		TFIELD(expr);
		TFIELD(body);
		break;
	}
	case node_t::BINARY_EXPR:
	{
		NODE(binary_expr_t*);
		TFIELD(lhs);
		TFIELD(rhs);
		break;
	}
	case node_t::UNARY_EXPR:
	{
		NODE(unary_expr_t*);
		TFIELD(expr);
		break;
	}
	case node_t::SELECTOR_EXPR:
	{
		NODE(selector_expr_t*);
		TFIELD(expr);
		TFIELD(sel);
		break;
	}
	case node_t::INDEX_EXPR:
	{
		NODE(index_expr_t*);
		TFIELD(expr);
		TFIELD(index);
		break;
	}
	case node_t::CALL_EXPR:
	{
		NODE(call_expr_t*);
		TFIELD(expr);
		TARRAY(args);
		break;
	}
	case node_t::PAREN_EXPR:
	{
		NODE(paren_expr_t*);
		TFIELD(expr);
		break;
	}
	case node_t::ARRAY_TYPE:
	{
		NODE(array_type_t*);
		TFIELD(size);
		TFIELD(etype);
		break;
	}
	case node_t::POINTER_TYPE:
	{
		NODE(pointer_type_t*);
		TFIELD(ptype);
		break;
	}
	case node_t::TYPE_CAST_EXPR:
	{
		NODE(type_cast_expr_t*);
		TFIELD(ctype);
		TFIELD(expr);
		break;
	}
	case node_t::TYPE_EXPR:
	{
		NODE(type_expr_t*);
		TFIELD(etype);
		break;
	}
	case node_t::STRUCT_TYPE:
	{
		NODE(struct_type_t*);
		for (size_t i = 0, n = node->fields.size(); i < n; ++i) {
			field_t *f = node->fields[i];
			for (size_t j = 0, m = f->names.size(); j < m; ++j)
				v->traverse(f->names[j]);
			v->traverse(f->type);
		}
		break;
	}
	case node_t::FUNC_TYPE:
	{
		NODE(func_type_t*);
		for (size_t i = 0, n = node->args.size(); i < n; ++i) {
			field_t *f = node->args[i];
			for (size_t j = 0, m = f->names.size(); j < m; ++j)
				v->traverse(f->names[j]);
			v->traverse(f->type);
		}
		for (size_t i = 0, n = node->results.size(); i < n; ++i) {
			field_t *f = node->results[i];
			for (size_t j = 0, m = f->names.size(); j < m; ++j)
				v->traverse(f->names[j]);
			v->traverse(f->type);
		}
		break;
	}
	case node_t::IMPORT_DECL:
	{
		NODE(import_decl_t*);
		for (size_t i = 0, n = node->specs.size(); i < n; ++i) {
			import_spec_t *s = node->specs[i];
			v->traverse(s->name);
			v->traverse(s->path);
		}
		break;
	}
	case node_t::TYPE_DECL:
	{
		NODE(type_decl_t*);
		for (size_t i = 0, n = node->specs.size(); i < n; ++i) {
			type_spec_t *s = node->specs[i];
			v->traverse(s->name);
			v->traverse(s->type);
		}
		break;
	}
	case node_t::CONST_DECL:
	{
		NODE(const_decl_t*);
		for (size_t i = 0, n = node->specs.size(); i < n; ++i) {
			value_spec_t *s = node->specs[i];
			for (size_t j = 0, m = s->names.size(); j < m; ++j)
				v->traverse(s->names[j]);
			for (size_t j = 0, m = s->values.size(); j < m; ++j)
				v->traverse(s->values[j]);
			v->traverse(s->type);
		}
		break;
	}
	case node_t::VAR_DECL:
	{
		NODE(var_decl_t*);
		for (size_t i = 0, n = node->specs.size(); i < n; ++i) {
			value_spec_t *s = node->specs[i];
			for (size_t j = 0, m = s->names.size(); j < m; ++j)
				v->traverse(s->names[j]);
			for (size_t j = 0, m = s->values.size(); j < m; ++j)
				v->traverse(s->values[j]);
			v->traverse(s->type);
		}
		break;
	}
	case node_t::FUNC_DECL:
	{
		NODE(func_decl_t*);
		TFIELD(ftype);
		TFIELD(name);
		TFIELD(body);
		break;
	}
	case node_t::BASIC_LIT_EXPR:
	case node_t::IDENT_EXPR:
	case node_t::FLOW_STMT:
		break;
	}
}

call_expr_t *is_call_expr(node_t *n)
{
	switch (n->type) {
	case node_t::CALL_EXPR:
		return (call_expr_t*)n;
	case node_t::PAREN_EXPR:
		return is_call_expr(((paren_expr_t*)n)->expr);
	default:
		break;
	}
	return 0;
}

ident_expr_t *is_ident_expr(node_t *n)
{
	switch (n->type) {
	case node_t::IDENT_EXPR:
		return (ident_expr_t*)n;
	case node_t::PAREN_EXPR:
		return is_ident_expr(((paren_expr_t*)n)->expr);
	default:
		break;
	}
	return 0;
}

int compound_assignment_to_binary_tok(int tok)
{
	switch (tok) {
	case TOK_A_DIVIDE: return TOK_DIVIDE;
	case TOK_A_TIMES:  return TOK_TIMES;
	case TOK_A_MOD:    return TOK_MOD;
	case TOK_A_SHIFTL: return TOK_SHIFTL;
	case TOK_A_SHIFTR: return TOK_SHIFTR;
	case TOK_A_AND:    return TOK_AND;
	case TOK_A_ANDNOT: return TOK_ANDNOT;
	case TOK_A_PLUS:   return TOK_PLUS;
	case TOK_A_MINUS:  return TOK_MINUS;
	case TOK_A_OR:     return TOK_OR;
	case TOK_A_XOR:    return TOK_XOR;
	default:
		CRAWL_QASSERT(!"fail");
		break;
	}
	return 0;
}
