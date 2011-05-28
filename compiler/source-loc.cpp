#include "crawl.hpp"
#include <algorithm>
#include <stdio.h>

//------------------------------------------------------------------------------
// source_pos_t
//------------------------------------------------------------------------------

unsigned int source_pos_t::col(char *input)
{
	unsigned int col = 1;
	char *c = input + line_offset;
	char *e = input + offset;
	while (c < e) {
		size_t len = utf8_char_length(*c);
		c += len;
		col++;
	}
	return col;
}

//------------------------------------------------------------------------------
// source_file_t
//------------------------------------------------------------------------------

static bool source_line_offset_cmp(const source_line_t &l, const source_line_t &r)
{
	return l.offset <= r.offset;
}

source_file_t::source_file_t()
{
	source_line_t firstline = {0, 1};
	lines.push_back(firstline);
}

source_line_t *source_file_t::get_line_by_loc(source_loc_t offset)
{
	std::vector<source_line_t>::iterator it;
	source_line_t proto;
	proto.offset = offset - this->offset;

	it = std::lower_bound(lines.begin(), lines.end(), proto,
			      source_line_offset_cmp);
	it--;
	return &*it;
}

source_pos_t source_file_t::get_pos_by_loc(source_loc_t offset)
{
	source_pos_t out;
	source_line_t *line = get_line_by_loc(offset);

	out.filename = filename.c_str();
	out.offset = offset - this->offset;
	out.line = line->line;
	out.line_offset = line->offset;
	return out;
}

void source_file_t::add_line(unsigned int offset)
{
	source_line_t newline = {offset, 1};
	if (!lines.empty())
		newline.line = lines.back().line+1;

	lines.push_back(newline);
}

//------------------------------------------------------------------------------
// source_group_t
//------------------------------------------------------------------------------

static bool source_file_offset_cmp(const source_file_t *l, const source_file_t *r)
{
	return l->offset <= r->offset;
}

source_group_t::~source_group_t()
{
	for (size_t i = 0, n = files.size(); i < n; ++i)
		delete files[i];
}

source_file_t *source_group_t::get_file_by_name(const char *filename)
{
	for (size_t i = 0, n = files.size(); i < n; ++i) {
		if (files[i]->filename == filename)
			return files[i];
	}
	return 0;
}

source_file_t *source_group_t::get_file_by_loc(source_loc_t offset)
{
	std::vector<source_file_t*>::iterator it;
	source_file_t proto;
	proto.offset = offset;

	it = std::lower_bound(files.begin(), files.end(), &proto,
			      source_file_offset_cmp);
	it--;
	return *it;
}

source_pos_t source_group_t::get_pos_by_loc(source_loc_t offset)
{
	source_file_t *f = get_file_by_loc(offset);
	if (!f) {
		source_pos_t empty = {"", 0, 0, 0};
		return empty;
	}

	return f->get_pos_by_loc(offset);
}

source_file_t *source_group_t::add_file(const char *filename,
					std::vector<char> *data)
{
	source_file_t *f = new source_file_t;
	f->filename = filename;
	f->offset = 0;
	if (!files.empty()) {
		source_file_t *last = files.back();
		f->offset = last->offset + last->data.size();
	}
	f->data.swap(*data);

	files.push_back(f);
	return f;
}
