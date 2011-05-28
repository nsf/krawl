// vim: filetype=ragel

#include "crawl.hpp"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

%%{
	machine crawl;

	action append_oct { append_oct(fpc-2); }
	action append_hex2 { append_hex(fpc-1, 2); }
	action append_hex4 { append_hex(fpc-3, 4); }
	action append_hex8 { append_hex(fpc-7, 8); }
	action append_escaped { append_escaped(fc); }
	action start_interp { interp.clear(); }
	action interp_any { interp.append(1, fc); }

	count_newline = '\n' @{
		if (file)
			file->add_line((fpc+1) - &file->data.front());
	};
	any_count_newline = any | count_newline;
	
	decimal_lit = [1-9] digit*;
	octal_lit = '0' [0-7]*;
	hex_lit = '0x' xdigit+;
	int_lit = decimal_lit | octal_lit | hex_lit;

	exponent = [eE] [+\-]? digit+;
	float_lit = digit+ '.' digit* exponent? | digit+ exponent | '.' digit+ exponent?;

	# hack for integer type casts '5.(int)'
	# TODO: skip whitespaces between '.' and '('
	int_type_cast = digit+ '.' '(';

	# different interpretation variants, all start with '\'
	octal_byte_value = '\\' [0-7] {3} @append_oct;
	hex_byte_value = '\\x' xdigit {2} @append_hex2;
	little_u_value = '\\u' xdigit {4} @append_hex4;
	big_u_value    = '\\U' xdigit {8} @append_hex8;
	escaped_char = '\\' [abfnrtv\\'"] @append_escaped;

	interpreted = octal_byte_value | hex_byte_value |
		      little_u_value | big_u_value | escaped_char;

	alnum_u = alnum | '_';
	alpha_u = alpha | '_';

	main := |*
		#------------------------------------------------------------------------------
		# keywords
		#------------------------------------------------------------------------------
	
		'if'          { tok_op(TOK_IF);          fbreak; };
		'else'        { tok_op(TOK_ELSE);        fbreak; };
		'for'         { tok_op(TOK_FOR);         fbreak; };
		'switch'      { tok_op(TOK_SWITCH);      fbreak; };
		'return'      { tok_op(TOK_RETURN);      fbreak; };
		'continue'    { tok_op(TOK_CONTINUE);    fbreak; };
		'fallthrough' { tok_op(TOK_FALLTHROUGH); fbreak; };
		'break'       { tok_op(TOK_BREAK);       fbreak; };
		'case'        { tok_op(TOK_CASE);        fbreak; };
		'default'     { tok_op(TOK_DEFAULT);     fbreak; };

		'func'        { tok_op(TOK_FUNC);        fbreak; };
		'type'        { tok_op(TOK_TYPE);        fbreak; };
		'import'      { tok_op(TOK_IMPORT);      fbreak; };
		'const'       { tok_op(TOK_CONST);       fbreak; };
		'var'         { tok_op(TOK_VAR);         fbreak; };
		'struct'      { tok_op(TOK_STRUCT);      fbreak; };
		
		#------------------------------------------------------------------------------
		# operators
		#------------------------------------------------------------------------------
		
		'...' { tok_op(TOK_ELLIPSIS);  fbreak; };

		'++'  { tok_op(TOK_INC);       fbreak; };
		'--'  { tok_op(TOK_DEC);       fbreak; };
		':='  { tok_op(TOK_DECLARIZE); fbreak; };
		
		'{'   { tok_op(TOK_LCURLY);    fbreak; };
		'}'   { tok_op(TOK_RCURLY);    fbreak; };
		'['   { tok_op(TOK_LSB);       fbreak; };
		']'   { tok_op(TOK_RSB);       fbreak; };
		'('   { tok_op(TOK_LPAREN);    fbreak; };
		')'   { tok_op(TOK_RPAREN);    fbreak; };
		';'   { tok_op(TOK_SEMICOLON); fbreak; };
		':'   { tok_op(TOK_COLON);     fbreak; };
		'.'   { tok_op(TOK_DOT);       fbreak; };
		','   { tok_op(TOK_COMMA);     fbreak; };
		'!'   { tok_op(TOK_NOT);       fbreak; };
		'='   { tok_op(TOK_ASSIGN);    fbreak; };
		
		'*'   { tok_op(TOK_TIMES);     fbreak; };
		'/'   { tok_op(TOK_DIVIDE);    fbreak; };
		'%'   { tok_op(TOK_MOD);       fbreak; };
		'<<'  { tok_op(TOK_SHIFTL);    fbreak; };
		'>>'  { tok_op(TOK_SHIFTR);    fbreak; };
		'&'   { tok_op(TOK_AND);       fbreak; };
		'&^'  { tok_op(TOK_ANDNOT);    fbreak; };
		
		'+'   { tok_op(TOK_PLUS);      fbreak; };
		'-'   { tok_op(TOK_MINUS);     fbreak; };
		'|'   { tok_op(TOK_OR);        fbreak; };
		'^'   { tok_op(TOK_XOR);       fbreak; };

		'*='  { tok_op(TOK_A_TIMES);   fbreak; };
		'/='  { tok_op(TOK_A_DIVIDE);  fbreak; };
		'%='  { tok_op(TOK_A_MOD);     fbreak; };
		'<<=' { tok_op(TOK_A_SHIFTL);  fbreak; };
		'>>=' { tok_op(TOK_A_SHIFTR);  fbreak; };
		'&='  { tok_op(TOK_A_AND);     fbreak; };
		'&^=' { tok_op(TOK_A_ANDNOT);  fbreak; };
		
		'+='  { tok_op(TOK_A_PLUS);    fbreak; };
		'-='  { tok_op(TOK_A_MINUS);   fbreak; };
		'|='  { tok_op(TOK_A_OR);      fbreak; };
		'^='  { tok_op(TOK_A_XOR);     fbreak; };
		
		'=='  { tok_op(TOK_EQ);        fbreak; };
		'!='  { tok_op(TOK_NEQ);       fbreak; };
		'<='  { tok_op(TOK_LE);        fbreak; };
		'>='  { tok_op(TOK_GE);        fbreak; };
		'<'   { tok_op(TOK_LT);        fbreak; };
		'>'   { tok_op(TOK_GT);        fbreak; };
		
		'&&'  { tok_op(TOK_ANDAND);    fbreak; };

		'||'  { tok_op(TOK_OROR);      fbreak; };

		#------------------------------------------------------------------------------
		# comments, whitespaces
		#------------------------------------------------------------------------------

		# skip spaces
		any_count_newline - 0x21..0x7E;

		# skip C++ comments
		'//' [^\n]* count_newline;

		# skip C comments
		'/*' any_count_newline* :>> '*/';

		#------------------------------------------------------------------------------
		# int literals (decimal, octal, hex)
		#------------------------------------------------------------------------------

		int_lit { tok_int(ts, te); fbreak; };

		#------------------------------------------------------------------------------
		# float literal
		#------------------------------------------------------------------------------
			
		float_lit { tok_float(ts, te); fbreak; };

		# hack for integer type casts '5.(int)'
		int_type_cast { 
			tok_int(ts, te-2);
			tok_op_pos(TOK_DOT, te-2, te-1);
			tok_op_pos(TOK_LPAREN, te-1, te);
			fbreak;
		};

		#------------------------------------------------------------------------------
		# raw string, string and char literal
		#------------------------------------------------------------------------------

		# raw strings are quite simple, no interpretation
		'`' any_count_newline* :>> '`' {
			beg = ts; end = te;
			interp.assign(ts+1, (te-ts)-2);
			tok_string(TOK_STRING);
			fbreak;
		};

		# ordinary string uses string interpreter
		'"' @start_interp { beg = fpc; fgoto string_interp; };

		# char
		"'" @start_interp ([^'\\\n] @interp_any | interpreted) "'" {
			beg = ts; end = te;
			tok_string(TOK_CHAR);
			fbreak;
		};

		#------------------------------------------------------------------------------
		# identifier
		#------------------------------------------------------------------------------

		alpha_u alnum_u* { 
			beg = ts; end = te;
			interp.assign(ts, te-ts);
			tok_string(TOK_IDENT);
			fbreak;
		};
	*|;

	#------------------------------------------------------------------------------
	# String interpreter (I just need to use longest-match here with backtracking)
	#
	# In case if there is an error in escape sequence, like: \u123k, the scanner
	# will parse it as an ordinary string without performing escape procedure.
	# Simply handy to use it as a bullet-proof string interpreter.
	#------------------------------------------------------------------------------
	string_interp := |*
		octal_byte_value;
		hex_byte_value;
		little_u_value;
		big_u_value;
		escaped_char;
		[^"\n] => interp_any;
		'"'    => {
			end = te;
			tok_string(TOK_STRING);
			fnext main;
			fbreak;
		};
	*|;
}%%

%% write data;

static uint32_t parse_hex(const char *p, size_t len)
{
	uint32_t out = 0;
	while (len--) {
		char c = *p;
		unsigned char n = 0;
		if (c >= '0' && c <= '9')
			n = c - '0';
		else if (c >= 'a' && c <= 'f')
			n = 10 + (c - 'a');
		else if (c >= 'A' && c <= 'F')
			n = 10 + (c - 'A');
		out = (out << 4) | n;
		p++;
	}
	return out;
}

static uint32_t parse_oct(const char *p)
{
	uint32_t a, b, c;
	a = p[0] - '0';
	b = p[1] - '0';
	c = p[2] - '0';
	return (a << 6) + (b << 3) + c;
}

//------------------------------------------------------------------------------

void lexer_t::set_input(source_file_t *f)
{
	beg       = 0;
	end       = 0;
	file      = f;
	p         = &file->data.front();
	pe        = &file->data.back();
	eof       = pe;

	%% write init;
}

token_t *lexer_t::next_token()
{
	token_t *n = 0;
	if (!next.empty()) {
		n = next.back();
		next.pop_back();
		return n;
	}

	%% write exec;

	if (!next.empty()) {
		n = next.back();
		next.pop_back();
	}
	return n;
}

// interp functions
void lexer_t::append_hex(const char *p, size_t len)
{
	char utfbuf[8];
	uint32_t hex = parse_hex(p, len);
	len = unicode_to_utf8(utfbuf, hex);
	interp.append(utfbuf, len);
}

void lexer_t::append_oct(const char *p)
{
	char utfbuf[8];
	uint32_t oct = parse_oct(p);
	size_t len = unicode_to_utf8(utfbuf, oct);
	interp.append(utfbuf, len);
}

void lexer_t::append_escaped(const char c)
{
	switch (c) {
	case '"':  interp.append(1, '"');  break;
	case 'a':  interp.append(1, '\a'); break;
	case 'b':  interp.append(1, '\b'); break;
	case 'f':  interp.append(1, '\f'); break;
	case 'n':  interp.append(1, '\n'); break;
	case 'r':  interp.append(1, '\r'); break;
	case 't':  interp.append(1, '\t'); break;
	case 'v':  interp.append(1, '\v'); break;
	case '\\': interp.append(1, '\\'); break;
	case '\'': interp.append(1, '\''); break;
	}
}

void lexer_t::tok_int(char *b, char *e)
{
	beg = b; end = e;
	interp.assign(b, e-b);
	token_t *n = new token_t(interp.c_str(), TOK_INT,
				 loc(), beg, end);
	next.insert(next.begin(), n);
}

void lexer_t::tok_float(char *b, char *e)
{
	beg = b; end = e;
	interp.assign(b, e-b);
	token_t *n = new token_t(interp.c_str(), TOK_FLOAT,
				 loc(), beg, end);
	next.insert(next.begin(), n);
}

void lexer_t::tok_string(int type)
{
	token_t *n = new token_t(interp.c_str(), type,
				 loc(), beg, end);
	next.insert(next.begin(), n);
}

void lexer_t::tok_op(int optype)
{
	beg = ts; end = te;
	token_t *n = new token_t(0, optype, loc(), beg, end);
	next.insert(next.begin(), n);
}

void lexer_t::tok_op_pos(int optype, char *b, char *e)
{
	beg = b; end = e;
	token_t *n = new token_t(0, optype, loc(), beg, end);
	next.insert(next.begin(), n);
}

source_loc_t lexer_t::loc()
{
	source_loc_t loc = beg - &file->data.front();
	if (file)
		loc += file->offset;
	return loc;
}
