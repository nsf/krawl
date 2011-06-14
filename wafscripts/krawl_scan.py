import re, collections
from waflib import Utils, Errors
import traceback

Tok = collections.namedtuple('Tok', 'tok value pos')
ImportSpec = collections.namedtuple('ImportSpec', 'name path')

class ParserError(Exception):
	def __init__(self, expected, t):
		self.value = 'expected %s, got: %s' % (expected, t.tok)
	def __str__(self):
		return repr(self.value)

def post_process_path(path):
	# TODO perform escape sequences interpretation here as well
	# rarely (never?) used in paths though

	# remove quotes
	return path[1:-1]

def token_iter(source):
	pos = source.find("import")
	if pos == -1:
		return

	tok_spec = [
		('ID', r'[A-Za-z]+'),
		('LPAREN', r'\('),
		('RPAREN', r'\)'),
		('STRING', r'("([^"\\]|\\.)*"|`[^`]*`)'),
		('NEWLINE', r'\n'),
		('SKIP', r'[ \t\r]|//.*?\n|/\*.*?\*/'),
	]

	tok_re = '|'.join('(?P<%s>%s)' % spec for spec in tok_spec)
	gettok = re.compile(tok_re).match

	lasttok = None

	mo = gettok(source, pos)
	while mo is not None:
		tok = mo.lastgroup
		pos = mo.end()

		# automatic semicolon insertion
		if tok == 'NEWLINE' and lasttok in ['STRING', 'RAW_STRING', 'RPAREN']:
			yield Tok('SEMI', ';', mo.start())
			lasttok = 'SEMI'

		# got a real token
		if tok not in ['SKIP', 'NEWLINE']:
			yield Tok(tok, mo.group(), mo.start())
			lasttok = tok

		mo = gettok(source, pos)

def next_expected(it, expected, value=None):
	t = next(it)
	if t.tok != expected or (value and t.value != value):
		raise ParserError(expected, t)
	return t

def parse_import_spec(t, it):
	name = ""
	path = ""
	if t.tok == 'ID':
		# got an ID, the next one should be the path
		name = t.value
		t = next_expected(it, 'STRING')
	# got path, consume semicolon
	path = post_process_path(t.value)
	t = next_expected(it, 'SEMI')
	return ImportSpec(name, path), t.pos

def parse_import_group(it):
	specs = []
	while 1:
		t = next(it)
		if t.tok in ['ID', 'STRING']:
			specs.append(parse_import_spec(t, it)[0])
		elif t.tok == 'RPAREN':
			t = next_expected(it, 'SEMI')
			return specs, t.pos
		else:
			raise ParserError('RPAREN or ID or STRING', t)

def parse_import_spec_or_group(it):
	t = next(it)
	if t.tok == 'LPAREN':
		return parse_import_group(it)
	elif t.tok in ['ID', 'STRING']:
		s, pos = parse_import_spec(t, it)
		return [s], pos
	raise ParserError('LPAREN or ID or STRING', t)

def parse_import_specs(source):
	specs = []
	while 1:
		try:
			it = token_iter(source)
			next_expected(it, 'ID', 'import')
			ss, pos = parse_import_spec_or_group(it)
			specs.extend(ss)
			source = source[pos:]
		except StopIteration:
			return specs

def try_find(path, incnodes):
	for n in incnodes:
		ret = n.find_resource(path + ".brl")
		if ret:
			return ret
	return None

def scan(task):
	try:
		incnodes = task.generator.includes_nodes
	except AttributeError:
		raise Error.WafError("%r must execute apply_incpaths before doing scan" % task.generator)

	nodes = []
	for i in task.inputs:
		filename = i.abspath()
		code = Utils.readf(filename)
		try:
			specs = parse_import_specs(code)
		except ParserError:
			specs = []

		for s in specs:
			if s.path[-2:] == '.h' or s.path[:2] == './':
				continue
			n = try_find(s.path, incnodes)
			if n:
				nodes.append(n)
	return (nodes, [])



