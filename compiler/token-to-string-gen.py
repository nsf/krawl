#!/usr/bin/env python

from string import Template
import re

def read_file(fname, m='r'):
	f = open(fname, m)
	try:
		return f.read()
	finally:
		f.close()

tpl = Template("""
#include "tokens.hpp"

const char *token_to_string(int tok)
{
	switch (tok) {
$tokens
	}
	return "???";
}
""".strip())

toks = re.findall(r'TOK_[A-Z_]+', read_file('tokens.hpp'))
toks = '\n'.join(['\tcase %s: return "%s";' % (t, t) for t in toks])
print(tpl.substitute(tokens=toks))
