#include "crawl.hpp"
#include <stdio.h>

int istty = -1;

void cppsprintf(std::string *out, const char *fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	cppvsprintf(out, fmt, va);
	va_end(va);
}

void cppvsprintf(std::string *out, const char *fmt, va_list va)
{
	va_list va2;
	va_copy(va2, va);

	int len = vsnprintf(0, 0, fmt, va);
	size_t size = out->size();
	out->resize(size + len);
	vsnprintf(&(*out)[size], len+1, fmt, va2);

	va_end(va2);
}

static bool in_highlight_range(source_loc_range_t *ranges, size_t ranges_n,
			       source_loc_t offset)
{
	for (size_t i = 0; i < ranges_n; ++i) {
		source_loc_range_t r = ranges[i];
		if (r.beg <= offset && r.end >= offset)
			return true;
	}
	return false;
}

void highlight_code(std::string *out, source_file_t *file, source_loc_t tok,
		    source_loc_range_t *ranges, size_t ranges_n, bool hltok)
{
	source_pos_t tokp = file->get_pos_by_loc(tok);
	char *input_beg = &file->data.front();
	char *input_end = &file->data.back();
	char *b = input_beg + tokp.line_offset;
	char *e = b;
	while (*e != '\n' && e != input_end)
		e++;

	// append line
	out->append(b, e-b);
	out->append(1, '\n');

	// perform highlights
	if (istty)
		out->append(CONSOLE_GREEN);
	while (b < e) {
		size_t clen = utf8_char_length(*b);
		if (clen == 1 && *b == '\t')
			out->append(1, '\t');
		else {
			source_loc_t offset = (b - input_beg) + file->offset;
			if (hltok && tok == offset) {
				if (istty) out->append(CONSOLE_YELLOW);
				out->append(1, '^');
				if (istty) out->append(CONSOLE_GREEN);
			} else if (in_highlight_range(ranges, ranges_n, offset))
				out->append(1, '~');
			else
				out->append(1, ' ');
		}
		b += clen;
	}
	if (istty)
		out->append(CONSOLE_NOCOL);
}

static void append_message_header_nocol(std::string *out, message_type_t type,
					const char *filename, int line, int col)
{
	cppsprintf(out, "%s:%d:%d: ", filename, line, col);
	switch (type) {
	case MESSAGE_ERROR:
		out->append("error: ");
		break;
	case MESSAGE_WARNING:
		out->append("warning: ");
		break;
	}
}

static void append_message_header(std::string *out, message_type_t type,
				  const char *filename, int line, int col)
{
	if (!istty) {
		append_message_header_nocol(out, type, filename, line, col);
		return;
	}
	cppsprintf(out, CONSOLE_WHITE "%s:%d:%d: " CONSOLE_NOCOL,
		   filename, line, col);
	switch (type) {
	case MESSAGE_ERROR:
		out->append(CONSOLE_RED "error: " CONSOLE_NOCOL);
		break;
	case MESSAGE_WARNING:
		out->append(CONSOLE_MAGENTA "warning: " CONSOLE_NOCOL);
		break;
	}
}

message_t *new_message(message_type_t type,
		       source_loc_t tok, bool hltok,
		       source_loc_range_t *ranges, size_t ranges_n,
		       const char *fmt, ...)
{
	message_t *m = new message_t;
	m->type = type;
	m->location = tok;
	m->ranges.assign(ranges, ranges+ranges_n);
	m->hltok = hltok;

	va_list va;
	va_start(va, fmt);
	cppvsprintf(&m->text, fmt, va);
	va_end(va);
	return m;
}

void message_t::print_to_stderr(source_group_t *group)
{
	// ugly hack, check whether stderr is a tty, disable colors if so
	if (istty == -1)
		istty = isatty(fileno(stderr));
	source_file_t *f = group->get_file_by_loc(location);
	source_pos_t ptok = f->get_pos_by_loc(location);

	std::string out;
	append_message_header(&out, type, ptok.filename, ptok.line,
			      ptok.col(&f->data.front()));
	if (istty)
		out.append(CONSOLE_WHITE);
	out.append(text);
	if (istty)
		out.append(CONSOLE_NOCOL);
	out.append("\n");
	highlight_code(&out, f, location, &ranges[0], ranges.size(), hltok);
	fprintf(stderr, "%s\n", out.c_str());
}

diagnostic_t::~diagnostic_t()
{
	for (size_t i = 0, n = messages.size(); i < n; ++i)
		delete messages[i];
}

void diagnostic_t::report(message_t *m)
{
	messages.push_back(m);
}

bool diagnostic_t::empty()
{
	return messages.empty();
}

void diagnostic_t::print_to_stderr(source_group_t *srcinfo)
{
	for (size_t i = 0, n = messages.size(); i < n; ++i)
		messages[i]->print_to_stderr(srcinfo);
}
