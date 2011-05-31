#include "tokens.hpp"

const char *token_to_string(int tok)
{
	switch (tok) {
	case TOK_OROR: return "TOK_OROR";
	case TOK_ANDAND: return "TOK_ANDAND";
	case TOK_EQ: return "TOK_EQ";
	case TOK_NEQ: return "TOK_NEQ";
	case TOK_LT: return "TOK_LT";
	case TOK_LE: return "TOK_LE";
	case TOK_GT: return "TOK_GT";
	case TOK_GE: return "TOK_GE";
	case TOK_PLUS: return "TOK_PLUS";
	case TOK_MINUS: return "TOK_MINUS";
	case TOK_OR: return "TOK_OR";
	case TOK_XOR: return "TOK_XOR";
	case TOK_DIVIDE: return "TOK_DIVIDE";
	case TOK_TIMES: return "TOK_TIMES";
	case TOK_MOD: return "TOK_MOD";
	case TOK_SHIFTL: return "TOK_SHIFTL";
	case TOK_SHIFTR: return "TOK_SHIFTR";
	case TOK_AND: return "TOK_AND";
	case TOK_ANDNOT: return "TOK_ANDNOT";
	case TOK_ASSIGN: return "TOK_ASSIGN";
	case TOK_DECLARIZE: return "TOK_DECLARIZE";
	case TOK_A_DIVIDE: return "TOK_A_DIVIDE";
	case TOK_A_TIMES: return "TOK_A_TIMES";
	case TOK_A_MOD: return "TOK_A_MOD";
	case TOK_A_SHIFTL: return "TOK_A_SHIFTL";
	case TOK_A_SHIFTR: return "TOK_A_SHIFTR";
	case TOK_A_AND: return "TOK_A_AND";
	case TOK_A_ANDNOT: return "TOK_A_ANDNOT";
	case TOK_A_PLUS: return "TOK_A_PLUS";
	case TOK_A_MINUS: return "TOK_A_MINUS";
	case TOK_A_OR: return "TOK_A_OR";
	case TOK_A_XOR: return "TOK_A_XOR";
	case TOK_INC: return "TOK_INC";
	case TOK_DEC: return "TOK_DEC";
	case TOK_SEMICOLON: return "TOK_SEMICOLON";
	case TOK_BREAK: return "TOK_BREAK";
	case TOK_CONTINUE: return "TOK_CONTINUE";
	case TOK_FALLTHROUGH: return "TOK_FALLTHROUGH";
	case TOK_RETURN: return "TOK_RETURN";
	case TOK_IF: return "TOK_IF";
	case TOK_ELSE: return "TOK_ELSE";
	case TOK_FOR: return "TOK_FOR";
	case TOK_SWITCH: return "TOK_SWITCH";
	case TOK_LCURLY: return "TOK_LCURLY";
	case TOK_RCURLY: return "TOK_RCURLY";
	case TOK_CASE: return "TOK_CASE";
	case TOK_COLON: return "TOK_COLON";
	case TOK_DEFAULT: return "TOK_DEFAULT";
	case TOK_IMPORT: return "TOK_IMPORT";
	case TOK_LPAREN: return "TOK_LPAREN";
	case TOK_RPAREN: return "TOK_RPAREN";
	case TOK_TYPE: return "TOK_TYPE";
	case TOK_CONST: return "TOK_CONST";
	case TOK_VAR: return "TOK_VAR";
	case TOK_FUNC: return "TOK_FUNC";
	case TOK_IDENT: return "TOK_IDENT";
	case TOK_STRING: return "TOK_STRING";
	case TOK_LSB: return "TOK_LSB";
	case TOK_RSB: return "TOK_RSB";
	case TOK_DOT: return "TOK_DOT";
	case TOK_STRUCT: return "TOK_STRUCT";
	case TOK_ELLIPSIS: return "TOK_ELLIPSIS";
	case TOK_COMMA: return "TOK_COMMA";
	case TOK_INT: return "TOK_INT";
	case TOK_FLOAT: return "TOK_FLOAT";
	case TOK_CHAR: return "TOK_CHAR";
	case TOK_NOT: return "TOK_NOT";
	}
	return "???";
}
