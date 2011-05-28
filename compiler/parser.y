%token_type { token_t* }
%token_destructor { delete $$; }

%left OROR.
%left ANDAND.
%left EQ NEQ LT LE GT GE.
%left PLUS MINUS OR XOR.
%left DIVIDE TIMES MOD SHIFTL SHIFTR AND ANDNOT.

%include {
	#include "crawlc.hpp"
	#include <stdlib.h>
	#include <stdio.h>
	#include <assert.h>
}

%token_prefix TOK_

%extra_argument { parser_t *ctx }

%syntax_error {
	message_t *m;
	m = new_message(MESSAGE_ERROR,
			ctx->last_pos, true, 0, 0,
			"unexpected token: %s",
			token_to_string(ctx->last_token));
	ctx->diag->report(m);
}

%type program { program_t* }
program(P) ::= decl_list(L) osemi. {
	P = new program_t(L);
	ctx->ast = P;
}




//------------------------------------------------------------------------------
// Statement list
//------------------------------------------------------------------------------

%type stmt_list { node_vector_t* }
stmt_list(L) ::= stmt(S). { 
	L = new node_vector_t;
	if (S) L->push_back(S);
}
stmt_list(L) ::= stmt_list(L2) stmt(S). {
	if (S) L2->push_back(S);
	L = L2;
}




//------------------------------------------------------------------------------
// Simple statement
//------------------------------------------------------------------------------

%type s_stmt { node_t* }
s_stmt(S) ::= .                                        { S = 0; }
s_stmt(S) ::= expr(E).                                 { S = new expr_stmt_t(E); }
s_stmt(S) ::= expr_list(L) ASSIGN(T)    cexpr_list(R). { S = new assign_stmt_t(L, R, T); }
s_stmt(S) ::= expr_list(L) DECLARIZE(T) cexpr_list(R). { S = new assign_stmt_t(L, R, T); }
s_stmt(S) ::= expr_list(L) A_DIVIDE(T)  cexpr_list(R). { S = new assign_stmt_t(L, R, T); }
s_stmt(S) ::= expr_list(L) A_TIMES(T)   cexpr_list(R). { S = new assign_stmt_t(L, R, T); }
s_stmt(S) ::= expr_list(L) A_MOD(T)     cexpr_list(R). { S = new assign_stmt_t(L, R, T); }
s_stmt(S) ::= expr_list(L) A_SHIFTL(T)  cexpr_list(R). { S = new assign_stmt_t(L, R, T); }
s_stmt(S) ::= expr_list(L) A_SHIFTR(T)  cexpr_list(R). { S = new assign_stmt_t(L, R, T); }
s_stmt(S) ::= expr_list(L) A_AND(T)     cexpr_list(R). { S = new assign_stmt_t(L, R, T); }
s_stmt(S) ::= expr_list(L) A_ANDNOT(T)  cexpr_list(R). { S = new assign_stmt_t(L, R, T); }
s_stmt(S) ::= expr_list(L) A_PLUS(T)    cexpr_list(R). { S = new assign_stmt_t(L, R, T); }
s_stmt(S) ::= expr_list(L) A_MINUS(T)   cexpr_list(R). { S = new assign_stmt_t(L, R, T); }
s_stmt(S) ::= expr_list(L) A_OR(T)      cexpr_list(R). { S = new assign_stmt_t(L, R, T); }
s_stmt(S) ::= expr_list(L) A_XOR(T)     cexpr_list(R). { S = new assign_stmt_t(L, R, T); }
s_stmt(S) ::= expr(E) INC(T).                          { S = new incdec_stmt_t(E, T); }
s_stmt(S) ::= expr(E) DEC(T).                          { S = new incdec_stmt_t(E, T); }




//------------------------------------------------------------------------------
// Statement
//------------------------------------------------------------------------------

%type stmt { node_t* }
stmt(S) ::= s_stmt(SS) SEMICOLON.                { S = SS; }
stmt(S) ::= decl(D).                             { S = new decl_stmt_t(D); }
stmt(S) ::= block_stmt(BS).                      { S = BS; }
stmt(S) ::= BREAK(TOK).                          { S = new flow_stmt_t(TOK); }
stmt(S) ::= CONTINUE(TOK).                       { S = new flow_stmt_t(TOK); }
stmt(S) ::= FALLTHROUGH(TOK).                    { S = new flow_stmt_t(TOK); }
stmt(S) ::= RETURN(TOK) cexpr_list(L) SEMICOLON. { S = new return_stmt_t(L, TOK); }
stmt(S) ::= RETURN(TOK) SEMICOLON.               { S = new return_stmt_t(0, TOK); }

stmt(S) ::= IF(TI) expr(E) block_stmt(BS). {
	S = new ifelse_stmt_t(E, BS, TI);
}
stmt(S) ::= IF(TI) expr(E) block_stmt(BS) ELSE(TE) stmt(ES). {
	S = new ifelse_stmt_t(E, BS, TI, TE, ES);
}
stmt(S) ::= FOR(T) s_stmt(I) SEMICOLON oexpr(C) SEMICOLON s_stmt(P) block_stmt(B). {
	S = new for_stmt_t(I, C, P, B, T);
}
stmt(S) ::= FOR(T) expr(C) block_stmt(B). {
	S = new for_stmt_t(0, C, 0, B, T);
}
stmt(S) ::= FOR(T) block_stmt(B). {
	S = new for_stmt_t(0, 0, 0, B, T);
}
stmt(S) ::= SWITCH(T) expr(E) sw_block_stmt(B). {
	S = new switch_stmt_t(E, B, T);
}




//------------------------------------------------------------------------------
// Switch block statement
//------------------------------------------------------------------------------

%type sw_block_stmt { block_stmt_t* }
sw_block_stmt(BS) ::= LCURLY(L) sw_case_list(STMTS) RCURLY(R). {
	BS = new block_stmt_t(STMTS, L, R);
}

%type sw_case_list { node_vector_t* }
sw_case_list(L) ::= sw_case(C).                  { L = new node_vector_t(1, C); }
sw_case_list(L) ::= sw_case_list(L2) sw_case(C). { L2->push_back(C); L = L2; }

%type sw_case { switch_stmt_case_t* }
sw_case(C) ::= CASE(TOK) expr_list(EL) COLON(COL) ostmt_list(SL). {
	C = new switch_stmt_case_t(EL, SL, TOK, COL);
}
sw_case(C) ::= DEFAULT(TOK) COLON(COL) ostmt_list(SL). {
	C = new switch_stmt_case_t(0, SL, TOK, COL);
}




//------------------------------------------------------------------------------
// Block statement
//------------------------------------------------------------------------------

%type block_stmt { block_stmt_t* }
block_stmt(BS) ::= LCURLY(L) ostmt_list(STMTS) RCURLY(R). {
	BS = new block_stmt_t(STMTS, L, R);
}




//------------------------------------------------------------------------------
// Declaration list
//------------------------------------------------------------------------------

%type decl_list { node_vector_t* }
decl_list(L) ::= .                      { L = new node_vector_t; }
decl_list(L) ::= decl_list(L2) decl(D). { L2->push_back(D); L = L2; }




//------------------------------------------------------------------------------
// Declaration
//
// Always starts with a keyword: const, var, func, type.
//
// var x, y = 5, 6
// type X struct { a, b, c int }
// func main(argc int, argv ->->byte) int { return 0 }
//------------------------------------------------------------------------------

%type decl { node_t* }
decl(D) ::= IMPORT(TOK) import_spec(IS) SEMICOLON. {
	D = new import_decl_t(IS, TOK);
}
decl(D) ::= IMPORT(TOK) LPAREN(L) import_spec_list(ISL) osemi RPAREN(R). {
	D = new import_decl_t(ISL, TOK, L, R);
}
decl(D) ::= TYPE(TOK) type_spec(TS) SEMICOLON. {
	D = new type_decl_t(TS, TOK);
}
decl(D) ::= TYPE(TOK) LPAREN(L) type_spec_list(TSL) osemi RPAREN(R). {
	D = new type_decl_t(TSL, TOK, L, R);
}
decl(D) ::= CONST(TOK) value_spec(VS) SEMICOLON. {
	const_decl_t *d = new const_decl_t(VS, TOK);
	syntax_check_const_decl(ctx->diag, d);
	D = d;
}
decl(D) ::= CONST(TOK) LPAREN(L) value_spec_list(VSL) osemi RPAREN(R). {
	const_decl_t *d = new const_decl_t(VSL, TOK, L, R);
	syntax_check_const_decl(ctx->diag, d);
	D = d;
}
decl(D) ::= VAR(TOK) value_spec(VS) SEMICOLON. {
	var_decl_t *d = new var_decl_t(VS, TOK);
	syntax_check_var_decl(ctx->diag, d);
	D = d;
}
decl(D) ::= VAR(TOK) LPAREN(L) value_spec_list(VSL) osemi RPAREN(R). {
	var_decl_t *d = new var_decl_t(VSL, TOK, L, R);
	syntax_check_var_decl(ctx->diag, d);
	D = d;
}
decl(D) ::= FUNC(TOK) ident(N) 
            LPAREN ofield_comma_list(A) RPAREN
	    func_results(R) SEMICOLON.
{
	func_type_t *ftype = new func_type_t(A, R, TOK);
	D = new func_decl_t(N, ftype);
}
decl(D) ::= FUNC(TOK) ident(N) 
            LPAREN ofield_comma_list(A) RPAREN
	    func_results(R) block_stmt(B).
{
	func_type_t *ftype = new func_type_t(A, R, TOK);
	D = new func_decl_t(N, ftype, B);
}




//------------------------------------------------------------------------------
// Function results
//------------------------------------------------------------------------------

%type func_results { field_vector_t* }
func_results(FR) ::= type(T). {
	field_t *f = new field_t(0, T);
	FR = new field_vector_t(1, f);
}
func_results(FR) ::= LPAREN field_comma_list(FL) RPAREN. { FR = FL; }
func_results(FR) ::= . { FR = 0; }




//------------------------------------------------------------------------------
// Import specification
//
// ast "go/ast"
// "fmt"
//------------------------------------------------------------------------------

%type import_spec_list { import_spec_vector_t* }
import_spec_list(L) ::= import_spec(S). { L = new import_spec_vector_t(1, S); }
import_spec_list(L) ::= import_spec_list(L2) SEMICOLON import_spec(S). {
	L2->push_back(S); L = L2;
}

%type import_spec { import_spec_t* }
import_spec(IS) ::= IDENT(I) STRING(S). { IS = new import_spec_t(I, S); }
import_spec(IS) ::= STRING(S).          { IS = new import_spec_t(0, S); }




//------------------------------------------------------------------------------
// Type specification
//
// pfloat ->float
// iarray [10]int
//------------------------------------------------------------------------------

%type type_spec_list { type_spec_vector_t* }
type_spec_list(L) ::= type_spec(S). { L = new type_spec_vector_t(1, S); }
type_spec_list(L) ::= type_spec_list(L2) SEMICOLON type_spec(S). {
	L2->push_back(S); L = L2;
}

%type type_spec { type_spec_t* }
type_spec(TS) ::= IDENT(I) type(T). { TS = new type_spec_t(I, T); }




//------------------------------------------------------------------------------
// Value specification (works as const or var spec)
//
// n1, n2, n3 int = 1, 2, 3
// x float = 3.14
// s = "123"
// n
//------------------------------------------------------------------------------

%type value_spec_list { value_spec_vector_t* }
value_spec_list(L) ::= value_spec(S). { L = new value_spec_vector_t(1, S); }
value_spec_list(L) ::= value_spec_list(L2) SEMICOLON value_spec(S). {
	L2->push_back(S); L = L2;
}

%type value_spec { value_spec_t* }
value_spec(VS) ::= ident_list(IL) otype(OT). {
	VS = new value_spec_t(IL, OT);
}
value_spec(VS) ::= ident_list(IL) otype(OT) ASSIGN(TOK) cexpr_list(EL). {
	VS = new value_spec_t(IL, OT, EL, TOK);
}




//------------------------------------------------------------------------------
// Type
//
// lib.Type
// int, float, uint32
// struct { a, b, c int; d, e, f float; }
// union { a, b, c int; }
// [10]int, [5]float
// *float
// **byte
// func(int, *float) (bool, bool)
//------------------------------------------------------------------------------

%type type { node_t* }
type(T) ::= TIMES(TOK) type(X).               { T = new pointer_type_t(X, TOK); }
type(T) ::= LSB(L) expr(LEN) RSB(R) type(EL). { T = new array_type_t(LEN, EL, L, R); }
type(T) ::= LSB(L) RSB(R) type(EL).           { T = new array_type_t(0, EL, L, R); }
type(T) ::= ident(P) DOT ident(SEL).          { T = new selector_expr_t(P, SEL); }
type(T) ::= ident(I).                         { T = I; }
type(T) ::= STRUCT(TOK) LCURLY(L) ofield_semi_list_and_osemi(FL) RCURLY(R). {
	T = new struct_type_t(FL, TOK, L, R);	
}
type(T) ::= FUNC(TOK) LPAREN ofield_comma_list(A) RPAREN func_results(R). {
	T = new func_type_t(A, R, TOK);
}




//------------------------------------------------------------------------------
// Field lists
//
// n1, n2, n3 type; n4 type; n5, n6 type (semicolon separated)
// n1, n2, n3 type, n4 type, n5, n6 type (comma separated)
//------------------------------------------------------------------------------

%type field_comma_list { field_vector_t* }
field_comma_list(L) ::= nametype_list(NTL) ofunc_ellipsis(FE). {
	L = nametypev_to_fieldv(ctx->diag, NTL, FE);
}

%type field_semi_list { field_vector_t* }
field_semi_list(L) ::= field(F). { L = new field_vector_t(1, F); }
field_semi_list(L) ::= field_semi_list(L2) SEMICOLON field(F). {
	L2->push_back(F); L = L2;
}

// this is a temporary list for comma separated field list
%type nametype_list { nametype_vector_t* }
nametype_list(FL) ::= nametype(F). { FL = new nametype_vector_t(1, F); }
nametype_list(FL) ::= nametype_list(FL2) COMMA nametype(F). {
	FL2->push_back(F); FL = FL2;
}

%type nametype { nametype_t }
nametype(F) ::= ident(I) type(T). { F.name = I; F.type = T; }
nametype(F) ::= type(T).          { F.name = 0; F.type = T; }




//------------------------------------------------------------------------------
// Function optional ellipsis (optional: , ... at the end of the function)
//------------------------------------------------------------------------------

%type ofunc_ellipsis { bool }
ofunc_ellipsis(FE) ::= COMMA ELLIPSIS. { FE = true; }
ofunc_ellipsis(FE) ::= .               { FE = false; }




//------------------------------------------------------------------------------
// Field
//
// name1, name2 type
// name type
//------------------------------------------------------------------------------

%type field { field_t* }
field(F) ::= ident_list(IL) type(T). { F = new field_t(IL, T); }
field(F) ::= type(T).                { F = new field_t(0, T); }




//------------------------------------------------------------------------------
// Basic literal (expr)
// 
// 3.14, 5, "string", `raw string`, 'c', etc.
//------------------------------------------------------------------------------
%type basic_lit { basic_lit_expr_t* }
basic_lit(A) ::= INT(B).    { A = new basic_lit_expr_t(B); }
basic_lit(A) ::= FLOAT(B).  { A = new basic_lit_expr_t(B); }
basic_lit(A) ::= STRING(B). { A = new basic_lit_expr_t(B); }
basic_lit(A) ::= CHAR(B).   { A = new basic_lit_expr_t(B); }




//------------------------------------------------------------------------------
// Primary expression
//
// Usually a part of a unary or binary expression.
//
// math.PI, 5, (7+4), cos(3.14), array[5], etc.
//------------------------------------------------------------------------------
%type pexpr { node_t* }
pexpr(A) ::= basic_lit(E).                   { A = E; }
pexpr(A) ::= ident(I).                       { A = I; }
pexpr(A) ::= LPAREN(L) expr(E) RPAREN(R).    { A = new paren_expr_t(E, L, R); }
pexpr(A) ::= pexpr(P) LSB(L) expr(I) RSB(R). { A = new index_expr_t(P, I, L, R); }

// selector expr
pexpr(A) ::= pexpr(P) DOT IDENT(I). {
	A = new selector_expr_t(P, new ident_expr_t(I));
}

// type cast expr
pexpr(A) ::= pexpr(P) DOT(D) LPAREN(L) type(T) RPAREN(R). {
	A = new type_cast_expr_t(P, T, D, L, R);
}


// call expr
pexpr(A) ::= pexpr(P) LPAREN(L) RPAREN(R).   { A = new call_expr_t(P, 0, L, R); }

// call expr with non-empty expr list
pexpr(A) ::= pexpr(P) LPAREN(L) cexpr_list(EL) RPAREN(R). {
	A = new call_expr_t(P, EL, L, R);
}




//------------------------------------------------------------------------------
// Unary expression
//
// -5, +2, !true, &x, *ptr
//------------------------------------------------------------------------------

%type uexpr { node_t* }
uexpr(A) ::= pexpr(B).                       { A = B; }
uexpr(A) ::= PLUS(T) uexpr(E).               { A = new unary_expr_t(E, T); }
uexpr(A) ::= MINUS(T) uexpr(E).              { A = new unary_expr_t(E, T); }
uexpr(A) ::= NOT(T) uexpr(E).                { A = new unary_expr_t(E, T); }
uexpr(A) ::= AND(T) uexpr(E).                { A = new unary_expr_t(E, T); }
uexpr(A) ::= TIMES(T) uexpr(E).              { A = new unary_expr_t(E, T); }
uexpr(A) ::= XOR(T) uexpr(E).                { A = new unary_expr_t(E, T); }




//------------------------------------------------------------------------------
// Binary expression
//
// 1 + 2, 7 / 8, 4 * 8, 5 << 1, true || false, true && true, 5 <= 6, etc.
//------------------------------------------------------------------------------

%type expr { node_t* }
expr(E) ::= uexpr(U).                  { E = U; }
expr(E) ::= expr(L) DIVIDE(T) expr(R). { E = new binary_expr_t(L, R, T); }
expr(E) ::= expr(L) TIMES(T)  expr(R). { E = new binary_expr_t(L, R, T); }
expr(E) ::= expr(L) MOD(T)    expr(R). { E = new binary_expr_t(L, R, T); }
expr(E) ::= expr(L) SHIFTL(T) expr(R). { E = new binary_expr_t(L, R, T); }
expr(E) ::= expr(L) SHIFTR(T) expr(R). { E = new binary_expr_t(L, R, T); }
expr(E) ::= expr(L) AND(T)    expr(R). { E = new binary_expr_t(L, R, T); }
expr(E) ::= expr(L) ANDNOT(T) expr(R). { E = new binary_expr_t(L, R, T); }
expr(E) ::= expr(L) PLUS(T)   expr(R). { E = new binary_expr_t(L, R, T); }
expr(E) ::= expr(L) MINUS(T)  expr(R). { E = new binary_expr_t(L, R, T); }
expr(E) ::= expr(L) OR(T)     expr(R). { E = new binary_expr_t(L, R, T); }
expr(E) ::= expr(L) XOR(T)    expr(R). { E = new binary_expr_t(L, R, T); }
expr(E) ::= expr(L) EQ(T)     expr(R). { E = new binary_expr_t(L, R, T); }
expr(E) ::= expr(L) NEQ(T)    expr(R). { E = new binary_expr_t(L, R, T); }
expr(E) ::= expr(L) LT(T)     expr(R). { E = new binary_expr_t(L, R, T); }
expr(E) ::= expr(L) LE(T)     expr(R). { E = new binary_expr_t(L, R, T); }
expr(E) ::= expr(L) GT(T)     expr(R). { E = new binary_expr_t(L, R, T); }
expr(E) ::= expr(L) GE(T)     expr(R). { E = new binary_expr_t(L, R, T); }
expr(E) ::= expr(L) ANDAND(T) expr(R). { E = new binary_expr_t(L, R, T); }
expr(E) ::= expr(L) OROR(T)   expr(R). { E = new binary_expr_t(L, R, T); }




//------------------------------------------------------------------------------
// Compound literal
//
// {cexpr, cexpr, cexpr}
// {cexpr, cexpr, cexpr}.(type)
//------------------------------------------------------------------------------

%type compound_lit { node_t* }
compound_lit(CL) ::= LCURLY(L) cexpr_list(CEL) RCURLY(R). {
	CL = new compound_lit_t(CEL, 0, L, R);
}
compound_lit(CL) ::= LCURLY(L) cexpr_list(CEL) RCURLY DOT LPAREN type(T) RPAREN(R). {
	CL = new compound_lit_t(CEL, T, L, R);
}




//------------------------------------------------------------------------------
// Compound/expression list (comma separated)
//
// expr, compound, expr
//------------------------------------------------------------------------------

%type cexpr_list { node_vector_t* }
%type cexpr      { node_t* }
cexpr(CE) ::= expr(E).                           { CE = E; }
cexpr(CE) ::= compound_lit(CL).                  { CE = CL; }
cexpr_list(L) ::= cexpr(E).                      { L = new node_vector_t(1, E); }
cexpr_list(L) ::= cexpr_list(L2) COMMA cexpr(E). { L2->push_back(E); L = L2; }

%type expr_list { node_vector_t* }
expr_list(L) ::= expr(E).                     { L = new node_vector_t(1, E); }
expr_list(L) ::= expr_list(L2) COMMA expr(E). { L2->push_back(E); L = L2; }




//------------------------------------------------------------------------------
// Misc
//------------------------------------------------------------------------------

// optional semicolon
osemi ::= .
osemi ::= SEMICOLON.

// optional expression
%type oexpr { node_t* }
oexpr(OE) ::= .        { OE = 0; }
oexpr(OE) ::= expr(E). { OE = E; }

// optional statement list
%type ostmt_list { node_vector_t* }
ostmt_list(OSL) ::= .              { OSL = 0; }
ostmt_list(OSL) ::= stmt_list(SL). { OSL = SL; }

// optional type
%type otype { node_t* }
otype(OT) ::= .        { OT = 0; }
otype(OT) ::= type(T). { OT = T; }

// identifier converted to an AST node
%type ident { ident_expr_t* }
ident(A) ::= IDENT(B). { A = new ident_expr_t(B); }

// identifier list
%type ident_list { ident_expr_vector_t* }
ident_list(L) ::= ident(I).                      { L = new ident_expr_vector_t(1, I); }
ident_list(L) ::= ident_list(L2) COMMA ident(I). { L2->push_back(I); L = L2; }

// optional comma-separated field list
%type ofield_comma_list { field_vector_t* }
ofield_comma_list(L) ::= . { L = 0; }
ofield_comma_list(L) ::= field_comma_list(R). { L = R; }

// optional semi-separated field list
%type ofield_semi_list_and_osemi { field_vector_t* }
ofield_semi_list_and_osemi(L) ::= . { L = 0; }
ofield_semi_list_and_osemi(L) ::= field_semi_list(R) osemi. { L = R; }
