/* Driver template for the LEMON parser generator.
** The author disclaims copyright to this source code.
*/
/* First off, code is included that follows the "include" declaration
** in the input grammar file. */
#include <stdio.h>
#line 10 "parser.y"

	#include "crawlc.hpp"
	#include <stdlib.h>
	#include <stdio.h>
	#include <assert.h>
#line 14 "parser.c"
/* Next is all token values, in a form suitable for use by makeheaders.
** This section will be null unless lemon is run with the -m switch.
*/
/* 
** These constants (all generated automatically by the parser generator)
** specify the various kinds of tokens (terminals) that the parser
** understands. 
**
** Each symbol here is a terminal symbol in the grammar.
*/
/* Make sure the INTERFACE macro is defined.
*/
#ifndef INTERFACE
# define INTERFACE 1
#endif
/* The next thing included is series of defines which control
** various aspects of the generated parser.
**    YYCODETYPE         is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 terminals
**                       and nonterminals.  "int" is used otherwise.
**    YYNOCODE           is a number of type YYCODETYPE which corresponds
**                       to no legal terminal or nonterminal number.  This
**                       number is used to fill in empty slots of the hash 
**                       table.
**    YYFALLBACK         If defined, this indicates that one or more tokens
**                       have fall-back values which should be used if the
**                       original value of the token will not parse.
**    YYACTIONTYPE       is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 rules and
**                       states combined.  "int" is used otherwise.
**    ParseTOKENTYPE     is the data type used for minor tokens given 
**                       directly to the parser from the tokenizer.
**    YYMINORTYPE        is the data type used for all minor tokens.
**                       This is typically a union of many types, one of
**                       which is ParseTOKENTYPE.  The entry in the union
**                       for base tokens is called "yy0".
**    YYSTACKDEPTH       is the maximum depth of the parser's stack.  If
**                       zero the stack is dynamically sized using realloc()
**    ParseARG_SDECL     A static variable declaration for the %extra_argument
**    ParseARG_PDECL     A parameter declaration for the %extra_argument
**    ParseARG_STORE     Code to store %extra_argument into yypParser
**    ParseARG_FETCH     Code to extract %extra_argument from yypParser
**    YYNSTATE           the combined number of states.
**    YYNRULE            the number of rules in the grammar
**    YYERRORSYMBOL      is the code number of the error symbol.  If not
**                       defined, then do no error processing.
*/
#define YYCODETYPE unsigned char
#define YYNOCODE 111
#define YYACTIONTYPE unsigned short int
#define ParseTOKENTYPE  token_t* 
typedef union {
  int yyinit;
  ParseTOKENTYPE yy0;
  import_spec_vector_t* yy41;
  nametype_t yy43;
  basic_lit_expr_t* yy46;
  type_spec_vector_t* yy60;
  node_vector_t* yy70;
  program_t* yy74;
  value_spec_t* yy77;
  type_spec_t* yy78;
  bool yy97;
  node_t* yy100;
  nametype_vector_t* yy119;
  ident_expr_vector_t* yy134;
  ident_expr_t* yy160;
  value_spec_vector_t* yy175;
  switch_stmt_case_t* yy194;
  field_t* yy195;
  field_vector_t* yy196;
  import_spec_t* yy203;
  block_stmt_t* yy220;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define ParseARG_SDECL  parser_t *ctx ;
#define ParseARG_PDECL , parser_t *ctx 
#define ParseARG_FETCH  parser_t *ctx  = yypParser->ctx 
#define ParseARG_STORE yypParser->ctx  = ctx 
#define YYNSTATE 276
#define YYNRULE 148
#define YY_NO_ACTION      (YYNSTATE+YYNRULE+2)
#define YY_ACCEPT_ACTION  (YYNSTATE+YYNRULE+1)
#define YY_ERROR_ACTION   (YYNSTATE+YYNRULE)

/* The yyzerominor constant is used to initialize instances of
** YYMINORTYPE objects to zero. */
static const YYMINORTYPE yyzerominor = { 0 };

/* Define the yytestcase() macro to be a no-op if is not already defined
** otherwise.
**
** Applications can choose to define yytestcase() in the %include section
** to a macro that can assist in verifying code coverage.  For production
** code the yytestcase() macro should be turned off.  But it is useful
** for testing.
*/
#ifndef yytestcase
# define yytestcase(X)
#endif


/* Next are the tables used to determine what action to take based on the
** current state and lookahead token.  These tables are used to implement
** functions that take a state number and lookahead value and return an
** action integer.  
**
** Suppose the action integer is N.  Then the action is determined as
** follows
**
**   0 <= N < YYNSTATE                  Shift N.  That is, push the lookahead
**                                      token onto the stack and goto state N.
**
**   YYNSTATE <= N < YYNSTATE+YYNRULE   Reduce by rule N-YYNSTATE.
**
**   N == YYNSTATE+YYNRULE              A syntax error has occurred.
**
**   N == YYNSTATE+YYNRULE+1            The parser accepts its input.
**
**   N == YYNSTATE+YYNRULE+2            No such action.  Denotes unused
**                                      slots in the yy_action[] table.
**
** The action table is constructed as a single large table named yy_action[].
** Given state S and lookahead X, the action is computed as
**
**      yy_action[ yy_shift_ofst[S] + X ]
**
** If the index value yy_shift_ofst[S]+X is out of range or if the value
** yy_lookahead[yy_shift_ofst[S]+X] is not equal to X or if yy_shift_ofst[S]
** is equal to YY_SHIFT_USE_DFLT, it means that the action is not in the table
** and that yy_default[S] should be used instead.  
**
** The formula above is for computing the action when the lookahead is
** a terminal symbol.  If the lookahead is a non-terminal (as occurs after
** a reduce action) then the yy_reduce_ofst[] array is used in place of
** the yy_shift_ofst[] array and YY_REDUCE_USE_DFLT is used in place of
** YY_SHIFT_USE_DFLT.
**
** The following are the tables generated in this section:
**
**  yy_action[]        A single table containing all actions.
**  yy_lookahead[]     A table containing the lookahead for each entry in
**                     yy_action.  Used to detect hash collisions.
**  yy_shift_ofst[]    For each state, the offset into yy_action for
**                     shifting terminals.
**  yy_reduce_ofst[]   For each state, the offset into yy_action for
**                     shifting non-terminals after a reduce.
**  yy_default[]       Default action for each state.
*/
static const YYACTIONTYPE yy_action[] = {
 /*     0 */    57,   58,  190,   62,  139,   61,  200,  193,  106,   60,
 /*    10 */    10,   11,   12,   13,   14,   15,   16,   17,   18,   19,
 /*    20 */    20,   21,   22,  139,  213,   93,  193,  257,  258,  259,
 /*    30 */     7,   54,   76,   24,   55,    1,  413,  212,  127,  222,
 /*    40 */    89,   33,  263,   92,   70,   71,  103,  190,  210,  213,
 /*    50 */   267,   27,   53,  174,   83,  208,  209,  211,   59,    6,
 /*    60 */   136,  181,  212,  127,  216,   57,   58,   35,   62,  128,
 /*    70 */    61,  104,  105,  115,   60,   41,   42,   43,   44,   32,
 /*    80 */    34,   36,   37,   38,   39,   40,  425,   73,  213,  171,
 /*    90 */   133,  279,  257,  258,  259,    7,   54,  176,   24,   55,
 /*   100 */     1,  212,  127,  222,  139,   89,   33,  192,   92,   70,
 /*   110 */    71,  103,  190,  210,  213,  346,  112,  132,  136,  181,
 /*   120 */   208,  209,  211,   59,  275,  346,  266,  212,  127,  217,
 /*   130 */    57,   58,  213,   62,  134,   61,  182,   94,   89,   60,
 /*   140 */    97,   92,   70,   71,  103,  212,  127,  222,  251,  170,
 /*   150 */   105,  115,  139,  255,  256,  204,   64,  257,  258,  259,
 /*   160 */     7,   54,  190,   24,   55,    1,  213,   90,  271,  213,
 /*   170 */    89,   33,  179,   92,   70,   71,  103,  190,  210,  212,
 /*   180 */   127,  222,  212,  127,  218,  208,  209,  211,   59,   52,
 /*   190 */    51,   45,   46,   47,   48,   49,   50,   41,   42,   43,
 /*   200 */    44,   32,   34,   36,   37,   38,   39,   40,   32,   34,
 /*   210 */    36,   37,   38,   39,   40,   57,   58,   77,   62,  268,
 /*   220 */    61,  252,  253,  280,   60,  139,  137,   65,  245,  241,
 /*   230 */    95,  151,    1,  190,   52,   51,   45,   46,   47,   48,
 /*   240 */    49,   50,   41,   42,   43,   44,   32,   34,   36,   37,
 /*   250 */    38,   39,   40,   56,  274,   91,   33,   74,  142,  190,
 /*   260 */   197,   31,  190,  210,  140,   82,  252,  253,  280,  196,
 /*   270 */   208,  209,  211,   59,   96,  139,   77,  280,  186,   52,
 /*   280 */    51,   45,   46,   47,   48,   49,   50,   41,   42,   43,
 /*   290 */    44,   32,   34,   36,   37,   38,   39,   40,   52,   51,
 /*   300 */    45,   46,   47,   48,   49,   50,   41,   42,   43,   44,
 /*   310 */    32,   34,   36,   37,   38,   39,   40,  142,  190,   27,
 /*   320 */    31,  174,    1,  140,  135,  198,  126,  126,  138,  206,
 /*   330 */   206,   75,   75,  141,   85,  189,   99,  202,  203,  139,
 /*   340 */    76,   84,  187,   52,   51,   45,   46,   47,   48,   49,
 /*   350 */    50,   41,   42,   43,   44,   32,   34,   36,   37,   38,
 /*   360 */    39,   40,   52,   51,   45,   46,   47,   48,   49,   50,
 /*   370 */    41,   42,   43,   44,   32,   34,   36,   37,   38,   39,
 /*   380 */    40,   52,   51,   45,   46,   47,   48,   49,   50,   41,
 /*   390 */    42,   43,   44,   32,   34,   36,   37,   38,   39,   40,
 /*   400 */    67,   78,  247,  101,  246,   79,  247,  102,  246,   68,
 /*   410 */   260,  228,  214,   68,   52,   51,   45,   46,   47,   48,
 /*   420 */    49,   50,   41,   42,   43,   44,   32,   34,   36,   37,
 /*   430 */    38,   39,   40,  149,  153,  246,  246,   23,  185,  215,
 /*   440 */    68,   68,   51,   45,   46,   47,   48,   49,   50,   41,
 /*   450 */    42,   43,   44,   32,   34,   36,   37,   38,   39,   40,
 /*   460 */    45,   46,   47,   48,   49,   50,   41,   42,   43,   44,
 /*   470 */    32,   34,   36,   37,   38,   39,   40,   57,   58,  195,
 /*   480 */    62,  244,   61,  246,  139,   63,   60,  188,   68,  139,
 /*   490 */    74,  143,  145,  197,  201,  273,   57,   58,   98,   62,
 /*   500 */   139,   61,  199,  148,    1,   60,  139,  139,    2,  233,
 /*   510 */   240,  205,    8,  207,   72,  150,  231,  154,   33,  230,
 /*   520 */    81,  146,  261,   53,  190,  210,  109,   23,  129,  155,
 /*   530 */     1,    8,  208,  209,  211,   59,  276,   33,   23,   81,
 /*   540 */   177,  264,  213,  190,  210,  178,  180,  183,  184,  191,
 /*   550 */    97,  208,  209,  211,   59,  212,  127,  222,  234,  235,
 /*   560 */   236,   57,   58,   29,   62,   28,   61,   66,  194,  229,
 /*   570 */    60,  147,   80,  242,   74,  156,  232,  197,  201,  243,
 /*   580 */    57,   58,   98,   62,    9,   61,  199,   30,   23,   60,
 /*   590 */   248,    4,  250,  170,  105,  115,    8,  255,  256,  249,
 /*   600 */    69,  254,   33,  175,   81,   26,    5,   25,  190,  210,
 /*   610 */   213,  426,  426,  272,  426,    1,  208,  209,  211,   59,
 /*   620 */     3,   33,  426,  212,  127,  222,  426,  190,  210,  426,
 /*   630 */   426,  426,  426,  426,  213,  208,  209,  211,   59,    4,
 /*   640 */   250,  170,  105,  115,  426,  255,  256,  212,  127,  219,
 /*   650 */   426,  269,   57,   58,  426,   62,  426,   61,  213,  426,
 /*   660 */   426,   60,    4,  250,  170,  105,  115,  426,  255,  256,
 /*   670 */   426,  212,  127,  222,  270,  426,  426,  213,  426,  426,
 /*   680 */   426,  213,  426,  262,  170,  105,  115,  426,  255,  256,
 /*   690 */   212,  127,  220,   33,  212,  127,  222,  426,  426,  190,
 /*   700 */   210,  213,  426,  109,  426,  131,  426,  208,  209,  211,
 /*   710 */    59,  109,  426,  130,  212,  127,  222,  426,  426,  213,
 /*   720 */    77,  426,  109,  426,  152,  426,  426,  213,  426,  426,
 /*   730 */   426,  426,  212,  127,  222,  234,  235,  236,  213,  426,
 /*   740 */   212,  127,  222,  234,  235,  236,  426,  109,  426,  157,
 /*   750 */   426,  212,  127,  222,  234,  235,  236,  109,  426,  158,
 /*   760 */   426,  142,  190,  213,   31,  426,  426,  140,  100,  109,
 /*   770 */   426,  159,  426,  213,  426,  426,  212,  127,  222,  234,
 /*   780 */   235,  236,  213,  426,  426,  213,  212,  127,  222,  234,
 /*   790 */   235,  236,  109,  426,  160,  212,  127,  221,  212,  127,
 /*   800 */   222,  234,  235,  236,  426,  426,  426,  426,  213,  426,
 /*   810 */   426,  109,  426,  161,  426,  426,  426,  109,   77,  162,
 /*   820 */   426,  212,  127,  222,  234,  235,  236,  213,   77,  426,
 /*   830 */   426,  426,  426,  213,  426,  426,  426,  426,  426,  426,
 /*   840 */   212,  127,  222,  234,  235,  236,  212,  127,  222,  234,
 /*   850 */   235,  236,  109,  426,  163,  426,  426,  426,  426,  142,
 /*   860 */   190,  109,   31,  164,   97,  140,  426,  426,  213,  142,
 /*   870 */   190,  426,   31,  426,  426,  140,  426,  213,  426,  426,
 /*   880 */   426,  212,  127,  222,  234,  235,  236,  109,  426,  165,
 /*   890 */   212,  127,  222,  234,  235,  236,  426,  109,  426,  166,
 /*   900 */   426,  426,  426,  213,  426,  426,  426,  426,  426,  109,
 /*   910 */   426,  167,  426,  213,  426,  426,  212,  127,  222,  234,
 /*   920 */   235,  236,  426,  426,  426,  213,  212,  127,  222,  234,
 /*   930 */   235,  236,  426,  426,  426,  239,  426,  426,  212,  127,
 /*   940 */   222,  234,  235,  236,  109,  426,  168,  426,  426,  426,
 /*   950 */   426,  213,  426,  109,  426,  169,  426,  426,  426,  426,
 /*   960 */   213,  426,  426,  109,  212,  127,  222,  426,  426,  213,
 /*   970 */   426,  426,  426,  212,  127,  222,  234,  235,  236,  213,
 /*   980 */   426,  426,  212,  127,  222,  234,  235,  236,  426,  426,
 /*   990 */   426,  426,  212,  127,  222,  234,  235,  237,  426,  426,
 /*  1000 */   172,   86,  115,  426,  111,  265,  426,  107,  426,  173,
 /*  1010 */    74,  238,  426,  197,  144,  426,  426,  213,   98,  426,
 /*  1020 */   213,  426,  199,  213,  426,  426,  426,  213,  426,  108,
 /*  1030 */   212,  127,  222,  212,  127,  222,  212,  127,  222,  223,
 /*  1040 */   212,  127,  222,  426,  426,  213,  426,  426,  426,  224,
 /*  1050 */   426,  426,  426,  426,  426,  213,  426,  426,  212,  127,
 /*  1060 */   222,  225,  426,  426,  426,  213,  426,  426,  212,  127,
 /*  1070 */   222,  226,  426,  426,  426,  426,  426,  213,  212,  127,
 /*  1080 */   222,  227,  426,  426,  426,  426,  426,  213,  426,  426,
 /*  1090 */   212,  127,  222,  122,  426,  426,  426,  213,  426,  123,
 /*  1100 */   212,  127,  222,  124,  426,  426,  426,  426,  426,  213,
 /*  1110 */   212,  127,  222,  426,  426,  213,  426,  426,  426,  213,
 /*  1120 */   426,  125,  212,  127,  222,  116,  426,  426,  212,  127,
 /*  1130 */   222,  117,  212,  127,  222,  426,  426,  213,  426,  426,
 /*  1140 */   426,  213,  426,  426,  426,  426,  426,  213,  426,  426,
 /*  1150 */   212,  127,  222,  118,  212,  127,  222,  119,  426,  426,
 /*  1160 */   212,  127,  222,  120,  426,  426,  426,  426,  426,  213,
 /*  1170 */   426,  426,  426,  213,  426,  121,  426,  426,  426,  213,
 /*  1180 */   426,  426,  212,  127,  222,  114,  212,  127,  222,  426,
 /*  1190 */   426,  213,  212,  127,  222,  113,  426,  426,  426,  426,
 /*  1200 */   426,  213,  426,  426,  212,  127,  222,  110,  426,  426,
 /*  1210 */   426,  213,  426,   87,  212,  127,  222,   88,  426,  426,
 /*  1220 */   426,  426,  426,  213,  212,  127,  222,  426,  426,  213,
 /*  1230 */   426,  426,  426,  213,  426,  426,  212,  127,  222,  426,
 /*  1240 */   426,  426,  212,  127,  222,  426,  212,  127,  222,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */     9,   10,   56,   12,   91,   14,   93,   94,   75,   18,
 /*    10 */    20,   21,   22,   23,   24,   25,   26,   27,   28,   29,
 /*    20 */    30,   31,   32,   91,   91,   93,   94,   36,   37,   38,
 /*    30 */    39,   40,   56,   42,   43,   44,   45,  104,  105,  106,
 /*    40 */    49,   50,   79,   52,   53,   54,   55,   56,   57,   91,
 /*    50 */    45,   46,   62,   48,   50,   64,   65,   66,   67,   50,
 /*    60 */    56,   57,  104,  105,  106,    9,   10,   58,   12,   60,
 /*    70 */    14,   74,   75,   76,   18,    9,   10,   11,   12,   13,
 /*    80 */    14,   15,   16,   17,   18,   19,   69,   70,   91,   79,
 /*    90 */    71,   35,   36,   37,   38,   39,   40,   78,   42,   43,
 /*   100 */    44,  104,  105,  106,   91,   49,   50,   94,   52,   53,
 /*   110 */    54,   55,   56,   57,   91,   35,   75,   76,   56,   57,
 /*   120 */    64,   65,   66,   67,   35,   45,   81,  104,  105,  106,
 /*   130 */     9,   10,   91,   12,   85,   14,   85,   86,   49,   18,
 /*   140 */    60,   52,   53,   54,   55,  104,  105,  106,   73,   74,
 /*   150 */    75,   76,   91,   78,   79,   94,   50,   36,   37,   38,
 /*   160 */    39,   40,   56,   42,   43,   44,   91,   82,   83,   91,
 /*   170 */    49,   50,   85,   52,   53,   54,   55,   56,   57,  104,
 /*   180 */   105,  106,  104,  105,  106,   64,   65,   66,   67,    1,
 /*   190 */     2,    3,    4,    5,    6,    7,    8,    9,   10,   11,
 /*   200 */    12,   13,   14,   15,   16,   17,   18,   19,   13,   14,
 /*   210 */    15,   16,   17,   18,   19,    9,   10,   14,   12,   83,
 /*   220 */    14,   33,   34,   35,   18,   91,   87,   50,   94,   87,
 /*   230 */    88,   97,   44,   56,    1,    2,    3,    4,    5,    6,
 /*   240 */     7,    8,    9,   10,   11,   12,   13,   14,   15,   16,
 /*   250 */    17,   18,   19,   50,   79,   35,   50,   91,   55,   56,
 /*   260 */    94,   58,   56,   57,   61,   59,   33,   34,   35,  103,
 /*   270 */    64,   65,   66,   67,   35,   91,   14,   44,   94,    1,
 /*   280 */     2,    3,    4,    5,    6,    7,    8,    9,   10,   11,
 /*   290 */    12,   13,   14,   15,   16,   17,   18,   19,    1,    2,
 /*   300 */     3,    4,    5,    6,    7,    8,    9,   10,   11,   12,
 /*   310 */    13,   14,   15,   16,   17,   18,   19,   55,   56,   46,
 /*   320 */    58,   48,   44,   61,   71,   63,   91,   91,   71,   94,
 /*   330 */    94,   96,   96,   98,   50,   91,  101,  102,  102,   91,
 /*   340 */    56,   44,   94,    1,    2,    3,    4,    5,    6,    7,
 /*   350 */     8,    9,   10,   11,   12,   13,   14,   15,   16,   17,
 /*   360 */    18,   19,    1,    2,    3,    4,    5,    6,    7,    8,
 /*   370 */     9,   10,   11,   12,   13,   14,   15,   16,   17,   18,
 /*   380 */    19,    1,    2,    3,    4,    5,    6,    7,    8,    9,
 /*   390 */    10,   11,   12,   13,   14,   15,   16,   17,   18,   19,
 /*   400 */    62,   59,   89,   90,   91,   50,   89,   90,   91,   96,
 /*   410 */    35,   56,   51,   96,    1,    2,    3,    4,    5,    6,
 /*   420 */     7,    8,    9,   10,   11,   12,   13,   14,   15,   16,
 /*   430 */    17,   18,   19,   89,   89,   91,   91,   62,   87,   59,
 /*   440 */    96,   96,    2,    3,    4,    5,    6,    7,    8,    9,
 /*   450 */    10,   11,   12,   13,   14,   15,   16,   17,   18,   19,
 /*   460 */     3,    4,    5,    6,    7,    8,    9,   10,   11,   12,
 /*   470 */    13,   14,   15,   16,   17,   18,   19,    9,   10,  100,
 /*   480 */    12,   89,   14,   91,   91,   35,   18,   94,   96,   91,
 /*   490 */    91,   92,   94,   94,   95,   35,    9,   10,   99,   12,
 /*   500 */    91,   14,  103,   94,   44,   18,   91,   91,   47,   94,
 /*   510 */    94,   91,   44,   71,   35,   71,   51,   71,   50,   51,
 /*   520 */    52,   45,   35,   62,   56,   57,   75,   62,   77,   91,
 /*   530 */    44,   44,   64,   65,   66,   67,    0,   50,   62,   52,
 /*   540 */    35,   79,   91,   56,   57,   51,   57,   35,   51,   45,
 /*   550 */    60,   64,   65,   66,   67,  104,  105,  106,  107,  108,
 /*   560 */   109,    9,   10,   50,   12,   44,   14,   51,   51,   51,
 /*   570 */    18,   60,   50,   35,   91,   92,   51,   94,   95,   51,
 /*   580 */     9,   10,   99,   12,   20,   14,  103,   50,   62,   18,
 /*   590 */    35,   72,   73,   74,   75,   76,   44,   78,   79,   51,
 /*   600 */    51,   35,   50,   84,   52,   35,   41,   35,   56,   57,
 /*   610 */    91,  110,  110,   45,  110,   44,   64,   65,   66,   67,
 /*   620 */    47,   50,  110,  104,  105,  106,  110,   56,   57,  110,
 /*   630 */   110,  110,  110,  110,   91,   64,   65,   66,   67,   72,
 /*   640 */    73,   74,   75,   76,  110,   78,   79,  104,  105,  106,
 /*   650 */   110,   84,    9,   10,  110,   12,  110,   14,   91,  110,
 /*   660 */   110,   18,   72,   73,   74,   75,   76,  110,   78,   79,
 /*   670 */   110,  104,  105,  106,   84,  110,  110,   91,  110,  110,
 /*   680 */   110,   91,  110,   73,   74,   75,   76,  110,   78,   79,
 /*   690 */   104,  105,  106,   50,  104,  105,  106,  110,  110,   56,
 /*   700 */    57,   91,  110,   75,  110,   77,  110,   64,   65,   66,
 /*   710 */    67,   75,  110,   77,  104,  105,  106,  110,  110,   91,
 /*   720 */    14,  110,   75,  110,   77,  110,  110,   91,  110,  110,
 /*   730 */   110,  110,  104,  105,  106,  107,  108,  109,   91,  110,
 /*   740 */   104,  105,  106,  107,  108,  109,  110,   75,  110,   77,
 /*   750 */   110,  104,  105,  106,  107,  108,  109,   75,  110,   77,
 /*   760 */   110,   55,   56,   91,   58,  110,  110,   61,   62,   75,
 /*   770 */   110,   77,  110,   91,  110,  110,  104,  105,  106,  107,
 /*   780 */   108,  109,   91,  110,  110,   91,  104,  105,  106,  107,
 /*   790 */   108,  109,   75,  110,   77,  104,  105,  106,  104,  105,
 /*   800 */   106,  107,  108,  109,  110,  110,  110,  110,   91,  110,
 /*   810 */   110,   75,  110,   77,  110,  110,  110,   75,   14,   77,
 /*   820 */   110,  104,  105,  106,  107,  108,  109,   91,   14,  110,
 /*   830 */   110,  110,  110,   91,  110,  110,  110,  110,  110,  110,
 /*   840 */   104,  105,  106,  107,  108,  109,  104,  105,  106,  107,
 /*   850 */   108,  109,   75,  110,   77,  110,  110,  110,  110,   55,
 /*   860 */    56,   75,   58,   77,   60,   61,  110,  110,   91,   55,
 /*   870 */    56,  110,   58,  110,  110,   61,  110,   91,  110,  110,
 /*   880 */   110,  104,  105,  106,  107,  108,  109,   75,  110,   77,
 /*   890 */   104,  105,  106,  107,  108,  109,  110,   75,  110,   77,
 /*   900 */   110,  110,  110,   91,  110,  110,  110,  110,  110,   75,
 /*   910 */   110,   77,  110,   91,  110,  110,  104,  105,  106,  107,
 /*   920 */   108,  109,  110,  110,  110,   91,  104,  105,  106,  107,
 /*   930 */   108,  109,  110,  110,  110,   75,  110,  110,  104,  105,
 /*   940 */   106,  107,  108,  109,   75,  110,   77,  110,  110,  110,
 /*   950 */   110,   91,  110,   75,  110,   77,  110,  110,  110,  110,
 /*   960 */    91,  110,  110,   75,  104,  105,  106,  110,  110,   91,
 /*   970 */   110,  110,  110,  104,  105,  106,  107,  108,  109,   91,
 /*   980 */   110,  110,  104,  105,  106,  107,  108,  109,  110,  110,
 /*   990 */   110,  110,  104,  105,  106,  107,  108,  109,  110,  110,
 /*  1000 */    74,   75,   76,  110,   75,   79,  110,   75,  110,   80,
 /*  1010 */    91,   75,  110,   94,   95,  110,  110,   91,   99,  110,
 /*  1020 */    91,  110,  103,   91,  110,  110,  110,   91,  110,   75,
 /*  1030 */   104,  105,  106,  104,  105,  106,  104,  105,  106,   75,
 /*  1040 */   104,  105,  106,  110,  110,   91,  110,  110,  110,   75,
 /*  1050 */   110,  110,  110,  110,  110,   91,  110,  110,  104,  105,
 /*  1060 */   106,   75,  110,  110,  110,   91,  110,  110,  104,  105,
 /*  1070 */   106,   75,  110,  110,  110,  110,  110,   91,  104,  105,
 /*  1080 */   106,   75,  110,  110,  110,  110,  110,   91,  110,  110,
 /*  1090 */   104,  105,  106,   75,  110,  110,  110,   91,  110,   75,
 /*  1100 */   104,  105,  106,   75,  110,  110,  110,  110,  110,   91,
 /*  1110 */   104,  105,  106,  110,  110,   91,  110,  110,  110,   91,
 /*  1120 */   110,   75,  104,  105,  106,   75,  110,  110,  104,  105,
 /*  1130 */   106,   75,  104,  105,  106,  110,  110,   91,  110,  110,
 /*  1140 */   110,   91,  110,  110,  110,  110,  110,   91,  110,  110,
 /*  1150 */   104,  105,  106,   75,  104,  105,  106,   75,  110,  110,
 /*  1160 */   104,  105,  106,   75,  110,  110,  110,  110,  110,   91,
 /*  1170 */   110,  110,  110,   91,  110,   75,  110,  110,  110,   91,
 /*  1180 */   110,  110,  104,  105,  106,   75,  104,  105,  106,  110,
 /*  1190 */   110,   91,  104,  105,  106,   75,  110,  110,  110,  110,
 /*  1200 */   110,   91,  110,  110,  104,  105,  106,   75,  110,  110,
 /*  1210 */   110,   91,  110,   75,  104,  105,  106,   75,  110,  110,
 /*  1220 */   110,  110,  110,   91,  104,  105,  106,  110,  110,   91,
 /*  1230 */   110,  110,  110,   91,  110,  110,  104,  105,  106,  110,
 /*  1240 */   110,  110,  104,  105,  106,  110,  104,  105,  106,
};
#define YY_SHIFT_USE_DFLT (-55)
#define YY_SHIFT_MAX 175
static const short yy_shift_ofst[] = {
 /*     0 */   -55,   -9,   56,   56,   56,  121,  468,  487,  552,  552,
 /*    10 */   552,  552,  552,  552,  552,  552,  552,  552,  552,  552,
 /*    20 */   552,  552,  552,  552,  571,  643,  643,  643,  814,  814,
 /*    30 */   814,  206,  643,  643,  643,  643,  643,  643,  643,  643,
 /*    40 */   643,  643,  643,  643,  643,  643,  643,  643,  643,  643,
 /*    50 */   643,  643,  643,  643,  643,  643,  814,  643,  643,  643,
 /*    60 */   643,  643,  643,  814,  -54,  -54,  203,  262,  706,  203,
 /*    70 */   106,  177,  -54,   89,  804,  706,  814,  814,  814,  814,
 /*    80 */   814,  814,  814,   62,  273,  -24,  188,  278,  297,    4,
 /*    90 */     5,   62,  284,  460,  220,  239,  -24,  -54,  338,  450,
 /*   100 */   -54,  479,  479,  -54,  486,  233,  342,  361,  380,  413,
 /*   110 */   413,  413,  413,  440,  457,  -10,   66,   66,   66,   66,
 /*   120 */    66,   66,  195,  195,  195,  195,   80,    9,  355,  465,
 /*   130 */   476,  375,  461,  536,  505,  494,  489,  512,  497,  490,
 /*   140 */   521,  504,  513,  516,  517,  518,  511,  522,  525,  538,
 /*   150 */   528,  564,  526,  555,  548,  537,  549,  526,  526,  526,
 /*   160 */   526,  526,  526,  526,  526,  526,  526,  526,  526,  526,
 /*   170 */   566,  565,  570,  572,  573,  568,
};
#define YY_REDUCE_USE_DFLT (-88)
#define YY_REDUCE_MAX 104
static const short yy_reduce_ofst[] = {
 /*     0 */    17,  519,  567,  590,   75,  610,  451,  628,  636,  647,
 /*    10 */   672,  682,  694,  717,  736,  742,  777,  786,  812,  822,
 /*    20 */   834,  869,  878,  888,  926,   -3,  929,   41,  235,  399,
 /*    30 */   483,  -67,  860,  932,  936,  954,  964,  974,  986,  996,
 /*    40 */  1006, 1018, 1024, 1028, 1046, 1050, 1056, 1078, 1082, 1088,
 /*    50 */  1100, 1110, 1120, 1132, 1138, 1142,  919,  -42,   23,   78,
 /*    60 */   543,  586,  691,  236,  313,  317,  -87,  166,  134,  -68,
 /*    70 */   344,  345,  392,   19,   13,   61,  184,  248,  393,  398,
 /*    80 */   409,  415,  416,   51,   85,  142,  -37,   10,   45,   49,
 /*    90 */   136,   87,  139,  175,  253,  257,  351,  244,  379,  442,
 /*   100 */   420,  444,  446,  438,  462,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   316,  279,  413,  413,  414,  279,  424,  424,  424,  424,
 /*    10 */   424,  424,  424,  424,  424,  424,  424,  424,  424,  424,
 /*    20 */   424,  424,  424,  424,  279,  279,  411,  424,  422,  420,
 /*    30 */   420,  424,  424,  424,  424,  424,  424,  424,  424,  424,
 /*    40 */   424,  424,  424,  424,  424,  424,  424,  424,  424,  424,
 /*    50 */   424,  424,  424,  424,  424,  424,  424,  424,  424,  424,
 /*    60 */   424,  424,  424,  410,  424,  424,  330,  424,  415,  330,
 /*    70 */   424,  424,  410,  409,  346,  424,  424,  424,  424,  424,
 /*    80 */   424,  424,  424,  424,  424,  424,  407,  424,  424,  424,
 /*    90 */   424,  410,  424,  424,  409,  409,  410,  424,  357,  409,
 /*   100 */   424,  409,  409,  424,  424,  407,  424,  424,  424,  402,
 /*   110 */   408,  412,  407,  398,  397,  424,  391,  392,  393,  394,
 /*   120 */   395,  396,  387,  388,  389,  390,  418,  372,  424,  424,
 /*   130 */   424,  424,  424,  424,  424,  424,  424,  424,  424,  346,
 /*   140 */   424,  424,  424,  424,  424,  424,  399,  424,  424,  424,
 /*   150 */   424,  340,  341,  424,  424,  424,  424,  281,  282,  283,
 /*   160 */   284,  285,  286,  287,  288,  289,  290,  291,  292,  293,
 /*   170 */   424,  304,  424,  424,  424,  424,  317,  318,  319,  332,
 /*   180 */   333,  334,  331,  320,  321,  336,  337,  342,  343,  345,
 /*   190 */   417,  347,  354,  328,  329,  349,  353,  355,  356,  352,
 /*   200 */   348,  421,  350,  351,  358,  419,  359,  423,  360,  361,
 /*   210 */   362,  363,  364,  365,  366,  367,  373,  374,  375,  376,
 /*   220 */   377,  378,  379,  382,  383,  384,  385,  386,  368,  369,
 /*   230 */   370,  371,  400,  401,  403,  404,  405,  406,  381,  380,
 /*   240 */   344,  335,  322,  323,  339,  416,  418,  338,  324,  325,
 /*   250 */   277,  278,  294,  295,  296,  297,  298,  299,  300,  301,
 /*   260 */   302,  303,  305,  307,  306,  308,  309,  310,  312,  313,
 /*   270 */   314,  311,  315,  326,  327,  410,
};
#define YY_SZ_ACTTAB (int)(sizeof(yy_action)/sizeof(yy_action[0]))

/* The next table maps tokens into fallback tokens.  If a construct
** like the following:
** 
**      %fallback ID X Y Z.
**
** appears in the grammar, then ID becomes a fallback token for X, Y,
** and Z.  Whenever one of the tokens X, Y, or Z is input to the parser
** but it does not parse, the type of the token is changed to ID and
** the parse is retried before an error is thrown.
*/
#ifdef YYFALLBACK
static const YYCODETYPE yyFallback[] = {
};
#endif /* YYFALLBACK */

/* The following structure represents a single element of the
** parser's stack.  Information stored includes:
**
**   +  The state number for the parser at this level of the stack.
**
**   +  The value of the token stored at this level of the stack.
**      (In other words, the "major" token.)
**
**   +  The semantic value stored at this level of the stack.  This is
**      the information used by the action routines in the grammar.
**      It is sometimes called the "minor" token.
*/
struct yyStackEntry {
  YYACTIONTYPE stateno;  /* The state-number */
  YYCODETYPE major;      /* The major token value.  This is the code
                         ** number for the token at this stack level */
  YYMINORTYPE minor;     /* The user-supplied minor token value.  This
                         ** is the value of the token  */
};
typedef struct yyStackEntry yyStackEntry;

/* The state of the parser is completely contained in an instance of
** the following structure */
struct yyParser {
  int yyidx;                    /* Index of top element in stack */
#ifdef YYTRACKMAXSTACKDEPTH
  int yyidxMax;                 /* Maximum value of yyidx */
#endif
  int yyerrcnt;                 /* Shifts left before out of the error */
  ParseARG_SDECL                /* A place to hold %extra_argument */
#if YYSTACKDEPTH<=0
  int yystksz;                  /* Current side of the stack */
  yyStackEntry *yystack;        /* The parser's stack */
#else
  yyStackEntry yystack[YYSTACKDEPTH];  /* The parser's stack */
#endif
};
typedef struct yyParser yyParser;

#ifndef NDEBUG
#include <stdio.h>
static FILE *yyTraceFILE = 0;
static char *yyTracePrompt = 0;
#endif /* NDEBUG */

#ifndef NDEBUG
/* 
** Turn parser tracing on by giving a stream to which to write the trace
** and a prompt to preface each trace message.  Tracing is turned off
** by making either argument NULL 
**
** Inputs:
** <ul>
** <li> A FILE* to which trace output should be written.
**      If NULL, then tracing is turned off.
** <li> A prefix string written at the beginning of every
**      line of trace output.  If NULL, then tracing is
**      turned off.
** </ul>
**
** Outputs:
** None.
*/
void ParseTrace(FILE *TraceFILE, char *zTracePrompt){
  yyTraceFILE = TraceFILE;
  yyTracePrompt = zTracePrompt;
  if( yyTraceFILE==0 ) yyTracePrompt = 0;
  else if( yyTracePrompt==0 ) yyTraceFILE = 0;
}
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing shifts, the names of all terminals and nonterminals
** are required.  The following table supplies these names */
static const char *const yyTokenName[] = { 
  "$",             "OROR",          "ANDAND",        "EQ",          
  "NEQ",           "LT",            "LE",            "GT",          
  "GE",            "PLUS",          "MINUS",         "OR",          
  "XOR",           "DIVIDE",        "TIMES",         "MOD",         
  "SHIFTL",        "SHIFTR",        "AND",           "ANDNOT",      
  "ASSIGN",        "DECLARIZE",     "A_DIVIDE",      "A_TIMES",     
  "A_MOD",         "A_SHIFTL",      "A_SHIFTR",      "A_AND",       
  "A_ANDNOT",      "A_PLUS",        "A_MINUS",       "A_OR",        
  "A_XOR",         "INC",           "DEC",           "SEMICOLON",   
  "BREAK",         "CONTINUE",      "FALLTHROUGH",   "RETURN",      
  "IF",            "ELSE",          "FOR",           "SWITCH",      
  "LCURLY",        "RCURLY",        "CASE",          "COLON",       
  "DEFAULT",       "IMPORT",        "LPAREN",        "RPAREN",      
  "TYPE",          "CONST",         "VAR",           "FUNC",        
  "IDENT",         "STRING",        "LSB",           "RSB",         
  "DOT",           "STRUCT",        "COMMA",         "ELLIPSIS",    
  "INT",           "FLOAT",         "CHAR",          "NOT",         
  "error",         "program",       "decl_list",     "osemi",       
  "stmt_list",     "stmt",          "s_stmt",        "expr",        
  "expr_list",     "iexpr_list",    "decl",          "block_stmt",  
  "oexpr",         "sw_block_stmt",  "sw_case_list",  "sw_case",     
  "ostmt_list",    "import_spec",   "import_spec_list",  "type_spec",   
  "type_spec_list",  "value_spec",    "value_spec_list",  "ident",       
  "ofield_comma_list",  "func_results",  "type",          "field_comma_list",
  "ident_list",    "otype",         "ofield_semi_list_and_osemi",  "nametype_list",
  "ofunc_ellipsis",  "field_semi_list",  "field",         "nametype",    
  "basic_lit",     "pexpr",         "uexpr",         "compound_lit",
  "type_expr",     "iexpr",       
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *const yyRuleName[] = {
 /*   0 */ "program ::= decl_list osemi",
 /*   1 */ "stmt_list ::= stmt",
 /*   2 */ "stmt_list ::= stmt_list stmt",
 /*   3 */ "s_stmt ::=",
 /*   4 */ "s_stmt ::= expr",
 /*   5 */ "s_stmt ::= expr_list ASSIGN iexpr_list",
 /*   6 */ "s_stmt ::= expr_list DECLARIZE iexpr_list",
 /*   7 */ "s_stmt ::= expr_list A_DIVIDE iexpr_list",
 /*   8 */ "s_stmt ::= expr_list A_TIMES iexpr_list",
 /*   9 */ "s_stmt ::= expr_list A_MOD iexpr_list",
 /*  10 */ "s_stmt ::= expr_list A_SHIFTL iexpr_list",
 /*  11 */ "s_stmt ::= expr_list A_SHIFTR iexpr_list",
 /*  12 */ "s_stmt ::= expr_list A_AND iexpr_list",
 /*  13 */ "s_stmt ::= expr_list A_ANDNOT iexpr_list",
 /*  14 */ "s_stmt ::= expr_list A_PLUS iexpr_list",
 /*  15 */ "s_stmt ::= expr_list A_MINUS iexpr_list",
 /*  16 */ "s_stmt ::= expr_list A_OR iexpr_list",
 /*  17 */ "s_stmt ::= expr_list A_XOR iexpr_list",
 /*  18 */ "s_stmt ::= expr INC",
 /*  19 */ "s_stmt ::= expr DEC",
 /*  20 */ "stmt ::= s_stmt SEMICOLON",
 /*  21 */ "stmt ::= decl",
 /*  22 */ "stmt ::= block_stmt",
 /*  23 */ "stmt ::= BREAK",
 /*  24 */ "stmt ::= CONTINUE",
 /*  25 */ "stmt ::= FALLTHROUGH",
 /*  26 */ "stmt ::= RETURN iexpr_list SEMICOLON",
 /*  27 */ "stmt ::= RETURN SEMICOLON",
 /*  28 */ "stmt ::= IF expr block_stmt",
 /*  29 */ "stmt ::= IF expr block_stmt ELSE stmt",
 /*  30 */ "stmt ::= FOR s_stmt SEMICOLON oexpr SEMICOLON s_stmt block_stmt",
 /*  31 */ "stmt ::= FOR expr block_stmt",
 /*  32 */ "stmt ::= FOR block_stmt",
 /*  33 */ "stmt ::= SWITCH expr sw_block_stmt",
 /*  34 */ "sw_block_stmt ::= LCURLY sw_case_list RCURLY",
 /*  35 */ "sw_case_list ::= sw_case",
 /*  36 */ "sw_case_list ::= sw_case_list sw_case",
 /*  37 */ "sw_case ::= CASE expr_list COLON ostmt_list",
 /*  38 */ "sw_case ::= DEFAULT COLON ostmt_list",
 /*  39 */ "block_stmt ::= LCURLY ostmt_list RCURLY",
 /*  40 */ "decl_list ::=",
 /*  41 */ "decl_list ::= decl_list decl",
 /*  42 */ "decl ::= IMPORT import_spec SEMICOLON",
 /*  43 */ "decl ::= IMPORT LPAREN import_spec_list osemi RPAREN",
 /*  44 */ "decl ::= TYPE type_spec SEMICOLON",
 /*  45 */ "decl ::= TYPE LPAREN type_spec_list osemi RPAREN",
 /*  46 */ "decl ::= CONST value_spec SEMICOLON",
 /*  47 */ "decl ::= CONST LPAREN value_spec_list osemi RPAREN",
 /*  48 */ "decl ::= VAR value_spec SEMICOLON",
 /*  49 */ "decl ::= VAR LPAREN value_spec_list osemi RPAREN",
 /*  50 */ "decl ::= FUNC ident LPAREN ofield_comma_list RPAREN func_results SEMICOLON",
 /*  51 */ "decl ::= FUNC ident LPAREN ofield_comma_list RPAREN func_results block_stmt",
 /*  52 */ "func_results ::= type",
 /*  53 */ "func_results ::= LPAREN field_comma_list RPAREN",
 /*  54 */ "func_results ::=",
 /*  55 */ "import_spec_list ::= import_spec",
 /*  56 */ "import_spec_list ::= import_spec_list SEMICOLON import_spec",
 /*  57 */ "import_spec ::= IDENT STRING",
 /*  58 */ "import_spec ::= STRING",
 /*  59 */ "type_spec_list ::= type_spec",
 /*  60 */ "type_spec_list ::= type_spec_list SEMICOLON type_spec",
 /*  61 */ "type_spec ::= IDENT type",
 /*  62 */ "value_spec_list ::= value_spec",
 /*  63 */ "value_spec_list ::= value_spec_list SEMICOLON value_spec",
 /*  64 */ "value_spec ::= ident_list otype",
 /*  65 */ "value_spec ::= ident_list otype ASSIGN iexpr_list",
 /*  66 */ "type ::= TIMES type",
 /*  67 */ "type ::= LSB expr RSB type",
 /*  68 */ "type ::= LSB RSB type",
 /*  69 */ "type ::= ident DOT ident",
 /*  70 */ "type ::= ident",
 /*  71 */ "type ::= STRUCT LCURLY ofield_semi_list_and_osemi RCURLY",
 /*  72 */ "type ::= FUNC LPAREN ofield_comma_list RPAREN func_results",
 /*  73 */ "field_comma_list ::= nametype_list ofunc_ellipsis",
 /*  74 */ "field_semi_list ::= field",
 /*  75 */ "field_semi_list ::= field_semi_list SEMICOLON field",
 /*  76 */ "nametype_list ::= nametype",
 /*  77 */ "nametype_list ::= nametype_list COMMA nametype",
 /*  78 */ "nametype ::= ident type",
 /*  79 */ "nametype ::= type",
 /*  80 */ "ofunc_ellipsis ::= COMMA ELLIPSIS",
 /*  81 */ "ofunc_ellipsis ::=",
 /*  82 */ "field ::= ident_list type",
 /*  83 */ "field ::= type",
 /*  84 */ "basic_lit ::= INT",
 /*  85 */ "basic_lit ::= FLOAT",
 /*  86 */ "basic_lit ::= STRING",
 /*  87 */ "basic_lit ::= CHAR",
 /*  88 */ "pexpr ::= basic_lit",
 /*  89 */ "pexpr ::= ident",
 /*  90 */ "pexpr ::= LPAREN expr RPAREN",
 /*  91 */ "pexpr ::= pexpr LSB expr RSB",
 /*  92 */ "pexpr ::= pexpr DOT IDENT",
 /*  93 */ "pexpr ::= pexpr DOT LPAREN type RPAREN",
 /*  94 */ "pexpr ::= pexpr LPAREN RPAREN",
 /*  95 */ "pexpr ::= pexpr LPAREN iexpr_list RPAREN",
 /*  96 */ "uexpr ::= pexpr",
 /*  97 */ "uexpr ::= PLUS uexpr",
 /*  98 */ "uexpr ::= MINUS uexpr",
 /*  99 */ "uexpr ::= NOT uexpr",
 /* 100 */ "uexpr ::= AND uexpr",
 /* 101 */ "uexpr ::= TIMES uexpr",
 /* 102 */ "uexpr ::= XOR uexpr",
 /* 103 */ "expr ::= uexpr",
 /* 104 */ "expr ::= expr DIVIDE expr",
 /* 105 */ "expr ::= expr TIMES expr",
 /* 106 */ "expr ::= expr MOD expr",
 /* 107 */ "expr ::= expr SHIFTL expr",
 /* 108 */ "expr ::= expr SHIFTR expr",
 /* 109 */ "expr ::= expr AND expr",
 /* 110 */ "expr ::= expr ANDNOT expr",
 /* 111 */ "expr ::= expr PLUS expr",
 /* 112 */ "expr ::= expr MINUS expr",
 /* 113 */ "expr ::= expr OR expr",
 /* 114 */ "expr ::= expr XOR expr",
 /* 115 */ "expr ::= expr EQ expr",
 /* 116 */ "expr ::= expr NEQ expr",
 /* 117 */ "expr ::= expr LT expr",
 /* 118 */ "expr ::= expr LE expr",
 /* 119 */ "expr ::= expr GT expr",
 /* 120 */ "expr ::= expr GE expr",
 /* 121 */ "expr ::= expr ANDAND expr",
 /* 122 */ "expr ::= expr OROR expr",
 /* 123 */ "compound_lit ::= LCURLY iexpr_list RCURLY",
 /* 124 */ "compound_lit ::= LCURLY iexpr_list RCURLY DOT LPAREN type RPAREN",
 /* 125 */ "type_expr ::= TYPE type",
 /* 126 */ "iexpr ::= expr",
 /* 127 */ "iexpr ::= compound_lit",
 /* 128 */ "iexpr ::= type_expr",
 /* 129 */ "iexpr_list ::= iexpr",
 /* 130 */ "iexpr_list ::= iexpr_list COMMA iexpr",
 /* 131 */ "expr_list ::= expr",
 /* 132 */ "expr_list ::= expr_list COMMA expr",
 /* 133 */ "osemi ::=",
 /* 134 */ "osemi ::= SEMICOLON",
 /* 135 */ "oexpr ::=",
 /* 136 */ "oexpr ::= expr",
 /* 137 */ "ostmt_list ::=",
 /* 138 */ "ostmt_list ::= stmt_list",
 /* 139 */ "otype ::=",
 /* 140 */ "otype ::= type",
 /* 141 */ "ident ::= IDENT",
 /* 142 */ "ident_list ::= ident",
 /* 143 */ "ident_list ::= ident_list COMMA ident",
 /* 144 */ "ofield_comma_list ::=",
 /* 145 */ "ofield_comma_list ::= field_comma_list",
 /* 146 */ "ofield_semi_list_and_osemi ::=",
 /* 147 */ "ofield_semi_list_and_osemi ::= field_semi_list osemi",
};
#endif /* NDEBUG */


#if YYSTACKDEPTH<=0
/*
** Try to increase the size of the parser stack.
*/
static void yyGrowStack(yyParser *p){
  int newSize;
  yyStackEntry *pNew;

  newSize = p->yystksz*2 + 100;
  pNew = realloc(p->yystack, newSize*sizeof(pNew[0]));
  if( pNew ){
    p->yystack = pNew;
    p->yystksz = newSize;
#ifndef NDEBUG
    if( yyTraceFILE ){
      fprintf(yyTraceFILE,"%sStack grows to %d entries!\n",
              yyTracePrompt, p->yystksz);
    }
#endif
  }
}
#endif

/* 
** This function allocates a new parser.
** The only argument is a pointer to a function which works like
** malloc.
**
** Inputs:
** A pointer to the function used to allocate memory.
**
** Outputs:
** A pointer to a parser.  This pointer is used in subsequent calls
** to Parse and ParseFree.
*/
void *ParseAlloc(void *(*mallocProc)(size_t)){
  yyParser *pParser;
  pParser = (yyParser*)(*mallocProc)( (size_t)sizeof(yyParser) );
  if( pParser ){
    pParser->yyidx = -1;
#ifdef YYTRACKMAXSTACKDEPTH
    pParser->yyidxMax = 0;
#endif
#if YYSTACKDEPTH<=0
    pParser->yystack = NULL;
    pParser->yystksz = 0;
    yyGrowStack(pParser);
#endif
  }
  return pParser;
}

/* The following function deletes the value associated with a
** symbol.  The symbol can be either a terminal or nonterminal.
** "yymajor" is the symbol code, and "yypminor" is a pointer to
** the value.
*/
static void yy_destructor(
  yyParser *yypParser,    /* The parser */
  YYCODETYPE yymajor,     /* Type code for object to destroy */
  YYMINORTYPE *yypminor   /* The object to be destroyed */
){
  ParseARG_FETCH;
  switch( yymajor ){
    /* Here is inserted the actions which take place when a
    ** terminal or non-terminal is destroyed.  This can happen
    ** when the symbol is popped from the stack during a
    ** reduce or during error processing or when a parser is 
    ** being destroyed before it is finished parsing.
    **
    ** Note: during a reduce, the only symbols destroyed are those
    ** which appear on the RHS of the rule, but which are not used
    ** inside the C code.
    */
      /* TERMINAL Destructor */
    case 1: /* OROR */
    case 2: /* ANDAND */
    case 3: /* EQ */
    case 4: /* NEQ */
    case 5: /* LT */
    case 6: /* LE */
    case 7: /* GT */
    case 8: /* GE */
    case 9: /* PLUS */
    case 10: /* MINUS */
    case 11: /* OR */
    case 12: /* XOR */
    case 13: /* DIVIDE */
    case 14: /* TIMES */
    case 15: /* MOD */
    case 16: /* SHIFTL */
    case 17: /* SHIFTR */
    case 18: /* AND */
    case 19: /* ANDNOT */
    case 20: /* ASSIGN */
    case 21: /* DECLARIZE */
    case 22: /* A_DIVIDE */
    case 23: /* A_TIMES */
    case 24: /* A_MOD */
    case 25: /* A_SHIFTL */
    case 26: /* A_SHIFTR */
    case 27: /* A_AND */
    case 28: /* A_ANDNOT */
    case 29: /* A_PLUS */
    case 30: /* A_MINUS */
    case 31: /* A_OR */
    case 32: /* A_XOR */
    case 33: /* INC */
    case 34: /* DEC */
    case 35: /* SEMICOLON */
    case 36: /* BREAK */
    case 37: /* CONTINUE */
    case 38: /* FALLTHROUGH */
    case 39: /* RETURN */
    case 40: /* IF */
    case 41: /* ELSE */
    case 42: /* FOR */
    case 43: /* SWITCH */
    case 44: /* LCURLY */
    case 45: /* RCURLY */
    case 46: /* CASE */
    case 47: /* COLON */
    case 48: /* DEFAULT */
    case 49: /* IMPORT */
    case 50: /* LPAREN */
    case 51: /* RPAREN */
    case 52: /* TYPE */
    case 53: /* CONST */
    case 54: /* VAR */
    case 55: /* FUNC */
    case 56: /* IDENT */
    case 57: /* STRING */
    case 58: /* LSB */
    case 59: /* RSB */
    case 60: /* DOT */
    case 61: /* STRUCT */
    case 62: /* COMMA */
    case 63: /* ELLIPSIS */
    case 64: /* INT */
    case 65: /* FLOAT */
    case 66: /* CHAR */
    case 67: /* NOT */
{
#line 2 "parser.y"
 delete (yypminor->yy0); 
#line 913 "parser.c"
}
      break;
    default:  break;   /* If no destructor action specified: do nothing */
  }
}

/*
** Pop the parser's stack once.
**
** If there is a destructor routine associated with the token which
** is popped from the stack, then call it.
**
** Return the major token number for the symbol popped.
*/
static int yy_pop_parser_stack(yyParser *pParser){
  YYCODETYPE yymajor;
  yyStackEntry *yytos = &pParser->yystack[pParser->yyidx];

  if( pParser->yyidx<0 ) return 0;
#ifndef NDEBUG
  if( yyTraceFILE && pParser->yyidx>=0 ){
    fprintf(yyTraceFILE,"%sPopping %s\n",
      yyTracePrompt,
      yyTokenName[yytos->major]);
  }
#endif
  yymajor = yytos->major;
  yy_destructor(pParser, yymajor, &yytos->minor);
  pParser->yyidx--;
  return yymajor;
}

/* 
** Deallocate and destroy a parser.  Destructors are all called for
** all stack elements before shutting the parser down.
**
** Inputs:
** <ul>
** <li>  A pointer to the parser.  This should be a pointer
**       obtained from ParseAlloc.
** <li>  A pointer to a function used to reclaim memory obtained
**       from malloc.
** </ul>
*/
void ParseFree(
  void *p,                    /* The parser to be deleted */
  void (*freeProc)(void*)     /* Function used to reclaim memory */
){
  yyParser *pParser = (yyParser*)p;
  if( pParser==0 ) return;
  while( pParser->yyidx>=0 ) yy_pop_parser_stack(pParser);
#if YYSTACKDEPTH<=0
  free(pParser->yystack);
#endif
  (*freeProc)((void*)pParser);
}

/*
** Return the peak depth of the stack for a parser.
*/
#ifdef YYTRACKMAXSTACKDEPTH
int ParseStackPeak(void *p){
  yyParser *pParser = (yyParser*)p;
  return pParser->yyidxMax;
}
#endif

/*
** Find the appropriate action for a parser given the terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_shift_action(
  yyParser *pParser,        /* The parser */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
  int stateno = pParser->yystack[pParser->yyidx].stateno;
 
  if( stateno>YY_SHIFT_MAX || (i = yy_shift_ofst[stateno])==YY_SHIFT_USE_DFLT ){
    return yy_default[stateno];
  }
  assert( iLookAhead!=YYNOCODE );
  i += iLookAhead;
  if( i<0 || i>=YY_SZ_ACTTAB || yy_lookahead[i]!=iLookAhead ){
    if( iLookAhead>0 ){
#ifdef YYFALLBACK
      YYCODETYPE iFallback;            /* Fallback token */
      if( iLookAhead<sizeof(yyFallback)/sizeof(yyFallback[0])
             && (iFallback = yyFallback[iLookAhead])!=0 ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE, "%sFALLBACK %s => %s\n",
             yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[iFallback]);
        }
#endif
        return yy_find_shift_action(pParser, iFallback);
      }
#endif
#ifdef YYWILDCARD
      {
        int j = i - iLookAhead + YYWILDCARD;
        if( j>=0 && j<YY_SZ_ACTTAB && yy_lookahead[j]==YYWILDCARD ){
#ifndef NDEBUG
          if( yyTraceFILE ){
            fprintf(yyTraceFILE, "%sWILDCARD %s => %s\n",
               yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[YYWILDCARD]);
          }
#endif /* NDEBUG */
          return yy_action[j];
        }
      }
#endif /* YYWILDCARD */
    }
    return yy_default[stateno];
  }else{
    return yy_action[i];
  }
}

/*
** Find the appropriate action for a parser given the non-terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_reduce_action(
  int stateno,              /* Current state number */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
#ifdef YYERRORSYMBOL
  if( stateno>YY_REDUCE_MAX ){
    return yy_default[stateno];
  }
#else
  assert( stateno<=YY_REDUCE_MAX );
#endif
  i = yy_reduce_ofst[stateno];
  assert( i!=YY_REDUCE_USE_DFLT );
  assert( iLookAhead!=YYNOCODE );
  i += iLookAhead;
#ifdef YYERRORSYMBOL
  if( i<0 || i>=YY_SZ_ACTTAB || yy_lookahead[i]!=iLookAhead ){
    return yy_default[stateno];
  }
#else
  assert( i>=0 && i<YY_SZ_ACTTAB );
  assert( yy_lookahead[i]==iLookAhead );
#endif
  return yy_action[i];
}

/*
** The following routine is called if the stack overflows.
*/
static void yyStackOverflow(yyParser *yypParser, YYMINORTYPE *yypMinor){
   ParseARG_FETCH;
   yypParser->yyidx--;
#ifndef NDEBUG
   if( yyTraceFILE ){
     fprintf(yyTraceFILE,"%sStack Overflow!\n",yyTracePrompt);
   }
#endif
   while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
   /* Here code is inserted which will execute if the parser
   ** stack every overflows */
   ParseARG_STORE; /* Suppress warning about unused %extra_argument var */
}

/*
** Perform a shift action.
*/
static void yy_shift(
  yyParser *yypParser,          /* The parser to be shifted */
  int yyNewState,               /* The new state to shift in */
  int yyMajor,                  /* The major token to shift in */
  YYMINORTYPE *yypMinor         /* Pointer to the minor token to shift in */
){
  yyStackEntry *yytos;
  yypParser->yyidx++;
#ifdef YYTRACKMAXSTACKDEPTH
  if( yypParser->yyidx>yypParser->yyidxMax ){
    yypParser->yyidxMax = yypParser->yyidx;
  }
#endif
#if YYSTACKDEPTH>0 
  if( yypParser->yyidx>=YYSTACKDEPTH ){
    yyStackOverflow(yypParser, yypMinor);
    return;
  }
#else
  if( yypParser->yyidx>=yypParser->yystksz ){
    yyGrowStack(yypParser);
    if( yypParser->yyidx>=yypParser->yystksz ){
      yyStackOverflow(yypParser, yypMinor);
      return;
    }
  }
#endif
  yytos = &yypParser->yystack[yypParser->yyidx];
  yytos->stateno = (YYACTIONTYPE)yyNewState;
  yytos->major = (YYCODETYPE)yyMajor;
  yytos->minor = *yypMinor;
#ifndef NDEBUG
  if( yyTraceFILE && yypParser->yyidx>0 ){
    int i;
    fprintf(yyTraceFILE,"%sShift %d\n",yyTracePrompt,yyNewState);
    fprintf(yyTraceFILE,"%sStack:",yyTracePrompt);
    for(i=1; i<=yypParser->yyidx; i++)
      fprintf(yyTraceFILE," %s",yyTokenName[yypParser->yystack[i].major]);
    fprintf(yyTraceFILE,"\n");
  }
#endif
}

/* The following table contains information about every rule that
** is used during the reduce.
*/
static const struct {
  YYCODETYPE lhs;         /* Symbol on the left-hand side of the rule */
  unsigned char nrhs;     /* Number of right-hand side symbols in the rule */
} yyRuleInfo[] = {
  { 69, 2 },
  { 72, 1 },
  { 72, 2 },
  { 74, 0 },
  { 74, 1 },
  { 74, 3 },
  { 74, 3 },
  { 74, 3 },
  { 74, 3 },
  { 74, 3 },
  { 74, 3 },
  { 74, 3 },
  { 74, 3 },
  { 74, 3 },
  { 74, 3 },
  { 74, 3 },
  { 74, 3 },
  { 74, 3 },
  { 74, 2 },
  { 74, 2 },
  { 73, 2 },
  { 73, 1 },
  { 73, 1 },
  { 73, 1 },
  { 73, 1 },
  { 73, 1 },
  { 73, 3 },
  { 73, 2 },
  { 73, 3 },
  { 73, 5 },
  { 73, 7 },
  { 73, 3 },
  { 73, 2 },
  { 73, 3 },
  { 81, 3 },
  { 82, 1 },
  { 82, 2 },
  { 83, 4 },
  { 83, 3 },
  { 79, 3 },
  { 70, 0 },
  { 70, 2 },
  { 78, 3 },
  { 78, 5 },
  { 78, 3 },
  { 78, 5 },
  { 78, 3 },
  { 78, 5 },
  { 78, 3 },
  { 78, 5 },
  { 78, 7 },
  { 78, 7 },
  { 93, 1 },
  { 93, 3 },
  { 93, 0 },
  { 86, 1 },
  { 86, 3 },
  { 85, 2 },
  { 85, 1 },
  { 88, 1 },
  { 88, 3 },
  { 87, 2 },
  { 90, 1 },
  { 90, 3 },
  { 89, 2 },
  { 89, 4 },
  { 94, 2 },
  { 94, 4 },
  { 94, 3 },
  { 94, 3 },
  { 94, 1 },
  { 94, 4 },
  { 94, 5 },
  { 95, 2 },
  { 101, 1 },
  { 101, 3 },
  { 99, 1 },
  { 99, 3 },
  { 103, 2 },
  { 103, 1 },
  { 100, 2 },
  { 100, 0 },
  { 102, 2 },
  { 102, 1 },
  { 104, 1 },
  { 104, 1 },
  { 104, 1 },
  { 104, 1 },
  { 105, 1 },
  { 105, 1 },
  { 105, 3 },
  { 105, 4 },
  { 105, 3 },
  { 105, 5 },
  { 105, 3 },
  { 105, 4 },
  { 106, 1 },
  { 106, 2 },
  { 106, 2 },
  { 106, 2 },
  { 106, 2 },
  { 106, 2 },
  { 106, 2 },
  { 75, 1 },
  { 75, 3 },
  { 75, 3 },
  { 75, 3 },
  { 75, 3 },
  { 75, 3 },
  { 75, 3 },
  { 75, 3 },
  { 75, 3 },
  { 75, 3 },
  { 75, 3 },
  { 75, 3 },
  { 75, 3 },
  { 75, 3 },
  { 75, 3 },
  { 75, 3 },
  { 75, 3 },
  { 75, 3 },
  { 75, 3 },
  { 75, 3 },
  { 107, 3 },
  { 107, 7 },
  { 108, 2 },
  { 109, 1 },
  { 109, 1 },
  { 109, 1 },
  { 77, 1 },
  { 77, 3 },
  { 76, 1 },
  { 76, 3 },
  { 71, 0 },
  { 71, 1 },
  { 80, 0 },
  { 80, 1 },
  { 84, 0 },
  { 84, 1 },
  { 97, 0 },
  { 97, 1 },
  { 91, 1 },
  { 96, 1 },
  { 96, 3 },
  { 92, 0 },
  { 92, 1 },
  { 98, 0 },
  { 98, 2 },
};

static void yy_accept(yyParser*);  /* Forward Declaration */

/*
** Perform a reduce action and the shift that must immediately
** follow the reduce.
*/
static void yy_reduce(
  yyParser *yypParser,         /* The parser */
  int yyruleno                 /* Number of the rule by which to reduce */
){
  int yygoto;                     /* The next state */
  int yyact;                      /* The next action */
  YYMINORTYPE yygotominor;        /* The LHS of the rule reduced */
  yyStackEntry *yymsp;            /* The top of the parser's stack */
  int yysize;                     /* Amount to pop the stack */
  ParseARG_FETCH;
  yymsp = &yypParser->yystack[yypParser->yyidx];
#ifndef NDEBUG
  if( yyTraceFILE && yyruleno>=0 
        && yyruleno<(int)(sizeof(yyRuleName)/sizeof(yyRuleName[0])) ){
    fprintf(yyTraceFILE, "%sReduce [%s].\n", yyTracePrompt,
      yyRuleName[yyruleno]);
  }
#endif /* NDEBUG */

  /* Silence complaints from purify about yygotominor being uninitialized
  ** in some cases when it is copied into the stack after the following
  ** switch.  yygotominor is uninitialized when a rule reduces that does
  ** not set the value of its left-hand side nonterminal.  Leaving the
  ** value of the nonterminal uninitialized is utterly harmless as long
  ** as the value is never used.  So really the only thing this code
  ** accomplishes is to quieten purify.  
  **
  ** 2007-01-16:  The wireshark project (www.wireshark.org) reports that
  ** without this code, their parser segfaults.  I'm not sure what there
  ** parser is doing to make this happen.  This is the second bug report
  ** from wireshark this week.  Clearly they are stressing Lemon in ways
  ** that it has not been previously stressed...  (SQLite ticket #2172)
  */
  /*memset(&yygotominor, 0, sizeof(yygotominor));*/
  yygotominor = yyzerominor;


  switch( yyruleno ){
  /* Beginning here are the reduction cases.  A typical example
  ** follows:
  **   case 0:
  **  #line <lineno> <grammarfile>
  **     { ... }           // User supplied code
  **  #line <lineno> <thisfile>
  **     break;
  */
      case 0: /* program ::= decl_list osemi */
#line 31 "parser.y"
{
	yygotominor.yy74 = new program_t(yymsp[-1].minor.yy70);
	ctx->ast = yygotominor.yy74;
}
#line 1350 "parser.c"
        break;
      case 1: /* stmt_list ::= stmt */
#line 44 "parser.y"
{ 
	yygotominor.yy70 = new node_vector_t;
	if (yymsp[0].minor.yy100) yygotominor.yy70->push_back(yymsp[0].minor.yy100);
}
#line 1358 "parser.c"
        break;
      case 2: /* stmt_list ::= stmt_list stmt */
#line 48 "parser.y"
{
	if (yymsp[0].minor.yy100) yymsp[-1].minor.yy70->push_back(yymsp[0].minor.yy100);
	yygotominor.yy70 = yymsp[-1].minor.yy70;
}
#line 1366 "parser.c"
        break;
      case 3: /* s_stmt ::= */
      case 135: /* oexpr ::= */ yytestcase(yyruleno==135);
      case 139: /* otype ::= */ yytestcase(yyruleno==139);
#line 61 "parser.y"
{ yygotominor.yy100 = 0; }
#line 1373 "parser.c"
        break;
      case 4: /* s_stmt ::= expr */
#line 62 "parser.y"
{ yygotominor.yy100 = new expr_stmt_t(yymsp[0].minor.yy100); }
#line 1378 "parser.c"
        break;
      case 5: /* s_stmt ::= expr_list ASSIGN iexpr_list */
      case 6: /* s_stmt ::= expr_list DECLARIZE iexpr_list */ yytestcase(yyruleno==6);
      case 7: /* s_stmt ::= expr_list A_DIVIDE iexpr_list */ yytestcase(yyruleno==7);
      case 8: /* s_stmt ::= expr_list A_TIMES iexpr_list */ yytestcase(yyruleno==8);
      case 9: /* s_stmt ::= expr_list A_MOD iexpr_list */ yytestcase(yyruleno==9);
      case 10: /* s_stmt ::= expr_list A_SHIFTL iexpr_list */ yytestcase(yyruleno==10);
      case 11: /* s_stmt ::= expr_list A_SHIFTR iexpr_list */ yytestcase(yyruleno==11);
      case 12: /* s_stmt ::= expr_list A_AND iexpr_list */ yytestcase(yyruleno==12);
      case 13: /* s_stmt ::= expr_list A_ANDNOT iexpr_list */ yytestcase(yyruleno==13);
      case 14: /* s_stmt ::= expr_list A_PLUS iexpr_list */ yytestcase(yyruleno==14);
      case 15: /* s_stmt ::= expr_list A_MINUS iexpr_list */ yytestcase(yyruleno==15);
      case 16: /* s_stmt ::= expr_list A_OR iexpr_list */ yytestcase(yyruleno==16);
      case 17: /* s_stmt ::= expr_list A_XOR iexpr_list */ yytestcase(yyruleno==17);
#line 63 "parser.y"
{ yygotominor.yy100 = new assign_stmt_t(yymsp[-2].minor.yy70, yymsp[0].minor.yy70, yymsp[-1].minor.yy0); }
#line 1395 "parser.c"
        break;
      case 18: /* s_stmt ::= expr INC */
      case 19: /* s_stmt ::= expr DEC */ yytestcase(yyruleno==19);
#line 76 "parser.y"
{ yygotominor.yy100 = new incdec_stmt_t(yymsp[-1].minor.yy100, yymsp[0].minor.yy0); }
#line 1401 "parser.c"
        break;
      case 20: /* stmt ::= s_stmt SEMICOLON */
#line 87 "parser.y"
{ yygotominor.yy100 = yymsp[-1].minor.yy100;   yy_destructor(yypParser,35,&yymsp[0].minor);
}
#line 1407 "parser.c"
        break;
      case 21: /* stmt ::= decl */
#line 88 "parser.y"
{ yygotominor.yy100 = new decl_stmt_t(yymsp[0].minor.yy100); }
#line 1412 "parser.c"
        break;
      case 22: /* stmt ::= block_stmt */
#line 89 "parser.y"
{ yygotominor.yy100 = yymsp[0].minor.yy220; }
#line 1417 "parser.c"
        break;
      case 23: /* stmt ::= BREAK */
      case 24: /* stmt ::= CONTINUE */ yytestcase(yyruleno==24);
      case 25: /* stmt ::= FALLTHROUGH */ yytestcase(yyruleno==25);
#line 90 "parser.y"
{ yygotominor.yy100 = new flow_stmt_t(yymsp[0].minor.yy0); }
#line 1424 "parser.c"
        break;
      case 26: /* stmt ::= RETURN iexpr_list SEMICOLON */
#line 93 "parser.y"
{ yygotominor.yy100 = new return_stmt_t(yymsp[-1].minor.yy70, yymsp[-2].minor.yy0);   yy_destructor(yypParser,35,&yymsp[0].minor);
}
#line 1430 "parser.c"
        break;
      case 27: /* stmt ::= RETURN SEMICOLON */
#line 94 "parser.y"
{ yygotominor.yy100 = new return_stmt_t(0, yymsp[-1].minor.yy0);   yy_destructor(yypParser,35,&yymsp[0].minor);
}
#line 1436 "parser.c"
        break;
      case 28: /* stmt ::= IF expr block_stmt */
#line 96 "parser.y"
{
	yygotominor.yy100 = new ifelse_stmt_t(yymsp[-1].minor.yy100, yymsp[0].minor.yy220, yymsp[-2].minor.yy0);
}
#line 1443 "parser.c"
        break;
      case 29: /* stmt ::= IF expr block_stmt ELSE stmt */
#line 99 "parser.y"
{
	yygotominor.yy100 = new ifelse_stmt_t(yymsp[-3].minor.yy100, yymsp[-2].minor.yy220, yymsp[-4].minor.yy0, yymsp[-1].minor.yy0, yymsp[0].minor.yy100);
}
#line 1450 "parser.c"
        break;
      case 30: /* stmt ::= FOR s_stmt SEMICOLON oexpr SEMICOLON s_stmt block_stmt */
#line 102 "parser.y"
{
	yygotominor.yy100 = new for_stmt_t(yymsp[-5].minor.yy100, yymsp[-3].minor.yy100, yymsp[-1].minor.yy100, yymsp[0].minor.yy220, yymsp[-6].minor.yy0);
  yy_destructor(yypParser,35,&yymsp[-4].minor);
  yy_destructor(yypParser,35,&yymsp[-2].minor);
}
#line 1459 "parser.c"
        break;
      case 31: /* stmt ::= FOR expr block_stmt */
#line 105 "parser.y"
{
	yygotominor.yy100 = new for_stmt_t(0, yymsp[-1].minor.yy100, 0, yymsp[0].minor.yy220, yymsp[-2].minor.yy0);
}
#line 1466 "parser.c"
        break;
      case 32: /* stmt ::= FOR block_stmt */
#line 108 "parser.y"
{
	yygotominor.yy100 = new for_stmt_t(0, 0, 0, yymsp[0].minor.yy220, yymsp[-1].minor.yy0);
}
#line 1473 "parser.c"
        break;
      case 33: /* stmt ::= SWITCH expr sw_block_stmt */
#line 111 "parser.y"
{
	yygotominor.yy100 = new switch_stmt_t(yymsp[-1].minor.yy100, yymsp[0].minor.yy220, yymsp[-2].minor.yy0);
}
#line 1480 "parser.c"
        break;
      case 34: /* sw_block_stmt ::= LCURLY sw_case_list RCURLY */
      case 39: /* block_stmt ::= LCURLY ostmt_list RCURLY */ yytestcase(yyruleno==39);
#line 123 "parser.y"
{
	yygotominor.yy220 = new block_stmt_t(yymsp[-1].minor.yy70, yymsp[-2].minor.yy0, yymsp[0].minor.yy0);
}
#line 1488 "parser.c"
        break;
      case 35: /* sw_case_list ::= sw_case */
#line 128 "parser.y"
{ yygotominor.yy70 = new node_vector_t(1, yymsp[0].minor.yy194); }
#line 1493 "parser.c"
        break;
      case 36: /* sw_case_list ::= sw_case_list sw_case */
#line 129 "parser.y"
{ yymsp[-1].minor.yy70->push_back(yymsp[0].minor.yy194); yygotominor.yy70 = yymsp[-1].minor.yy70; }
#line 1498 "parser.c"
        break;
      case 37: /* sw_case ::= CASE expr_list COLON ostmt_list */
#line 132 "parser.y"
{
	yygotominor.yy194 = new switch_stmt_case_t(yymsp[-2].minor.yy70, yymsp[0].minor.yy70, yymsp[-3].minor.yy0, yymsp[-1].minor.yy0);
}
#line 1505 "parser.c"
        break;
      case 38: /* sw_case ::= DEFAULT COLON ostmt_list */
#line 135 "parser.y"
{
	yygotominor.yy194 = new switch_stmt_case_t(0, yymsp[0].minor.yy70, yymsp[-2].minor.yy0, yymsp[-1].minor.yy0);
}
#line 1512 "parser.c"
        break;
      case 40: /* decl_list ::= */
#line 159 "parser.y"
{ yygotominor.yy70 = new node_vector_t; }
#line 1517 "parser.c"
        break;
      case 41: /* decl_list ::= decl_list decl */
#line 160 "parser.y"
{ yymsp[-1].minor.yy70->push_back(yymsp[0].minor.yy100); yygotominor.yy70 = yymsp[-1].minor.yy70; }
#line 1522 "parser.c"
        break;
      case 42: /* decl ::= IMPORT import_spec SEMICOLON */
#line 176 "parser.y"
{
	yygotominor.yy100 = new import_decl_t(yymsp[-1].minor.yy203, yymsp[-2].minor.yy0);
  yy_destructor(yypParser,35,&yymsp[0].minor);
}
#line 1530 "parser.c"
        break;
      case 43: /* decl ::= IMPORT LPAREN import_spec_list osemi RPAREN */
#line 179 "parser.y"
{
	yygotominor.yy100 = new import_decl_t(yymsp[-2].minor.yy41, yymsp[-4].minor.yy0, yymsp[-3].minor.yy0, yymsp[0].minor.yy0);
}
#line 1537 "parser.c"
        break;
      case 44: /* decl ::= TYPE type_spec SEMICOLON */
#line 182 "parser.y"
{
	yygotominor.yy100 = new type_decl_t(yymsp[-1].minor.yy78, yymsp[-2].minor.yy0);
  yy_destructor(yypParser,35,&yymsp[0].minor);
}
#line 1545 "parser.c"
        break;
      case 45: /* decl ::= TYPE LPAREN type_spec_list osemi RPAREN */
#line 185 "parser.y"
{
	yygotominor.yy100 = new type_decl_t(yymsp[-2].minor.yy60, yymsp[-4].minor.yy0, yymsp[-3].minor.yy0, yymsp[0].minor.yy0);
}
#line 1552 "parser.c"
        break;
      case 46: /* decl ::= CONST value_spec SEMICOLON */
#line 188 "parser.y"
{
	const_decl_t *d = new const_decl_t(yymsp[-1].minor.yy77, yymsp[-2].minor.yy0);
	syntax_check_const_decl(ctx->diag, d);
	yygotominor.yy100 = d;
  yy_destructor(yypParser,35,&yymsp[0].minor);
}
#line 1562 "parser.c"
        break;
      case 47: /* decl ::= CONST LPAREN value_spec_list osemi RPAREN */
#line 193 "parser.y"
{
	const_decl_t *d = new const_decl_t(yymsp[-2].minor.yy175, yymsp[-4].minor.yy0, yymsp[-3].minor.yy0, yymsp[0].minor.yy0);
	syntax_check_const_decl(ctx->diag, d);
	yygotominor.yy100 = d;
}
#line 1571 "parser.c"
        break;
      case 48: /* decl ::= VAR value_spec SEMICOLON */
#line 198 "parser.y"
{
	var_decl_t *d = new var_decl_t(yymsp[-1].minor.yy77, yymsp[-2].minor.yy0);
	syntax_check_var_decl(ctx->diag, d);
	yygotominor.yy100 = d;
  yy_destructor(yypParser,35,&yymsp[0].minor);
}
#line 1581 "parser.c"
        break;
      case 49: /* decl ::= VAR LPAREN value_spec_list osemi RPAREN */
#line 203 "parser.y"
{
	var_decl_t *d = new var_decl_t(yymsp[-2].minor.yy175, yymsp[-4].minor.yy0, yymsp[-3].minor.yy0, yymsp[0].minor.yy0);
	syntax_check_var_decl(ctx->diag, d);
	yygotominor.yy100 = d;
}
#line 1590 "parser.c"
        break;
      case 50: /* decl ::= FUNC ident LPAREN ofield_comma_list RPAREN func_results SEMICOLON */
#line 211 "parser.y"
{
	func_type_t *ftype = new func_type_t(yymsp[-3].minor.yy196, yymsp[-1].minor.yy196, yymsp[-6].minor.yy0);
	yygotominor.yy100 = new func_decl_t(yymsp[-5].minor.yy160, ftype);
  yy_destructor(yypParser,50,&yymsp[-4].minor);
  yy_destructor(yypParser,51,&yymsp[-2].minor);
  yy_destructor(yypParser,35,&yymsp[0].minor);
}
#line 1601 "parser.c"
        break;
      case 51: /* decl ::= FUNC ident LPAREN ofield_comma_list RPAREN func_results block_stmt */
#line 218 "parser.y"
{
	func_type_t *ftype = new func_type_t(yymsp[-3].minor.yy196, yymsp[-1].minor.yy196, yymsp[-6].minor.yy0);
	yygotominor.yy100 = new func_decl_t(yymsp[-5].minor.yy160, ftype, yymsp[0].minor.yy220);
  yy_destructor(yypParser,50,&yymsp[-4].minor);
  yy_destructor(yypParser,51,&yymsp[-2].minor);
}
#line 1611 "parser.c"
        break;
      case 52: /* func_results ::= type */
#line 231 "parser.y"
{
	field_t *f = new field_t(0, yymsp[0].minor.yy100);
	yygotominor.yy196 = new field_vector_t(1, f);
}
#line 1619 "parser.c"
        break;
      case 53: /* func_results ::= LPAREN field_comma_list RPAREN */
#line 235 "parser.y"
{ yygotominor.yy196 = yymsp[-1].minor.yy196;   yy_destructor(yypParser,50,&yymsp[-2].minor);
  yy_destructor(yypParser,51,&yymsp[0].minor);
}
#line 1626 "parser.c"
        break;
      case 54: /* func_results ::= */
      case 144: /* ofield_comma_list ::= */ yytestcase(yyruleno==144);
      case 146: /* ofield_semi_list_and_osemi ::= */ yytestcase(yyruleno==146);
#line 236 "parser.y"
{ yygotominor.yy196 = 0; }
#line 1633 "parser.c"
        break;
      case 55: /* import_spec_list ::= import_spec */
#line 249 "parser.y"
{ yygotominor.yy41 = new import_spec_vector_t(1, yymsp[0].minor.yy203); }
#line 1638 "parser.c"
        break;
      case 56: /* import_spec_list ::= import_spec_list SEMICOLON import_spec */
#line 250 "parser.y"
{
	yymsp[-2].minor.yy41->push_back(yymsp[0].minor.yy203); yygotominor.yy41 = yymsp[-2].minor.yy41;
  yy_destructor(yypParser,35,&yymsp[-1].minor);
}
#line 1646 "parser.c"
        break;
      case 57: /* import_spec ::= IDENT STRING */
#line 255 "parser.y"
{ yygotominor.yy203 = new import_spec_t(yymsp[-1].minor.yy0, yymsp[0].minor.yy0); }
#line 1651 "parser.c"
        break;
      case 58: /* import_spec ::= STRING */
#line 256 "parser.y"
{ yygotominor.yy203 = new import_spec_t(0, yymsp[0].minor.yy0); }
#line 1656 "parser.c"
        break;
      case 59: /* type_spec_list ::= type_spec */
#line 269 "parser.y"
{ yygotominor.yy60 = new type_spec_vector_t(1, yymsp[0].minor.yy78); }
#line 1661 "parser.c"
        break;
      case 60: /* type_spec_list ::= type_spec_list SEMICOLON type_spec */
#line 270 "parser.y"
{
	yymsp[-2].minor.yy60->push_back(yymsp[0].minor.yy78); yygotominor.yy60 = yymsp[-2].minor.yy60;
  yy_destructor(yypParser,35,&yymsp[-1].minor);
}
#line 1669 "parser.c"
        break;
      case 61: /* type_spec ::= IDENT type */
#line 275 "parser.y"
{ yygotominor.yy78 = new type_spec_t(yymsp[-1].minor.yy0, yymsp[0].minor.yy100); }
#line 1674 "parser.c"
        break;
      case 62: /* value_spec_list ::= value_spec */
#line 290 "parser.y"
{ yygotominor.yy175 = new value_spec_vector_t(1, yymsp[0].minor.yy77); }
#line 1679 "parser.c"
        break;
      case 63: /* value_spec_list ::= value_spec_list SEMICOLON value_spec */
#line 291 "parser.y"
{
	yymsp[-2].minor.yy175->push_back(yymsp[0].minor.yy77); yygotominor.yy175 = yymsp[-2].minor.yy175;
  yy_destructor(yypParser,35,&yymsp[-1].minor);
}
#line 1687 "parser.c"
        break;
      case 64: /* value_spec ::= ident_list otype */
#line 296 "parser.y"
{
	yygotominor.yy77 = new value_spec_t(yymsp[-1].minor.yy134, yymsp[0].minor.yy100);
}
#line 1694 "parser.c"
        break;
      case 65: /* value_spec ::= ident_list otype ASSIGN iexpr_list */
#line 299 "parser.y"
{
	yygotominor.yy77 = new value_spec_t(yymsp[-3].minor.yy134, yymsp[-2].minor.yy100, yymsp[0].minor.yy70, yymsp[-1].minor.yy0);
}
#line 1701 "parser.c"
        break;
      case 66: /* type ::= TIMES type */
#line 320 "parser.y"
{ yygotominor.yy100 = new pointer_type_t(yymsp[0].minor.yy100, yymsp[-1].minor.yy0); }
#line 1706 "parser.c"
        break;
      case 67: /* type ::= LSB expr RSB type */
#line 321 "parser.y"
{ yygotominor.yy100 = new array_type_t(yymsp[-2].minor.yy100, yymsp[0].minor.yy100, yymsp[-3].minor.yy0, yymsp[-1].minor.yy0); }
#line 1711 "parser.c"
        break;
      case 68: /* type ::= LSB RSB type */
#line 322 "parser.y"
{ yygotominor.yy100 = new array_type_t(0, yymsp[0].minor.yy100, yymsp[-2].minor.yy0, yymsp[-1].minor.yy0); }
#line 1716 "parser.c"
        break;
      case 69: /* type ::= ident DOT ident */
#line 323 "parser.y"
{ yygotominor.yy100 = new selector_expr_t(yymsp[-2].minor.yy160, yymsp[0].minor.yy160);   yy_destructor(yypParser,60,&yymsp[-1].minor);
}
#line 1722 "parser.c"
        break;
      case 70: /* type ::= ident */
      case 89: /* pexpr ::= ident */ yytestcase(yyruleno==89);
#line 324 "parser.y"
{ yygotominor.yy100 = yymsp[0].minor.yy160; }
#line 1728 "parser.c"
        break;
      case 71: /* type ::= STRUCT LCURLY ofield_semi_list_and_osemi RCURLY */
#line 325 "parser.y"
{
	yygotominor.yy100 = new struct_type_t(yymsp[-1].minor.yy196, yymsp[-3].minor.yy0, yymsp[-2].minor.yy0, yymsp[0].minor.yy0);	
}
#line 1735 "parser.c"
        break;
      case 72: /* type ::= FUNC LPAREN ofield_comma_list RPAREN func_results */
#line 328 "parser.y"
{
	yygotominor.yy100 = new func_type_t(yymsp[-2].minor.yy196, yymsp[0].minor.yy196, yymsp[-4].minor.yy0);
  yy_destructor(yypParser,50,&yymsp[-3].minor);
  yy_destructor(yypParser,51,&yymsp[-1].minor);
}
#line 1744 "parser.c"
        break;
      case 73: /* field_comma_list ::= nametype_list ofunc_ellipsis */
#line 343 "parser.y"
{
	yygotominor.yy196 = nametypev_to_fieldv(ctx->diag, yymsp[-1].minor.yy119, yymsp[0].minor.yy97);
}
#line 1751 "parser.c"
        break;
      case 74: /* field_semi_list ::= field */
#line 348 "parser.y"
{ yygotominor.yy196 = new field_vector_t(1, yymsp[0].minor.yy195); }
#line 1756 "parser.c"
        break;
      case 75: /* field_semi_list ::= field_semi_list SEMICOLON field */
#line 349 "parser.y"
{
	yymsp[-2].minor.yy196->push_back(yymsp[0].minor.yy195); yygotominor.yy196 = yymsp[-2].minor.yy196;
  yy_destructor(yypParser,35,&yymsp[-1].minor);
}
#line 1764 "parser.c"
        break;
      case 76: /* nametype_list ::= nametype */
#line 355 "parser.y"
{ yygotominor.yy119 = new nametype_vector_t(1, yymsp[0].minor.yy43); }
#line 1769 "parser.c"
        break;
      case 77: /* nametype_list ::= nametype_list COMMA nametype */
#line 356 "parser.y"
{
	yymsp[-2].minor.yy119->push_back(yymsp[0].minor.yy43); yygotominor.yy119 = yymsp[-2].minor.yy119;
  yy_destructor(yypParser,62,&yymsp[-1].minor);
}
#line 1777 "parser.c"
        break;
      case 78: /* nametype ::= ident type */
#line 361 "parser.y"
{ yygotominor.yy43.name = yymsp[-1].minor.yy160; yygotominor.yy43.type = yymsp[0].minor.yy100; }
#line 1782 "parser.c"
        break;
      case 79: /* nametype ::= type */
#line 362 "parser.y"
{ yygotominor.yy43.name = 0; yygotominor.yy43.type = yymsp[0].minor.yy100; }
#line 1787 "parser.c"
        break;
      case 80: /* ofunc_ellipsis ::= COMMA ELLIPSIS */
#line 372 "parser.y"
{ yygotominor.yy97 = true;   yy_destructor(yypParser,62,&yymsp[-1].minor);
  yy_destructor(yypParser,63,&yymsp[0].minor);
}
#line 1794 "parser.c"
        break;
      case 81: /* ofunc_ellipsis ::= */
#line 373 "parser.y"
{ yygotominor.yy97 = false; }
#line 1799 "parser.c"
        break;
      case 82: /* field ::= ident_list type */
#line 386 "parser.y"
{ yygotominor.yy195 = new field_t(yymsp[-1].minor.yy134, yymsp[0].minor.yy100); }
#line 1804 "parser.c"
        break;
      case 83: /* field ::= type */
#line 387 "parser.y"
{ yygotominor.yy195 = new field_t(0, yymsp[0].minor.yy100); }
#line 1809 "parser.c"
        break;
      case 84: /* basic_lit ::= INT */
      case 85: /* basic_lit ::= FLOAT */ yytestcase(yyruleno==85);
      case 86: /* basic_lit ::= STRING */ yytestcase(yyruleno==86);
      case 87: /* basic_lit ::= CHAR */ yytestcase(yyruleno==87);
#line 398 "parser.y"
{ yygotominor.yy46 = new basic_lit_expr_t(yymsp[0].minor.yy0); }
#line 1817 "parser.c"
        break;
      case 88: /* pexpr ::= basic_lit */
#line 414 "parser.y"
{ yygotominor.yy100 = yymsp[0].minor.yy46; }
#line 1822 "parser.c"
        break;
      case 90: /* pexpr ::= LPAREN expr RPAREN */
#line 416 "parser.y"
{ yygotominor.yy100 = new paren_expr_t(yymsp[-1].minor.yy100, yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1827 "parser.c"
        break;
      case 91: /* pexpr ::= pexpr LSB expr RSB */
#line 417 "parser.y"
{ yygotominor.yy100 = new index_expr_t(yymsp[-3].minor.yy100, yymsp[-1].minor.yy100, yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1832 "parser.c"
        break;
      case 92: /* pexpr ::= pexpr DOT IDENT */
#line 420 "parser.y"
{
	yygotominor.yy100 = new selector_expr_t(yymsp[-2].minor.yy100, new ident_expr_t(yymsp[0].minor.yy0));
  yy_destructor(yypParser,60,&yymsp[-1].minor);
}
#line 1840 "parser.c"
        break;
      case 93: /* pexpr ::= pexpr DOT LPAREN type RPAREN */
#line 425 "parser.y"
{
	yygotominor.yy100 = new type_cast_expr_t(yymsp[-4].minor.yy100, yymsp[-1].minor.yy100, yymsp[-3].minor.yy0, yymsp[-2].minor.yy0, yymsp[0].minor.yy0);
}
#line 1847 "parser.c"
        break;
      case 94: /* pexpr ::= pexpr LPAREN RPAREN */
#line 431 "parser.y"
{ yygotominor.yy100 = new call_expr_t(yymsp[-2].minor.yy100, 0, yymsp[-1].minor.yy0, yymsp[0].minor.yy0); }
#line 1852 "parser.c"
        break;
      case 95: /* pexpr ::= pexpr LPAREN iexpr_list RPAREN */
#line 434 "parser.y"
{
	yygotominor.yy100 = new call_expr_t(yymsp[-3].minor.yy100, yymsp[-1].minor.yy70, yymsp[-2].minor.yy0, yymsp[0].minor.yy0);
}
#line 1859 "parser.c"
        break;
      case 96: /* uexpr ::= pexpr */
      case 103: /* expr ::= uexpr */ yytestcase(yyruleno==103);
      case 126: /* iexpr ::= expr */ yytestcase(yyruleno==126);
      case 127: /* iexpr ::= compound_lit */ yytestcase(yyruleno==127);
      case 128: /* iexpr ::= type_expr */ yytestcase(yyruleno==128);
      case 136: /* oexpr ::= expr */ yytestcase(yyruleno==136);
      case 140: /* otype ::= type */ yytestcase(yyruleno==140);
#line 448 "parser.y"
{ yygotominor.yy100 = yymsp[0].minor.yy100; }
#line 1870 "parser.c"
        break;
      case 97: /* uexpr ::= PLUS uexpr */
      case 98: /* uexpr ::= MINUS uexpr */ yytestcase(yyruleno==98);
      case 99: /* uexpr ::= NOT uexpr */ yytestcase(yyruleno==99);
      case 100: /* uexpr ::= AND uexpr */ yytestcase(yyruleno==100);
      case 101: /* uexpr ::= TIMES uexpr */ yytestcase(yyruleno==101);
      case 102: /* uexpr ::= XOR uexpr */ yytestcase(yyruleno==102);
#line 449 "parser.y"
{ yygotominor.yy100 = new unary_expr_t(yymsp[0].minor.yy100, yymsp[-1].minor.yy0); }
#line 1880 "parser.c"
        break;
      case 104: /* expr ::= expr DIVIDE expr */
      case 105: /* expr ::= expr TIMES expr */ yytestcase(yyruleno==105);
      case 106: /* expr ::= expr MOD expr */ yytestcase(yyruleno==106);
      case 107: /* expr ::= expr SHIFTL expr */ yytestcase(yyruleno==107);
      case 108: /* expr ::= expr SHIFTR expr */ yytestcase(yyruleno==108);
      case 109: /* expr ::= expr AND expr */ yytestcase(yyruleno==109);
      case 110: /* expr ::= expr ANDNOT expr */ yytestcase(yyruleno==110);
      case 111: /* expr ::= expr PLUS expr */ yytestcase(yyruleno==111);
      case 112: /* expr ::= expr MINUS expr */ yytestcase(yyruleno==112);
      case 113: /* expr ::= expr OR expr */ yytestcase(yyruleno==113);
      case 114: /* expr ::= expr XOR expr */ yytestcase(yyruleno==114);
      case 115: /* expr ::= expr EQ expr */ yytestcase(yyruleno==115);
      case 116: /* expr ::= expr NEQ expr */ yytestcase(yyruleno==116);
      case 117: /* expr ::= expr LT expr */ yytestcase(yyruleno==117);
      case 118: /* expr ::= expr LE expr */ yytestcase(yyruleno==118);
      case 119: /* expr ::= expr GT expr */ yytestcase(yyruleno==119);
      case 120: /* expr ::= expr GE expr */ yytestcase(yyruleno==120);
      case 121: /* expr ::= expr ANDAND expr */ yytestcase(yyruleno==121);
      case 122: /* expr ::= expr OROR expr */ yytestcase(yyruleno==122);
#line 467 "parser.y"
{ yygotominor.yy100 = new binary_expr_t(yymsp[-2].minor.yy100, yymsp[0].minor.yy100, yymsp[-1].minor.yy0); }
#line 1903 "parser.c"
        break;
      case 123: /* compound_lit ::= LCURLY iexpr_list RCURLY */
#line 498 "parser.y"
{
	yygotominor.yy100 = new compound_lit_t(yymsp[-1].minor.yy70, 0, yymsp[-2].minor.yy0, yymsp[0].minor.yy0);
}
#line 1910 "parser.c"
        break;
      case 124: /* compound_lit ::= LCURLY iexpr_list RCURLY DOT LPAREN type RPAREN */
#line 501 "parser.y"
{
	yygotominor.yy100 = new compound_lit_t(yymsp[-5].minor.yy70, yymsp[-1].minor.yy100, yymsp[-6].minor.yy0, yymsp[0].minor.yy0);
  yy_destructor(yypParser,45,&yymsp[-4].minor);
  yy_destructor(yypParser,60,&yymsp[-3].minor);
  yy_destructor(yypParser,50,&yymsp[-2].minor);
}
#line 1920 "parser.c"
        break;
      case 125: /* type_expr ::= TYPE type */
#line 515 "parser.y"
{ yygotominor.yy100 = new type_expr_t(yymsp[0].minor.yy100, yymsp[-1].minor.yy0); }
#line 1925 "parser.c"
        break;
      case 129: /* iexpr_list ::= iexpr */
      case 131: /* expr_list ::= expr */ yytestcase(yyruleno==131);
#line 531 "parser.y"
{ yygotominor.yy70 = new node_vector_t(1, yymsp[0].minor.yy100); }
#line 1931 "parser.c"
        break;
      case 130: /* iexpr_list ::= iexpr_list COMMA iexpr */
      case 132: /* expr_list ::= expr_list COMMA expr */ yytestcase(yyruleno==132);
#line 532 "parser.y"
{ yymsp[-2].minor.yy70->push_back(yymsp[0].minor.yy100); yygotominor.yy70 = yymsp[-2].minor.yy70;   yy_destructor(yypParser,62,&yymsp[-1].minor);
}
#line 1938 "parser.c"
        break;
      case 134: /* osemi ::= SEMICOLON */
#line 547 "parser.y"
{
  yy_destructor(yypParser,35,&yymsp[0].minor);
}
#line 1945 "parser.c"
        break;
      case 137: /* ostmt_list ::= */
#line 556 "parser.y"
{ yygotominor.yy70 = 0; }
#line 1950 "parser.c"
        break;
      case 138: /* ostmt_list ::= stmt_list */
#line 557 "parser.y"
{ yygotominor.yy70 = yymsp[0].minor.yy70; }
#line 1955 "parser.c"
        break;
      case 141: /* ident ::= IDENT */
#line 566 "parser.y"
{ yygotominor.yy160 = new ident_expr_t(yymsp[0].minor.yy0); }
#line 1960 "parser.c"
        break;
      case 142: /* ident_list ::= ident */
#line 570 "parser.y"
{ yygotominor.yy134 = new ident_expr_vector_t(1, yymsp[0].minor.yy160); }
#line 1965 "parser.c"
        break;
      case 143: /* ident_list ::= ident_list COMMA ident */
#line 571 "parser.y"
{ yymsp[-2].minor.yy134->push_back(yymsp[0].minor.yy160); yygotominor.yy134 = yymsp[-2].minor.yy134;   yy_destructor(yypParser,62,&yymsp[-1].minor);
}
#line 1971 "parser.c"
        break;
      case 145: /* ofield_comma_list ::= field_comma_list */
#line 576 "parser.y"
{ yygotominor.yy196 = yymsp[0].minor.yy196; }
#line 1976 "parser.c"
        break;
      case 147: /* ofield_semi_list_and_osemi ::= field_semi_list osemi */
#line 581 "parser.y"
{ yygotominor.yy196 = yymsp[-1].minor.yy196; }
#line 1981 "parser.c"
        break;
      default:
      /* (133) osemi ::= */ yytestcase(yyruleno==133);
        break;
  };
  yygoto = yyRuleInfo[yyruleno].lhs;
  yysize = yyRuleInfo[yyruleno].nrhs;
  yypParser->yyidx -= yysize;
  yyact = yy_find_reduce_action(yymsp[-yysize].stateno,(YYCODETYPE)yygoto);
  if( yyact < YYNSTATE ){
#ifdef NDEBUG
    /* If we are not debugging and the reduce action popped at least
    ** one element off the stack, then we can push the new element back
    ** onto the stack here, and skip the stack overflow test in yy_shift().
    ** That gives a significant speed improvement. */
    if( yysize ){
      yypParser->yyidx++;
      yymsp -= yysize-1;
      yymsp->stateno = (YYACTIONTYPE)yyact;
      yymsp->major = (YYCODETYPE)yygoto;
      yymsp->minor = yygotominor;
    }else
#endif
    {
      yy_shift(yypParser,yyact,yygoto,&yygotominor);
    }
  }else{
    assert( yyact == YYNSTATE + YYNRULE + 1 );
    yy_accept(yypParser);
  }
}

/*
** The following code executes when the parse fails
*/
#ifndef YYNOERRORRECOVERY
static void yy_parse_failed(
  yyParser *yypParser           /* The parser */
){
  ParseARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sFail!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser fails */
  ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}
#endif /* YYNOERRORRECOVERY */

/*
** The following code executes when a syntax error first occurs.
*/
static void yy_syntax_error(
  yyParser *yypParser,           /* The parser */
  int yymajor,                   /* The major type of the error token */
  YYMINORTYPE yyminor            /* The minor type of the error token */
){
  ParseARG_FETCH;
#define TOKEN (yyminor.yy0)
#line 21 "parser.y"

	message_t *m;
	m = new_message(MESSAGE_ERROR,
			ctx->last_pos, true, 0, 0,
			"unexpected token: %s",
			token_to_string(ctx->last_token));
	ctx->diag->report(m);
#line 2052 "parser.c"
  ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following is executed when the parser accepts
*/
static void yy_accept(
  yyParser *yypParser           /* The parser */
){
  ParseARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sAccept!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser accepts */
  ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/* The main parser program.
** The first argument is a pointer to a structure obtained from
** "ParseAlloc" which describes the current state of the parser.
** The second argument is the major token number.  The third is
** the minor token.  The fourth optional argument is whatever the
** user wants (and specified in the grammar) and is available for
** use by the action routines.
**
** Inputs:
** <ul>
** <li> A pointer to the parser (an opaque structure.)
** <li> The major token number.
** <li> The minor token number.
** <li> An option argument of a grammar-specified type.
** </ul>
**
** Outputs:
** None.
*/
void Parse(
  void *yyp,                   /* The parser */
  int yymajor,                 /* The major token code number */
  ParseTOKENTYPE yyminor       /* The value for the token */
  ParseARG_PDECL               /* Optional %extra_argument parameter */
){
  YYMINORTYPE yyminorunion;
  int yyact;            /* The parser action. */
  int yyendofinput;     /* True if we are at the end of input */
#ifdef YYERRORSYMBOL
  int yyerrorhit = 0;   /* True if yymajor has invoked an error */
#endif
  yyParser *yypParser;  /* The parser */

  /* (re)initialize the parser, if necessary */
  yypParser = (yyParser*)yyp;
  if( yypParser->yyidx<0 ){
#if YYSTACKDEPTH<=0
    if( yypParser->yystksz <=0 ){
      /*memset(&yyminorunion, 0, sizeof(yyminorunion));*/
      yyminorunion = yyzerominor;
      yyStackOverflow(yypParser, &yyminorunion);
      return;
    }
#endif
    yypParser->yyidx = 0;
    yypParser->yyerrcnt = -1;
    yypParser->yystack[0].stateno = 0;
    yypParser->yystack[0].major = 0;
  }
  yyminorunion.yy0 = yyminor;
  yyendofinput = (yymajor==0);
  ParseARG_STORE;

#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sInput %s\n",yyTracePrompt,yyTokenName[yymajor]);
  }
#endif

  do{
    yyact = yy_find_shift_action(yypParser,(YYCODETYPE)yymajor);
    if( yyact<YYNSTATE ){
      assert( !yyendofinput );  /* Impossible to shift the $ token */
      yy_shift(yypParser,yyact,yymajor,&yyminorunion);
      yypParser->yyerrcnt--;
      yymajor = YYNOCODE;
    }else if( yyact < YYNSTATE + YYNRULE ){
      yy_reduce(yypParser,yyact-YYNSTATE);
    }else{
      assert( yyact == YY_ERROR_ACTION );
#ifdef YYERRORSYMBOL
      int yymx;
#endif
#ifndef NDEBUG
      if( yyTraceFILE ){
        fprintf(yyTraceFILE,"%sSyntax Error!\n",yyTracePrompt);
      }
#endif
#ifdef YYERRORSYMBOL
      /* A syntax error has occurred.
      ** The response to an error depends upon whether or not the
      ** grammar defines an error token "ERROR".  
      **
      ** This is what we do if the grammar does define ERROR:
      **
      **  * Call the %syntax_error function.
      **
      **  * Begin popping the stack until we enter a state where
      **    it is legal to shift the error symbol, then shift
      **    the error symbol.
      **
      **  * Set the error count to three.
      **
      **  * Begin accepting and shifting new tokens.  No new error
      **    processing will occur until three tokens have been
      **    shifted successfully.
      **
      */
      if( yypParser->yyerrcnt<0 ){
        yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yymx = yypParser->yystack[yypParser->yyidx].major;
      if( yymx==YYERRORSYMBOL || yyerrorhit ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE,"%sDiscard input token %s\n",
             yyTracePrompt,yyTokenName[yymajor]);
        }
#endif
        yy_destructor(yypParser, (YYCODETYPE)yymajor,&yyminorunion);
        yymajor = YYNOCODE;
      }else{
         while(
          yypParser->yyidx >= 0 &&
          yymx != YYERRORSYMBOL &&
          (yyact = yy_find_reduce_action(
                        yypParser->yystack[yypParser->yyidx].stateno,
                        YYERRORSYMBOL)) >= YYNSTATE
        ){
          yy_pop_parser_stack(yypParser);
        }
        if( yypParser->yyidx < 0 || yymajor==0 ){
          yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
          yy_parse_failed(yypParser);
          yymajor = YYNOCODE;
        }else if( yymx!=YYERRORSYMBOL ){
          YYMINORTYPE u2;
          u2.YYERRSYMDT = 0;
          yy_shift(yypParser,yyact,YYERRORSYMBOL,&u2);
        }
      }
      yypParser->yyerrcnt = 3;
      yyerrorhit = 1;
#elif defined(YYNOERRORRECOVERY)
      /* If the YYNOERRORRECOVERY macro is defined, then do not attempt to
      ** do any kind of error recovery.  Instead, simply invoke the syntax
      ** error routine and continue going as if nothing had happened.
      **
      ** Applications can set this macro (for example inside %include) if
      ** they intend to abandon the parse upon the first syntax error seen.
      */
      yy_syntax_error(yypParser,yymajor,yyminorunion);
      yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      yymajor = YYNOCODE;
      
#else  /* YYERRORSYMBOL is not defined */
      /* This is what we do if the grammar does not define ERROR:
      **
      **  * Report an error message, and throw away the input token.
      **
      **  * If the input token is $, then fail the parse.
      **
      ** As before, subsequent error messages are suppressed until
      ** three input tokens have been successfully shifted.
      */
      if( yypParser->yyerrcnt<=0 ){
        yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yypParser->yyerrcnt = 3;
      yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      if( yyendofinput ){
        yy_parse_failed(yypParser);
      }
      yymajor = YYNOCODE;
#endif
    }
  }while( yymajor!=YYNOCODE && yypParser->yyidx>=0 );
  return;
}
