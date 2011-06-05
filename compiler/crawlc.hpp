#pragma once

#include <gmp.h>
#include <mpfr.h>
#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include <string>
#include <vector>
#include "config.hpp"
#include "tokens.hpp"

using std::tr1::unordered_map;
using std::tr1::unordered_set;
using std::tr1::hash;




//==============================================================================
// Utils
//==============================================================================

#ifdef CRAWL_NO_ASSERT
	#define CRAWL_ASSERT(cond, ...) ((void)0)
	#define CRAWL_QASSERT(cond) ((void)0)
#else
	#define CRAWL_ASSERT(cond, ...)						\
	do {									\
		if (!(cond)) {							\
			fprintf(stderr, "CRAWL_ASSERT(%s) (%s:%d): ", #cond,	\
				pretty_print_FILE(__FILE__), __LINE__);		\
			fprintf(stderr, __VA_ARGS__);				\
			fprintf(stderr, "\n");					\
			exit(EXIT_FAILURE);					\
		}								\
	} while (0)
	#define CRAWL_QASSERT(cond)						\
	do {									\
		if (!(cond)) {							\
			fprintf(stderr, "CRAWL_QASSERT(%s) (%s:%d)\n", #cond,	\
				pretty_print_FILE(__FILE__), __LINE__);		\
			exit(EXIT_FAILURE);					\
		}								\
	} while (0)
#endif

#define DIE(...)								\
do {										\
	fprintf(stderr, "FATAL: ");						\
	fprintf(stderr, __VA_ARGS__);						\
	fprintf(stderr, "\n");							\
	exit(EXIT_FAILURE);							\
} while (0)

#define PRETTY_PRINT_FILE_BASE "compiler"
const char *pretty_print_FILE(const char *file);

std::string extract_extension(const char *path);
std::string extract_stem(const char *path);
std::string extract_dir(const char *path);
std::string replace_extension(const char *path, const char *newext);

// 11 identifier symbols for 64 bit number
void base62_encode64(uint64_t n, char *out);
uint64_t base62_decode64(const char *in);




//==============================================================================
// PREDECLARATIONS
//==============================================================================

struct stype_t;
struct sdecl_t;
struct scope_block_t;
struct brawl_context_t;
namespace llvm { struct Type; }
namespace llvm { struct Value; }




//==============================================================================
// UTF-8
//==============================================================================

size_t utf8_char_length(char c);
size_t unicode_to_utf8(char *out, uint32_t c);
size_t utf8_to_unicode(uint32_t *out, const char *c);




//==============================================================================
// SOURCE LOC
//==============================================================================

// User-friendly position representation.
struct source_pos_t {
	const char *filename;
	unsigned int offset;
	unsigned int line;
	unsigned int line_offset;

	// we need original input to calculate column properly, because input
	// is utf-8
	unsigned int col(char *input);
};

// Context-dependent source_pos_t encoding.
typedef unsigned int source_loc_t;

// A line in the source_file_t, offset is relative to the beginning of the file.
struct source_line_t {
	unsigned int offset;
	unsigned int line;
};

// A source file in a source group, offset is relative to all files data.
// First line is added automatically (therefore it makes sure that there is at
// least one line in the file).
struct source_file_t {
	std::string filename;
	unsigned int offset;
	std::vector<source_line_t> lines;
	std::vector<char> data;

	source_file_t();

	source_line_t *get_line_by_loc(source_loc_t loc);
	source_pos_t get_pos_by_loc(source_loc_t loc);

	void add_line(unsigned int file_offset);
};

struct source_group_t {
	std::vector<source_file_t*> files;

	~source_group_t();

	source_file_t *get_file_by_name(const char *filename);
	source_file_t *get_file_by_loc(source_loc_t loc);
	source_pos_t get_pos_by_loc(source_loc_t loc);

	source_file_t *add_file(const char *filename, std::vector<char> *data);
};

struct source_loc_range_t {
	source_loc_t beg;
	source_loc_t end;
};




//==============================================================================
// MESSAGES
//==============================================================================

#define CONSOLE_WHITE   "\033[1;37m"
#define CONSOLE_MAGENTA "\033[1;35m"
#define CONSOLE_RED     "\033[1;31m"
#define CONSOLE_GREEN   "\033[1;32m"
#define CONSOLE_YELLOW  "\033[1;33m"
#define CONSOLE_NOCOL   "\033[m"

void cppsprintf(std::string *out, const char *fmt, ...);
void cppvsprintf(std::string *out, const char *fmt, va_list va);
void highlight_code(std::string *out, source_file_t *file, source_loc_t tok,
		    source_loc_range_t *ranges = 0, size_t ranges_n = 0,
		    bool hltok = true);

//------------------------------------------------------------------------------
// message_t
//------------------------------------------------------------------------------

enum message_type_t {
	MESSAGE_ERROR,
	MESSAGE_WARNING
};

struct message_t {
	message_type_t type;

	std::string text;
	source_loc_t location;
	std::vector<source_loc_range_t> ranges;
	bool hltok;

	void print_to_stderr(source_group_t *group);
};

message_t *new_message(message_type_t type,
		       source_loc_t tok, bool hltok,
		       source_loc_range_t *ranges, size_t ranges_n,
		       const char *fmt, ...);

struct diagnostic_t {
	std::vector<message_t*> messages;

	~diagnostic_t();

	void report(message_t *m);
	bool empty();
	void print_to_stderr(source_group_t *srcinfo);
};




//==============================================================================
// AST
//==============================================================================

//------------------------------------------------------------------------------
// value_t
//------------------------------------------------------------------------------

enum value_type_t {
	VALUE_NONE, // when there is no value
	VALUE_ERROR, // when there is an error (e.g. div by zero)
	VALUE_STRING,
	VALUE_INT,
	VALUE_FLOAT
};

struct value_t {
	value_type_t type;

	union {
		char *xstr;
		mpz_t xint;
		mpfr_t xfloat;
	};

	value_t();
	value_t(const value_t &r);
	value_t(const char *s, int tok);
	value_t(const char *s, value_type_t t);
	explicit value_t(value_type_t t);
	explicit value_t(unsigned int v);
	explicit value_t(const std::string &s);

	~value_t();

	value_t &operator=(const value_t &r);

	value_t to_float();
	value_t to_int();
	std::string to_string();
	std::string to_literal();
	bool to_bool();

	void free();

	static value_t none;
	static value_t error;
};

//------------------------------------------------------------------------------
// value_stype_t
//------------------------------------------------------------------------------

struct value_stype_t {
	value_t value;
	stype_t *stype;

	value_stype_t();
};

//------------------------------------------------------------------------------
// token_t
//------------------------------------------------------------------------------

struct token_t {
	int type;
	value_t val;
	source_loc_t pos;
	char *beg, *end;

	token_t(const char *s, int type, source_loc_t p, char *beg, char *end);
	token_t(const token_t &r);
	~token_t();

private:
	token_t &operator=(const token_t &r); // copy protection
};

const char *token_to_string(int tok);

//------------------------------------------------------------------------------
// node_t
//------------------------------------------------------------------------------

struct node_t {
	enum type_t {
		PROGRAM,

		COMPOUND_LIT, // not an expression, but it fits there
		BINARY_EXPR,
		UNARY_EXPR,
		BASIC_LIT_EXPR,
		IDENT_EXPR,
		SELECTOR_EXPR,
		INDEX_EXPR,
		CALL_EXPR,
		PAREN_EXPR,
		TYPE_CAST_EXPR,
		TYPE_EXPR,

		EXPR_STMT,
		ASSIGN_STMT,
		BLOCK_STMT,
		RETURN_STMT,
		IFELSE_STMT,
		INCDEC_STMT,
		DECL_STMT,
		FOR_STMT,
		SWITCH_STMT,
		SWITCH_STMT_CASE,
		FLOW_STMT, // break, continue

		IMPORT_DECL,
		TYPE_DECL,
		CONST_DECL,
		VAR_DECL,
		FUNC_DECL,

		ARRAY_TYPE,
		POINTER_TYPE,
		STRUCT_TYPE,
		FUNC_TYPE,
	};

	type_t type;
	value_stype_t vst;

	node_t(type_t t);
	virtual ~node_t();
	virtual std::string to_string(int indent) = 0;
	virtual source_loc_range_t source_loc_range() = 0;

	bool is_expr();
	bool is_terminator();
};

void print_ast(node_t *n);

typedef std::vector<node_t*> node_vector_t;

//------------------------------------------------------------------------------
// program_t
//------------------------------------------------------------------------------

struct program_t : node_t {
	node_vector_t decls;

	program_t(node_vector_t *decls);
	~program_t();
	std::string to_string(int indent);
	source_loc_range_t source_loc_range();
};


//==============================================================================
// AST.COMPOUND
//==============================================================================

//------------------------------------------------------------------------------
// compound_lit_t
//------------------------------------------------------------------------------

struct compound_lit_t : node_t {
	node_vector_t elts;
	node_t *etype;
	source_loc_t pos_lp; // {
	source_loc_t pos_rp; // }
	std::vector<llvm::Value*> vals;

	compound_lit_t(node_vector_t *elts, node_t *etype,
		       token_t *l, token_t *r);
	~compound_lit_t();
	std::string to_string(int indent);
	source_loc_range_t source_loc_range();
};

//==============================================================================
// AST.STATEMENTS
//==============================================================================

//------------------------------------------------------------------------------
// expr_stmt_t
//------------------------------------------------------------------------------

struct expr_stmt_t : node_t {
	node_t *expr;

	expr_stmt_t(node_t *e);
	~expr_stmt_t();
	std::string to_string(int indent);
	source_loc_range_t source_loc_range();
};

//------------------------------------------------------------------------------
// decl_stmt_t
//------------------------------------------------------------------------------

struct decl_stmt_t : node_t {
	node_t *decl;

	decl_stmt_t(node_t *d);
	~decl_stmt_t();
	std::string to_string(int indent);
	source_loc_range_t source_loc_range();
};

//------------------------------------------------------------------------------
// incdec_stmt_t
//------------------------------------------------------------------------------

struct incdec_stmt_t : node_t {
	int tok;
	node_t *expr;
	source_loc_t pos;

	incdec_stmt_t(node_t *e, token_t *t);
	~incdec_stmt_t();
	std::string to_string(int indent);
	source_loc_range_t source_loc_range();
};

//------------------------------------------------------------------------------
// assign_stmt_t
//------------------------------------------------------------------------------

struct assign_stmt_t : node_t {
	int tok;
	node_vector_t lhs;
	node_vector_t rhs;
	source_loc_t pos;

	assign_stmt_t(node_vector_t *lhs, node_vector_t *rhs, token_t *t);
	~assign_stmt_t();
	std::string to_string(int indent);
	source_loc_range_t source_loc_range();
};

//------------------------------------------------------------------------------
// block_stmt_t
//------------------------------------------------------------------------------

struct block_stmt_t : node_t {
	node_vector_t stmts;
	source_loc_t pos_lcb;
	source_loc_t pos_rcb;

	block_stmt_t(node_vector_t *s, token_t *l, token_t *r);
	~block_stmt_t();
	std::string to_string(int indent);
	source_loc_range_t source_loc_range();
};

//------------------------------------------------------------------------------
// return_stmt_t
//------------------------------------------------------------------------------

struct return_stmt_t : node_t {
	node_vector_t returns;
	source_loc_t pos; // 'return'

	return_stmt_t(node_vector_t *r, token_t *t);
	~return_stmt_t();
	std::string to_string(int indent);
	source_loc_range_t source_loc_range();
};

//------------------------------------------------------------------------------
// ifelse_stmt_t
//------------------------------------------------------------------------------

struct ifelse_stmt_t : node_t {
	node_t *cond;
	block_stmt_t *body;
	node_t *elsestmt;
	source_loc_t pos_if; // 'if'
	source_loc_t pos_else; // 'else'

	ifelse_stmt_t(node_t *c, block_stmt_t *b, token_t *i,
		      token_t *et = 0, node_t *e = 0);
	~ifelse_stmt_t();
	std::string to_string(int indent);
	source_loc_range_t source_loc_range();
};

//------------------------------------------------------------------------------
// for_stmt_t
//------------------------------------------------------------------------------

struct for_stmt_t : node_t {
	node_t *init;
	node_t *cond;
	node_t *post;
	block_stmt_t *body;
	source_loc_t pos; // 'for'

	// goto label for post statement in case if I want to issue "continue"
	// behaviour
	std::string postgoto;

	for_stmt_t(node_t *i, node_t *c, node_t *p, block_stmt_t *b, token_t *t);
	~for_stmt_t();
	std::string to_string(int indent);
	source_loc_range_t source_loc_range();
};

//------------------------------------------------------------------------------
// switch_stmt_case_t
//------------------------------------------------------------------------------

struct switch_stmt_case_t : node_t {
	// if exprs is empty, then it's a "default", otherwise it's a "case"
	node_vector_t exprs;
	node_vector_t stmts;
	source_loc_t pos_tok; // tok position
	source_loc_t pos_col; // ':' position

	switch_stmt_case_t(node_vector_t *exprs, node_vector_t *stmts,
			   token_t *t, token_t *c);
	~switch_stmt_case_t();
	std::string to_string(int indent);
	source_loc_range_t source_loc_range();
};

//------------------------------------------------------------------------------
// switch_stmt_t
//------------------------------------------------------------------------------

struct switch_stmt_t : node_t {
	node_t *expr;
	block_stmt_t *body;
	source_loc_t pos; // 'switch'

	// switch indicates that every case expr is a constant value, for
	// codegen it means we can use C's switch
	bool constant;

	switch_stmt_t(node_t *expr, block_stmt_t *body, token_t *t);
	~switch_stmt_t();
	std::string to_string(int indent);
	source_loc_range_t source_loc_range();

	size_t num_cases();
};

//------------------------------------------------------------------------------
// flow_stmt_t
//------------------------------------------------------------------------------

struct flow_stmt_t : node_t {
	int tok; // TOK_BREAK, TOK_CONTINUE or TOK_FALLTHROUGH
	source_loc_t pos; // keyword position

	flow_stmt_t(token_t *t);
	std::string to_string(int indent);
	source_loc_range_t source_loc_range();
};




//==============================================================================
// AST.EXPRESSIONS
//==============================================================================

//------------------------------------------------------------------------------
// binary_expr_t
//------------------------------------------------------------------------------

struct binary_expr_t : node_t {
	int tok;
	node_t *lhs;
	node_t *rhs;
	source_loc_t pos;

	binary_expr_t(node_t *l, node_t *r, token_t *t);
	~binary_expr_t();
	std::string to_string(int indent);
	source_loc_range_t source_loc_range();
};

//------------------------------------------------------------------------------
// unary_expr_t
//------------------------------------------------------------------------------

struct unary_expr_t : node_t {
	int tok;
	node_t *expr;
	source_loc_t pos;

	unary_expr_t(node_t *e, token_t *t);
	~unary_expr_t();
	std::string to_string(int indent);
	source_loc_range_t source_loc_range();
};

//------------------------------------------------------------------------------
// basic_lit_expr_t
//------------------------------------------------------------------------------

struct basic_lit_expr_t : node_t {
	int tok;
	value_t val;
	source_loc_t pos_beg;
	source_loc_t pos_end;

	basic_lit_expr_t(token_t *t);
	~basic_lit_expr_t();
	std::string to_string(int indent);
	source_loc_range_t source_loc_range();
};

//------------------------------------------------------------------------------
// ident_expr_t
//------------------------------------------------------------------------------

struct ident_expr_t : node_t {
	std::string val;
	source_loc_t pos;
	sdecl_t *sdecl;

	ident_expr_t(token_t *t);
	std::string to_string(int indent);
	source_loc_range_t source_loc_range();
};

typedef std::vector<ident_expr_t*> ident_expr_vector_t;

//------------------------------------------------------------------------------
// selector_expr_t
//------------------------------------------------------------------------------

struct selector_expr_t : node_t {
	node_t *expr;
	ident_expr_t *sel;
	size_t idx; // for sturcts, cached index of a field

	selector_expr_t(node_t *e, ident_expr_t *ident);
	~selector_expr_t();
	std::string to_string(int indent);
	source_loc_range_t source_loc_range();
};

//------------------------------------------------------------------------------
// index_expr_t
//------------------------------------------------------------------------------

struct index_expr_t : node_t {
	node_t *expr;
	node_t *index;
	source_loc_t pos_lsb;
	source_loc_t pos_rsb;

	index_expr_t(node_t *e, node_t *i, token_t *l, token_t *r);
	~index_expr_t();
	std::string to_string(int indent);
	source_loc_range_t source_loc_range();
};

//------------------------------------------------------------------------------
// call_expr_t
//------------------------------------------------------------------------------

struct call_expr_t : node_t {
	node_t *expr;
	node_vector_t args;
	source_loc_t pos_lp;
	source_loc_t pos_rp;

	bool typeerror; // hack
	size_t mexpected; // hack

	call_expr_t(node_t *e, node_vector_t *args, token_t *l, token_t *r);
	~call_expr_t();
	std::string to_string(int indent);
	source_loc_range_t source_loc_range();
};

//------------------------------------------------------------------------------
// paren_expr_t
//------------------------------------------------------------------------------

struct paren_expr_t : node_t {
	node_t *expr;
	source_loc_t pos_lp;
	source_loc_t pos_rp;

	paren_expr_t(node_t *e, token_t *l, token_t *r);
	~paren_expr_t();
	std::string to_string(int indent);
	source_loc_range_t source_loc_range();
};

//------------------------------------------------------------------------------
// type_cast_expr_t
//------------------------------------------------------------------------------

struct type_cast_expr_t : node_t {
	node_t *expr;
	node_t *ctype;
	source_loc_t pos_dot; // .
	source_loc_t pos_lp;  // (
	source_loc_t pos_rp;  // )

	type_cast_expr_t(node_t *e, node_t *t, token_t *dot,
			 token_t *l, token_t *r);
	~type_cast_expr_t();
	std::string to_string(int indent);
	source_loc_range_t source_loc_range();
};

//------------------------------------------------------------------------------
// type_expr_t
//------------------------------------------------------------------------------

struct type_expr_t : node_t {
	node_t *etype;
	source_loc_t pos; // 'type'

	type_expr_t(node_t *etype, token_t *tok);
	~type_expr_t();
	std::string to_string(int indent);
	source_loc_range_t source_loc_range();
};




//==============================================================================
// AST.TYPES
//==============================================================================

//------------------------------------------------------------------------------
// array_type_t
//------------------------------------------------------------------------------

struct array_type_t : node_t {
	node_t *size;
	node_t *etype;
	source_loc_t pos_lsb;
	source_loc_t pos_rsb;

	array_type_t(node_t *s, node_t *et, token_t *l, token_t *r);
	~array_type_t();
	std::string to_string(int indent);
	source_loc_range_t source_loc_range();
};

//------------------------------------------------------------------------------
// pointer_type_t
//------------------------------------------------------------------------------

struct pointer_type_t : node_t {
	node_t *ptype;
	source_loc_t pos;

	pointer_type_t(node_t *pt, token_t *t);
	~pointer_type_t();
	std::string to_string(int indent);
	source_loc_range_t source_loc_range();
};

//------------------------------------------------------------------------------
// field_t
//------------------------------------------------------------------------------

struct field_t {
	ident_expr_vector_t names;
	node_t *type;

	field_t(ident_expr_vector_t *n, node_t *t);
	~field_t();
	std::string to_string(int indent);
};

typedef std::vector<field_t*> field_vector_t;

struct nametype_t {
	ident_expr_t *name;
	node_t *type;

	enum {
		SINGLE, // type only, which can also be a name ;-)
		DOUBLE  // name and type
	};

	int classify();
};

typedef std::vector<nametype_t> nametype_vector_t;

//------------------------------------------------------------------------------
// struct_type_t
//------------------------------------------------------------------------------

struct struct_type_t : node_t {
	int tok;
	field_vector_t fields;
	source_loc_t pos; // struct
	source_loc_t pos_lcb;
	source_loc_t pos_rcb;

	struct_type_t(field_vector_t *f, token_t *t, token_t *l, token_t *r);
	~struct_type_t();
	std::string to_string(int indent);
	source_loc_range_t source_loc_range();
};

//------------------------------------------------------------------------------
// func_type_t
//------------------------------------------------------------------------------

struct func_type_t : node_t {
	field_vector_t args;
	field_vector_t results;
	source_loc_t pos; // func
	bool varargs;

	func_type_t(field_vector_t *a, field_vector_t *r, token_t *t);
	~func_type_t();
	std::string to_string(int indent);
	source_loc_range_t source_loc_range();
	bool named_results();
};




//==============================================================================
// AST.DECLARATIONS
//==============================================================================

//------------------------------------------------------------------------------
// import_decl_t
//------------------------------------------------------------------------------

struct import_spec_t {
	ident_expr_t *name;
	basic_lit_expr_t *path;

	import_spec_t(token_t *ident, token_t *path);
	~import_spec_t();
	std::string to_string(int indent);
};

typedef std::vector<import_spec_t*> import_spec_vector_t;

struct import_decl_t : node_t {
	import_spec_vector_t specs;
	source_loc_t pos; // import
	source_loc_t pos_lp;
	source_loc_t pos_rp;

	import_decl_t(import_spec_vector_t *s, token_t *t,
		      token_t *l, token_t *r);
	import_decl_t(import_spec_t *is, token_t *t);
	~import_decl_t();
	std::string to_string(int indent);
	source_loc_range_t source_loc_range();
};

//------------------------------------------------------------------------------
// type_decl_t
//------------------------------------------------------------------------------

struct type_spec_t {
	ident_expr_t *name;
	node_t *type;

	type_spec_t(token_t *ident, node_t *type);
	~type_spec_t();
	std::string to_string(int indent);
};

typedef std::vector<type_spec_t*> type_spec_vector_t;

struct type_decl_t : node_t {
	type_spec_vector_t specs;
	source_loc_t pos; // type
	source_loc_t pos_lp;
	source_loc_t pos_rp;

	type_decl_t(type_spec_vector_t *s, token_t *t,
		    token_t *l, token_t *r);
	type_decl_t(type_spec_t *ts, token_t *t);
	~type_decl_t();
	std::string to_string(int indent);
	source_loc_range_t source_loc_range();
};

//------------------------------------------------------------------------------
// value_spec_t
//------------------------------------------------------------------------------

struct value_spec_t {
	ident_expr_vector_t names;
	node_vector_t values;
	node_t *type;
	source_loc_t pos; // =

	value_spec_t(ident_expr_vector_t *n, node_t *t = 0, node_vector_t *v = 0,
		     token_t *assign = 0);
	~value_spec_t();
	std::string to_string(int indent);
	source_loc_t source_loc_end();
};

typedef std::vector<value_spec_t*> value_spec_vector_t;

//------------------------------------------------------------------------------
// const_decl_t
//------------------------------------------------------------------------------

struct const_decl_t : node_t {
	value_spec_vector_t specs;
	source_loc_t pos;
	source_loc_t pos_lp;
	source_loc_t pos_rp;

	const_decl_t(value_spec_vector_t *s, token_t *t,
		     token_t *l, token_t *r);
	const_decl_t(value_spec_t *vs, token_t *t);
	~const_decl_t();
	std::string to_string(int indent);
	source_loc_range_t source_loc_range();
};

//------------------------------------------------------------------------------
// var_decl_t
//------------------------------------------------------------------------------

struct var_decl_t : node_t {
	value_spec_vector_t specs;
	source_loc_t pos;
	source_loc_t pos_lp;
	source_loc_t pos_rp;

	var_decl_t(value_spec_vector_t *s, token_t *t,
		   token_t *l, token_t *r);
	var_decl_t(value_spec_t *vs, token_t *t);
	~var_decl_t();
	std::string to_string(int indent);
	source_loc_range_t source_loc_range();
};

//------------------------------------------------------------------------------
// func_decl_t
//------------------------------------------------------------------------------

struct func_decl_t : node_t {
	func_type_t *ftype;
	ident_expr_t *name;
	block_stmt_t *body;

	func_decl_t(ident_expr_t *n, func_type_t *t, block_stmt_t *b = 0);
	~func_decl_t();
	std::string to_string(int indent);
	source_loc_range_t source_loc_range();
};




//==============================================================================
// AST.UTILS
//==============================================================================

struct ast_visitor_t {
	virtual ~ast_visitor_t();
	virtual ast_visitor_t *visit(node_t *n) = 0;
	void traverse(node_t *ast);
};

call_expr_t *is_call_expr(node_t *n);
ident_expr_t *is_ident_expr(node_t *n);
int compound_assignment_to_binary_tok(int tok);




//==============================================================================
// SEMANTIC TYPES
//==============================================================================

enum {
	BUILTIN_VOID,

	BUILTIN_BOOL,
	BUILTIN_ABSTRACT_BOOL, // as a result of const expr or built-in true/false

	BUILTIN_INT8,
	BUILTIN_INT16,
	BUILTIN_INT32,
	BUILTIN_INT64,
	BUILTIN_UINT8,
	BUILTIN_UINT16,
	BUILTIN_UINT32,
	BUILTIN_UINT64,
	BUILTIN_ABSTRACT_INT, // for literals

	BUILTIN_FLOAT32,
	BUILTIN_FLOAT64,
	BUILTIN_ABSTRACT_FLOAT, // for literals
	BUILTIN_ABSTRACT_STRING, // for literals
	BUILTIN_ABSTRACT_COMPOUND, // for literals
	BUILTIN_ABSTRACT_MODULE, // for modules

	BUILTIN_FUNC,

	BUILTIN_VA_LIST,
	BUILTIN_VA_LIST_PTR,

	BUILTIN_N
};

//------------------------------------------------------------------------------
// stype_t
//------------------------------------------------------------------------------

enum stype_type_t {
	// simple
	STYPE_VOID     = 1 << 0,
	STYPE_BOOL     = 1 << 1,
	STYPE_INT      = 1 << 2,
	STYPE_FLOAT    = 1 << 3,
	STYPE_STRING   = 1 << 4,
	STYPE_COMPOUND = 1 << 5,
	STYPE_MODULE   = 1 << 6,

	// composite
	STYPE_POINTER  = 1 << 7,
	STYPE_STRUCT   = 1 << 8,
	STYPE_UNION    = 1 << 9,
	STYPE_ARRAY    = 1 << 10,
	STYPE_FUNC     = 1 << 11,

	// mods
	STYPE_NAMED    = 1 << 12,
	STYPE_ABSTRACT = 1 << 13,
	STYPE_BUILTIN  = 1 << 14,
};

struct stype_t {
	unsigned int type;
	const llvm::Type *llvmtype; // cached type

	explicit stype_t(unsigned int t);
	virtual ~stype_t();
	virtual stype_t *end_type();
	virtual int bits();
	virtual std::string to_string();
};


typedef std::vector<stype_t*> stype_tracker_t;
typedef std::vector<stype_t*> stype_vector_t;

//------------------------------------------------------------------------------
// compound_stype_t
// (purely virtual type like 'string')
//------------------------------------------------------------------------------

struct compound_stype_t : stype_t {
	compound_stype_t();
	std::string to_string();
};

//------------------------------------------------------------------------------
// module_stype_t
// (purely virtual type like 'string')
//------------------------------------------------------------------------------

struct module_stype_t : stype_t {
	module_stype_t();
	std::string to_string();
};

//------------------------------------------------------------------------------
// void_stype_t
//------------------------------------------------------------------------------

struct void_stype_t : stype_t {
	void_stype_t();
	int bits();
};

//------------------------------------------------------------------------------
// bool_stype_t
//------------------------------------------------------------------------------

struct bool_stype_t : stype_t {
	bool_stype_t(bool abstract = false);
	std::string to_string();
	int bits();
};

//------------------------------------------------------------------------------
// int_stype_t
//------------------------------------------------------------------------------

struct int_stype_t : stype_t {
	int size;
	bool is_signed;

	int_stype_t(int s, bool is_signed, bool abstract = false);
	std::string to_string();
	int bits();
};

//------------------------------------------------------------------------------
// float_stype_t
//------------------------------------------------------------------------------

struct float_stype_t : stype_t {
	int size;

	float_stype_t(int s, bool abstract = false);
	std::string to_string();
	int bits();
};

//------------------------------------------------------------------------------
// string_stype_t
//------------------------------------------------------------------------------

struct string_stype_t : stype_t {
	string_stype_t();
	std::string to_string();
};

//------------------------------------------------------------------------------
// named_stype_t
//------------------------------------------------------------------------------

struct named_stype_t : stype_t {
	std::string name;
	stype_t *real;

	// used for type loop checking
	sdecl_t *decl;

	// used for pointer restoring on deserialization
	bool restored;

	named_stype_t();
	named_stype_t(const char *name, stype_t *real);
	stype_t *end_type();
	std::string to_string();
	int bits();

	void assign_real(stype_t *real);
};

//------------------------------------------------------------------------------
// pointer_stype_t
//------------------------------------------------------------------------------

struct pointer_stype_t : stype_t {
	stype_t *points_to;

	pointer_stype_t();
	pointer_stype_t(stype_t *points_to);
	std::string to_string();
	int bits();
	bool points_to_void();
};

//------------------------------------------------------------------------------
// func_stype_t
//------------------------------------------------------------------------------

struct func_stype_t : stype_t {
	stype_vector_t args;
	stype_vector_t results;
	bool varargs;

	func_stype_t();
	std::string to_string();
	int bits();
};

//------------------------------------------------------------------------------
// struct_stype_t
//------------------------------------------------------------------------------

struct struct_field_t {
	stype_t *type;
	std::string name;
	size_t padding; // padding after that field

	bool operator==(const struct_field_t &r);
	bool operator!=(const struct_field_t &r);
};

struct struct_stype_t : stype_t {
	std::vector<struct_field_t> fields;
	bool u; // is this a union?
	size_t alignment; // alignment of a structure
	size_t size;
	stype_t *biggest;

	struct_stype_t();
	std::string to_string();
	int bits();

	bool alignment_fixed();
};

//------------------------------------------------------------------------------
// array_stype_t
//------------------------------------------------------------------------------

struct array_stype_t : stype_t {
	stype_t *elem;
	size_t size;

	array_stype_t();
	array_stype_t(stype_t *elem, size_t size);
	std::string to_string();
	int bits();
};

//------------------------------------------------------------------------------
// type functions
//------------------------------------------------------------------------------

stype_t *get_int_type(size_t bits);
size_t stype_hash(stype_t *t);
void check_type_for_size_loops(diagnostic_t *diag, named_stype_t *t);
void fix_structs_alignment(struct_stype_t *sst);
size_t alignment_of(stype_t *t);
size_t size_from_array_or_compound(node_t *v);

typedef std::vector<stype_t*> stype_tracker_t;
void free_tracked_stypes(stype_tracker_t *tt);
stype_t *new_named_stype(stype_tracker_t *tt, const char *name, stype_t *real);
stype_t *new_pointer_stype(stype_tracker_t *tt, stype_t *points_to);
stype_t *new_func_stype(stype_tracker_t *tt,
			stype_vector_t *args, stype_vector_t *results,
			bool varargs);
stype_t *new_array_stype(stype_tracker_t *tt, stype_t *elem, size_t size);
stype_t *new_struct_stype(stype_tracker_t *tt, std::vector<struct_field_t> *fields,
			  size_t alignment, size_t size, bool u);

extern stype_t *builtin_stypes[BUILTIN_N];
extern stype_t *builtin_named_stypes[BUILTIN_N];
void init_builtin_stypes();
void free_builtin_stypes();

#define IS_STYPE_VOID(t)		(t->type & STYPE_VOID)
#define IS_STYPE_BOOL(t)		(t->type & STYPE_BOOL)
#define IS_STYPE_INT(t)			(t->type & STYPE_INT)
#define IS_STYPE_FLOAT(t)		(t->type & STYPE_FLOAT)
#define IS_STYPE_STRING(t)		(t->type & STYPE_STRING)
#define IS_STYPE_COMPOUND(t)		(t->type & STYPE_COMPOUND)
#define IS_STYPE_POINTER(t)		(t->type & STYPE_POINTER)
#define IS_STYPE_STRUCT(t)		(t->type & STYPE_STRUCT)
#define IS_STYPE_UNION(t)		(t->type & STYPE_UNION)
#define IS_STYPE_ARRAY(t)		(t->type & STYPE_ARRAY)
#define IS_STYPE_FUNC(t)		(t->type & STYPE_FUNC)
#define IS_STYPE_NAMED(t)		(t->type & STYPE_NAMED)
#define IS_STYPE_ABSTRACT(t)		(t->type & STYPE_ABSTRACT)
#define IS_STYPE_BUILTIN(t)		(t->type & STYPE_BUILTIN)
#define IS_STYPE_MODULE(t)		(t->type & STYPE_MODULE)

#define IS_STYPE_NUMBER(t)		(IS_STYPE_INT(t) || IS_STYPE_FLOAT(t))
#define IS_STYPE_POINTER_OR_INT(t)	(IS_STYPE_POINTER(t) || IS_STYPE_INT(t))
#define IS_STYPE_POINTER_OR_ARRAY(t)	(IS_STYPE_POINTER(t) || IS_STYPE_ARRAY(t))
#define IS_STYPE_COMPOUND_OR_ARRAY(t)	(IS_STYPE_COMPOUND(t) || IS_STYPE_ARRAY(t))
#define IS_STYPE_ARRAY_OR_STRUCT(t)	(IS_STYPE_ARRAY(t) || IS_STYPE_STRUCT(t))
#define IS_STYPE_POINTER_OR_FUNC(t)     (IS_STYPE_POINTER(t) || IS_STYPE_FUNC(t))
#define IS_STYPE_STRUCT_OR_UNION(t)	(IS_STYPE_STRUCT(t) || IS_STYPE_UNION(t))

#define IS_STYPE_ABSTRACT_NUMBER(t)	(IS_STYPE_ABSTRACT(t) && IS_STYPE_NUMBER(t))
#define IS_STYPE_ABSTRACT_BOOL(t)	(IS_STYPE_ABSTRACT(t) && IS_STYPE_BOOL(t))
#define IS_STYPE_ABSTRACT_INT(t)	(IS_STYPE_ABSTRACT(t) && IS_STYPE_INT(t))
#define IS_STYPE_ABSTRACT_FLOAT(t)	(IS_STYPE_ABSTRACT(t) && IS_STYPE_FLOAT(t))

#define IS_STYPE_NAMED_BOOL(t)		(IS_STYPE_NAMED(t) && IS_STYPE_BOOL(t))
#define IS_STYPE_NAMED_INT(t)		(IS_STYPE_NAMED(t) && IS_STYPE_INT(t))
#define IS_STYPE_NAMED_FLOAT(t)		(IS_STYPE_NAMED(t) && IS_STYPE_FLOAT(t))

stype_t *non_abstract(stype_t *a, stype_t *b);
stype_t *biggest_stype(stype_t *a, stype_t *b);
stype_t *numeric_binop_compat(stype_t *a, stype_t *b);
stype_t *type_binop_compat(stype_t *a, stype_t *b, unsigned int type);
stype_t *types_binop_compat(stype_t *a, stype_t *b, unsigned int *type);
stype_t *vice_versa(stype_t *(*pred)(stype_t*, stype_t*), stype_t *a, stype_t *b);
stype_t *pointer_op_int(stype_t *a, stype_t *b);
stype_t *pointer_int_compat(stype_t *a, stype_t *b);
stype_t *pointer_pointer_compat(stype_t *a, stype_t *b);
stype_t *shift_op_compat(stype_t *a, stype_t *b);
stype_t *pointer_op_pointer_to_void(stype_t *a, stype_t *b);

// binary operation compatible, returns the resulting type of the expression
// 'optype' is the type for evaluating values in
// for example for '5.5 > 4' function will return 'bool' and 'optype' is 'float'
stype_t *binop_compat(stype_t *a, stype_t *b, int tok, stype_t **optype);
stype_t *unop_compat(stype_t *operand, int tok, node_t *n, stype_tracker_t *tt);

bool are_the_same(stype_t *t1, stype_t *t2);
bool can_typecast(stype_t *from, stype_t *to);
bool value_fits_in_type(value_t *v, stype_t *t);
bool assignable(stype_t *from, stype_t *to);

struct stype_visitor_t {
	// set this to true if you want to terminate immediately
	// (no more 'visit' calls)
	bool terminate;

	stype_visitor_t();
	virtual ~stype_visitor_t();
	virtual stype_visitor_t *visit(stype_t *t) = 0;

	// post_visit is called when all children of 't' were traversed
	virtual void post_visit(stype_t *t);
	void traverse(stype_t *t);
};




//==============================================================================
// SEMANTIC DECLARATIONS
//==============================================================================

//------------------------------------------------------------------------------
// sdecl_t
//------------------------------------------------------------------------------
enum sdecl_type_t {
	SDECL_IMPORT,
	SDECL_TYPE,
	SDECL_CONST,
	SDECL_VAR,
	SDECL_FUNC
};

struct sdecl_t {
	sdecl_type_t type;

	// Scope defines where it was declared, each declaration may contain
	// some kind of init statement, and it should be resolved in that
	// scope.
	//
	// NOTE: It's not a scope where this declaration is visible. In fact
	// that scope should not contain this declaration (with exception for
	// types, they can be recursive).
	scope_block_t *scope;
	std::string name;
	stype_t *stype;
	bool incycle;
	bool typeerror;
	bool inited;
	llvm::Value *addr;

	sdecl_t(sdecl_type_t t, const char *name);
	virtual ~sdecl_t();
	virtual ident_expr_t *get_ident() = 0;
};

//------------------------------------------------------------------------------
// const_sdecl_t
//------------------------------------------------------------------------------

struct const_sdecl_t : sdecl_t {
	ident_expr_t *ident;
	source_loc_t pos; // =
	node_t *init;
	node_t *ctype;
	int iota;
	value_t value;

	const_sdecl_t(const char *name);
	const_sdecl_t(ident_expr_t *name, source_loc_t pos, node_t *init,
		      node_t *ctype, int iota);
	ident_expr_t *get_ident();
};

//------------------------------------------------------------------------------
// type_sdecl_t
//------------------------------------------------------------------------------

struct type_sdecl_t : sdecl_t {
	type_spec_t *spec;

	type_sdecl_t(const char *name);
	type_sdecl_t(type_spec_t *spec, stype_t *t);
	ident_expr_t *get_ident();
};

//------------------------------------------------------------------------------
// var_sdecl_t
//------------------------------------------------------------------------------

struct var_sdecl_t : sdecl_t {
	ident_expr_t *ident;
	source_loc_t pos; // =
	node_t *init; // can be 0
	node_t *vtype; // can be 0
	// for multiple return values case, the index in the return values
	// tuple, -1 if it's not a multiple assignment case
	int index;

	// if this declaration represents named return value, this is the index
	// in the return values tuple, -1 otherwise
	int namedret;

	var_sdecl_t(const char *name);
	var_sdecl_t(ident_expr_t *name, source_loc_t pos, node_t *init,
		    node_t *vtype, int index);
	ident_expr_t *get_ident();
};

//------------------------------------------------------------------------------
// func_sdecl_t
//------------------------------------------------------------------------------

struct func_sdecl_t : sdecl_t {
	func_decl_t *decl;
	unordered_set<var_sdecl_t*> deps;

	func_sdecl_t(const char *name);
	func_sdecl_t(func_decl_t *decl);
	ident_expr_t *get_ident();

	int n_return_values();
	bool has_named_return_values();
	stype_vector_t *return_types();
};

//------------------------------------------------------------------------------
// import_sdecl_t
//------------------------------------------------------------------------------

struct import_sdecl_t : sdecl_t {
	import_spec_t *spec;
	std::string prefix;
	unordered_map<std::string, sdecl_t*> decls;
	bool used;

	import_sdecl_t(import_spec_t *spec);
	ident_expr_t *get_ident();

	void load(brawl_context_t *ctx);
};

//------------------------------------------------------------------------------
// decl functions
//------------------------------------------------------------------------------

typedef std::vector<sdecl_t*> sdecl_tracker_t;
void free_tracked_sdecls(sdecl_tracker_t *tracker);
const_sdecl_t *new_const_sdecl(sdecl_tracker_t *dt, ident_expr_t *name,
			       source_loc_t pos, node_t *init, node_t *ctype,
			       int iota);
const_sdecl_t *new_const_bool_sdecl(sdecl_tracker_t *dt, const char *name,
				    bool value);
const_sdecl_t *new_const_iota_sdecl(sdecl_tracker_t *dt);
const_sdecl_t *new_const_nil_sdecl(sdecl_tracker_t *dt, stype_tracker_t *tt);
type_sdecl_t *new_type_sdecl(sdecl_tracker_t *dt, type_spec_t *spec, stype_t *t);
var_sdecl_t *new_var_sdecl(sdecl_tracker_t *dt, ident_expr_t *name,
			   source_loc_t pos, node_t *init, node_t *vtype,
			   int index);
var_sdecl_t *new_typed_var_sdecl(sdecl_tracker_t *dt, ident_expr_t *name,
				 stype_t *type, node_t *vtype);
func_sdecl_t *new_func_sdecl(sdecl_tracker_t *dt, func_decl_t *decl);
func_sdecl_t *new_builtin_func_sdecl(sdecl_tracker_t *dt, const char *name);
import_sdecl_t *new_import_sdecl(sdecl_tracker_t *dt, import_spec_t *spec);
sdecl_t *new_sdecl(sdecl_tracker_t *dt, const char *name, sdecl_type_t type);

//------------------------------------------------------------------------------
// scope_block_t
//------------------------------------------------------------------------------

struct scope_block_t {
	scope_block_t *parent;
	unordered_map<std::string, sdecl_t*> sdecls;

	scope_block_t(scope_block_t *parent = 0);
	sdecl_t *lookup(const char *name);
	sdecl_t *has(const char *name);

	// true if added successfully, false if already exists
	bool add(sdecl_t *d);
};

typedef std::vector<scope_block_t*> scope_block_tracker_t;

void free_tracked_scope_blocks(scope_block_tracker_t *v);
void fill_global_scope(scope_block_t *scope, sdecl_tracker_t *dt,
		       stype_tracker_t *tt);
scope_block_t *new_scope_block(scope_block_tracker_t *t, scope_block_t *parent);




//==============================================================================
// LEXER AND PARSER
//==============================================================================

//------------------------------------------------------------------------------
// lexer_t
//------------------------------------------------------------------------------

struct lexer_t {
	source_file_t *file;
	char *beg, *end;
	std::string interp;

	std::vector<token_t*> next;

	// ragel state
	int cs, act;
	char *ts, *te, *p, *pe, *eof;

	// Input end ('e') can be null, in that case 'noend' mode is used.
	// It's useful to reparse a token from known position (error message
	// purposes for example).
	// Source file ('f') can be null as well. In that case no line
	// additions are emitted and offset starts from zero.
	void set_input(source_file_t *f);

	// Returns the next token or zero if there is an error or no more
	// tokens. Token is allocated with 'new', it's your responsibility to
	// free it with 'delete'.
	token_t *next_token();

private:
	void append_hex(const char *p, size_t len);
	void append_oct(const char *p);
	void append_escaped(const char c);

	void tok_int(char *beg, char *end);
	void tok_float(char *beg, char *end);
	void tok_string(int type);
	void tok_op(int optype);
	void tok_op_pos(int optype, char *b, char *e);

	source_loc_t loc();
};

//------------------------------------------------------------------------------
// parser_t
//------------------------------------------------------------------------------

struct parser_t {
	source_group_t *srcinfo;
	diagnostic_t *diag;

	lexer_t lex;
	void *lemon;
	node_t *ast;

	int last_token;
	source_loc_t last_pos;

	parser_t(source_group_t *srcinfo, diagnostic_t *diag);
	~parser_t();

	void set_input(const char *filename, std::vector<char> *buf);
	node_t *parse();
};

field_vector_t *nametypev_to_fieldv(diagnostic_t *diag, nametype_vector_t *nv,
				    bool ellipsis);

// additional syntax checks
void syntax_check_const_decl(diagnostic_t *diag, const_decl_t *d);
void syntax_check_var_decl(diagnostic_t *diag, var_decl_t *d);

// TODO: statement after else clause must be an IFELSE_STMT or BLOCK_STMT
//void syntax_check_ifelse_stmt(diagnostic_t *diag, ifelse_stmt_t *s);




//==============================================================================
// TYPE CHECKS
//==============================================================================

bool addressable(node_t *n);

// Converts "node" to a series of declarations (1 to N) and calls back to
// "declare".
//
// Supported node types:
// IMPORT_DECL, TYPE_DECL, CONST_DECL, VAR_DECL, FUNC_DECL, ASSIGN_STMT
//
// Note: it doesn't set scope for a declaration, you have to do it manually
void declare_decl(node_t *node, sdecl_tracker_t *dt,
		  void (*declare)(std::vector<sdecl_t*>*, void*), void *data);
void redeclared_error(ident_expr_t *ident, diagnostic_t *diag);


//------------------------------------------------------------------------------
// PASS 1
// Collect all global declarations to the package scope.
//
// Details:
//
// Fills "pkgscope" with global declarations and "names" with their names.
// Uses "dtracker" for allocating sdecls.
// Uses "stracker" for allocating file scopes.
// Reports errors to "diag". These are only redeclaration errors.
//
// Also this pass actually loads imported modules, that's why it wants type
// tracker.
//------------------------------------------------------------------------------

struct pass1_t {
	// customizable by user
	scope_block_tracker_t *stracker;
	sdecl_tracker_t *dtracker;
	scope_block_t *pkgscope;
	std::vector<const char*> *names;
	diagnostic_t *diag;
	brawl_context_t *brawl;


	// run once for each file, AST should contain PROGRAM
	void pass(node_t *ast);
};

//------------------------------------------------------------------------------
// PASS 2
// Does the typecheck for all, evaluates constant expressions.
//------------------------------------------------------------------------------

struct pass2_t {
	// customizable by user
	scope_block_tracker_t *stracker;
	stype_tracker_t *ttracker;
	sdecl_tracker_t *dtracker;
	scope_block_t *pkgscope;
	diagnostic_t *diag;

	// When evaluating const declaration, this var points to it.
	const_sdecl_t *cur_const_decl;

	// When in function body, it points to this function. Useful for
	// checking correctness of return statements, etc.
	func_sdecl_t *cur_func_decl;

	// I use it when traversing declarations. In case if I have a loop
	// closed on a function, I look down to the stack. Loop may contain
	// only functions in that case, otherwise it's a typechecking loop.
	std::vector<sdecl_t*> recursion_stack;

	// When typegening array types, there is a certain contextual
	// value possible. For example:
	//	var a []int = {1, 2, 3, 4, 5};
	// In this case, []int must be converted to [5]int. This is a simple
	// hack for typegen, that allows one to specify that context. If it's
	// not zero, we must take it into account in typegen.
	size_t array_ctx;

	// Current scope.
	scope_block_t *cur_scope;

	// Current switch statement. We need to check case expressions against
	// switch expression.
	switch_stmt_t *cur_switch_stmt;

	// Used external declarations
	std::vector<import_sdecl_t*> used_extern_sdecls;

	void error_args_mismatch(call_expr_t *expr, size_t num);

	size_t typegen_array_size(node_t *size);
	bool typegen_for_funcfields(stype_vector_t *sv, field_vector_t *fv);
	bool typegen_for_structfields(std::vector<struct_field_t> *out,
				      field_vector_t *fields);
	stype_t *typegen(node_t *expr);

	bool stmt_returns(node_t *stmt);

	// use builtin_stypes[BUILTIN_VOID] if you need to allow any type as an
	// argument, it is useful for things like 'sizeof', where argument is a
	// type expression
	bool typecheck_call_expr_args(call_expr_t *expr,
				      stype_t **args, size_t args_n,
				      bool varargs);

	stype_t *typecheck_var_init(node_t *init, int index);
	value_stype_t typecheck_basic_lit_expr(basic_lit_expr_t *expr);
	value_stype_t typecheck_binary_expr(binary_expr_t *expr);
	value_stype_t typecheck_unary_expr(unary_expr_t *expr);
	value_stype_t typecheck_ident_expr(ident_expr_t *expr);
	value_stype_t typecheck_type_cast_expr(type_cast_expr_t *expr);
	value_stype_t typecheck_type_expr(type_expr_t *expr);
	value_stype_t typecheck_builtin_call_expr(call_expr_t *expr);
	value_stype_t typecheck_call_expr(call_expr_t *expr, bool mok = false);
	value_stype_t typecheck_index_expr(index_expr_t *expr);
	value_stype_t typecheck_compound_lit(compound_lit_t *expr);
	value_stype_t typecheck_selector_expr(selector_expr_t *expr);
	value_stype_t typecheck_expr(node_t *expr);

	void realize_binary_expr_type(binary_expr_t *expr, stype_t *ctx);
	void realize_unary_expr_type(unary_expr_t *expr, stype_t *ctx);
	void realize_type_cast_expr_type(type_cast_expr_t *expr, stype_t *ctx);
	void realize_call_expr_type(call_expr_t *expr, stype_t *ctx);
	void realize_index_expr_type(index_expr_t *expr, stype_t *ctx);
	void realize_compound_lit_type(compound_lit_t *expr, stype_t *ctx);
	void realize_selector_expr_type(selector_expr_t *expr, stype_t *ctx);
	void realize_expr_type(node_t *expr, stype_t *ctx);
	stype_t *default_abstract_context(stype_t *abstract);
	stype_t *non_abstract_or_null(binary_expr_t *expr);

	void declare_many(std::vector<sdecl_t*> *decls);
	void declare_function_vars(func_type_t *ft, func_stype_t *fst);
	void typecheck_block_stmt(block_stmt_t *stmt);
	void typecheck_decl_stmt(decl_stmt_t *stmt);
	void typecheck_return_stmt(return_stmt_t *stmt);
	void typecheck_compound_assign_stmt(assign_stmt_t *stmt);
	void typecheck_assign_stmt(assign_stmt_t *stmt);
	void typecheck_incdec_stmt(incdec_stmt_t *stmt);
	void typecheck_ifelse_stmt(ifelse_stmt_t *stmt);
	void typecheck_for_stmt(for_stmt_t *stmt);
	void typecheck_switch_stmt_case(switch_stmt_case_t *stmt);
	void typecheck_switch_stmt(switch_stmt_t *stmt);
	void typecheck_stmt(node_t *stmt);

	void resolve_sdecl(sdecl_t *d);
	bool is_function_loop(sdecl_t *f);
	size_t find_loop_start(sdecl_t *d);
	std::string report_recursion_stack(sdecl_t *d);

	value_t eval_binop(value_stype_t *vs1, value_stype_t *vs2,
			   stype_t *result, stype_t *optype, binary_expr_t *expr);
	value_t eval_unop(value_stype_t *v, stype_t *result, unary_expr_t *expr);

	void pass(std::vector<const char*> *pkgdecls);

	// for each function value_t must be INT, FLOAT and STRING respectively
	// for const bool I use VALUE_INT (1 == true, 0 == false)
	value_t eval_int_binop(value_t *v1, value_t *v2, binary_expr_t *expr);
	value_t eval_float_binop(value_t *v1, value_t *v2, binary_expr_t *expr);
	value_t eval_string_binop(value_t *v1, value_t *v2, binary_expr_t *expr);
	value_t eval_bool_binop(value_t *v1, value_t *v2, binary_expr_t *expr);

	value_t eval_int_unop(value_t *v, int_stype_t *t, unary_expr_t *expr);
	value_t eval_float_unop(value_t *v, unary_expr_t *expr);
	value_t eval_bool_unop(value_t *v, unary_expr_t *expr);
	value_t eval_type_cast(value_t *v, stype_t *from, stype_t *to,
			       type_cast_expr_t *expr);
	value_t eval_assignment(value_t *v, stype_t *from, stype_t *to,
				ident_expr_t *name, source_loc_t pos,
				node_t *init);
};

//------------------------------------------------------------------------------
// PASS 3
// Generate code
//
// Requires an AST that has passed pass 2.
//------------------------------------------------------------------------------

struct pass3_t {
	std::string uid;
	scope_block_t *pkgscope;
	std::vector<import_sdecl_t*> *used_extern_sdecls;
	const char *out_name;
	std::vector<const char*> *libs;
	bool dump;

	// Interface
	void pass(std::vector<const char*> *pkgdecls);
};

//------------------------------------------------------------------------------
// Brawl (binary crawl format)
//------------------------------------------------------------------------------

struct FILE_reader_t {
	FILE *file;
	std::string strbuf;

	FILE_reader_t(FILE *f);

	uint64_t     read_varuint();
	uint8_t      read_uint8();
	uint16_t     read_uint16();
	uint32_t     read_uint32();
	uint64_t     read_uint64();
	int8_t       read_int8();
	int16_t      read_int16();
	int32_t      read_int32();
	int64_t      read_int64();
	std::string &read_string();
};

struct FILE_writer_t {
	FILE *file;

	FILE_writer_t(FILE *f);

	void write_varuint(uint64_t n);
	void write_uint8(uint8_t n);
	void write_uint16(uint16_t n);
	void write_uint32(uint32_t n);
	void write_uint64(uint64_t n);
	void write_int8(int8_t n);
	void write_int16(int16_t n);
	void write_int32(int32_t n);
	void write_int64(int64_t n);
	void write_string(const char *str);
	void write_string(const char *str, size_t len);
	void write_string(const std::string &cppstr);
};

bool read_stdin(std::vector<char> *out);
bool read_file(std::vector<char> *out, const char *filename);
bool read_FILE(std::vector<char> *out, FILE *f);

struct stype_hasher_t {
	size_t operator()(const stype_t *t) const
	{
		return stype_hash((stype_t*)t);
	}
};

struct stype_comparer_t {
	size_t operator()(const stype_t *a, const stype_t *b) const
	{
		return are_the_same((stype_t*)a, (stype_t*)b);
	}
};

typedef unordered_map<stype_t*, int32_t, stype_hasher_t, stype_comparer_t> stype_map_t;

struct brawl_context_t {
	unordered_map<std::string, named_stype_t*> named_map;
	stype_tracker_t *ttracker;
	sdecl_tracker_t *dtracker;
};

//------------------------------------------------------------------------------
// Brawl semantic types
//------------------------------------------------------------------------------

struct brawl_stypes_t {
	brawl_context_t *ctx;
	std::string prefix;

	std::vector<stype_t*> stypes;
	stype_map_t map;
	int32_t index;

	brawl_stypes_t(brawl_context_t *ctx);
	void clear();

	int32_t stype_index(stype_t *t);
	int32_t builtin_stype_index(stype_t *t);
	stype_t *index_stype(int32_t idx);

	void serialize_named(FILE_writer_t *cout, named_stype_t *t);
	void serialize_pointer(FILE_writer_t *cout, pointer_stype_t *t);
	void serialize_array(FILE_writer_t *cout, array_stype_t *t);
	void serialize_struct(FILE_writer_t *cout, struct_stype_t *t);
	void serialize_func(FILE_writer_t *cout, func_stype_t *t);

	void deserialize_named(FILE_reader_t *cin);
	void deserialize_pointer(FILE_reader_t *cin);
	void deserialize_array(FILE_reader_t *cin);
	void deserialize_struct(FILE_reader_t *cin);
	void deserialize_func(FILE_reader_t *cin);
	void deserialize_types(FILE_reader_t *cin, size_t n);
	void restore_pointers();

	int32_t queue_for_serialization(stype_t *t);
	void save(FILE_writer_t *cout);
	void load(FILE_reader_t *cin);
};

//------------------------------------------------------------------------------
// Brawl semantic decls
//------------------------------------------------------------------------------

struct brawl_sdecls_t {
	brawl_context_t *ctx;
	brawl_stypes_t btypes;

	std::vector<sdecl_t*> sdecls;

	brawl_sdecls_t(brawl_context_t *ctx);
	void clear();

	void deserialize_decls(FILE_reader_t *cin, size_t n);
	void restore_pointers();

	void queue_for_serialization(sdecl_t *d);
	void save(FILE_writer_t *cout);
	void load(FILE_reader_t *cin);
};

//------------------------------------------------------------------------------
// Brawl module
//------------------------------------------------------------------------------

struct brawl_module_t {
	brawl_sdecls_t bdecls;

	std::string prefix;
	std::string package;
	unordered_map<std::string, sdecl_t*> decls;

	brawl_module_t(brawl_context_t *ctx);
	void clear();

	void queue_for_serialization(scope_block_t *pkgscope,
				     std::vector<const char*> *declnames,
				     const char *prefix, const char *package);
	void save(FILE_writer_t *cout);
	void load(FILE_reader_t *cin);
};

//------------------------------------------------------------------------------
// Misc
//------------------------------------------------------------------------------

std::string update_c_module_hash(const char *header);
