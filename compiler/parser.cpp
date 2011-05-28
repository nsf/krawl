/* Driver template for the LEMON parser generator.
** The author disclaims copyright to this source code.
*/
/* First off, code is included that follows the "include" declaration
** in the input grammar file. */
#include <stdio.h>
#line 10 "parser.y"

	#include "crawl.hpp"
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
#define YYNOCODE 110
#define YYACTIONTYPE unsigned short int
#define ParseTOKENTYPE  token_t* 
typedef union {
  int yyinit;
  ParseTOKENTYPE yy0;
  block_stmt_t* yy2;
  node_vector_t* yy10;
  ident_expr_vector_t* yy22;
  bool yy25;
  field_vector_t* yy56;
  import_spec_t* yy57;
  node_t* yy60;
  basic_lit_expr_t* yy61;
  nametype_t yy63;
  ident_expr_t* yy72;
  type_spec_t* yy78;
  program_t* yy96;
  value_spec_vector_t* yy107;
  nametype_vector_t* yy108;
  switch_stmt_case_t* yy114;
  field_t* yy150;
  import_spec_vector_t* yy155;
  value_spec_t* yy156;
  type_spec_vector_t* yy168;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define ParseARG_SDECL  parser_t *ctx ;
#define ParseARG_PDECL , parser_t *ctx 
#define ParseARG_FETCH  parser_t *ctx  = yypParser->ctx 
#define ParseARG_STORE yypParser->ctx  = ctx 
#define YYNSTATE 273
#define YYNRULE 146
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
 /*     0 */    57,   58,  189,   62,  138,   61,  199,  192,  105,   60,
 /*    10 */    10,   11,   12,   13,   14,   15,   16,   17,   18,   19,
 /*    20 */    20,   21,   22,  138,  212,   92,  192,  254,  255,  256,
 /*    30 */     7,   54,   76,   23,   55,    1,  408,  211,  126,  221,
 /*    40 */    88,   33,  132,   91,   70,   71,  102,  189,  209,  175,
 /*    50 */   264,   27,   53,  173,  260,  207,  208,  210,   59,   74,
 /*    60 */   142,   74,  196,  200,  196,   57,   58,   97,   62,  138,
 /*    70 */    61,  198,  191,  195,   60,   41,   42,   43,   44,   32,
 /*    80 */    34,   36,   37,   38,   39,   40,  108,  170,  128,  420,
 /*    90 */    73,  276,  254,  255,  256,    7,   54,   64,   23,   55,
 /*   100 */     1,  138,  212,  189,  242,   88,   33,  150,   91,   70,
 /*   110 */    71,  102,  189,  209,  212,  211,  126,  221,  232,  233,
 /*   120 */   207,  208,  210,   59,  272,  263,    6,  211,  126,  215,
 /*   130 */    57,   58,  212,   62,   35,   61,  127,  138,   88,   60,
 /*   140 */   203,   91,   70,   71,  102,  211,  126,  216,  248,  169,
 /*   150 */   104,  114,  138,  252,  253,  185,   65,  254,  255,  256,
 /*   160 */     7,   54,  189,   23,   55,    1,  212,  135,  180,  212,
 /*   170 */    88,   33,  133,   91,   70,   71,  102,  189,  209,  211,
 /*   180 */   126,  221,  211,  126,  217,  207,  208,  210,   59,   52,
 /*   190 */    51,   45,   46,   47,   48,   49,   50,   41,   42,   43,
 /*   200 */    44,   32,   34,   36,   37,   38,   39,   40,   32,   34,
 /*   210 */    36,   37,   38,   39,   40,   57,   58,   77,   62,  136,
 /*   220 */    61,  249,  250,  277,   60,  138,  138,  138,  186,  187,
 /*   230 */   144,   27,    1,  173,   52,   51,   45,   46,   47,   48,
 /*   240 */    49,   50,   41,   42,   43,   44,   32,   34,   36,   37,
 /*   250 */    38,   39,   40,   56,  138,  265,   33,  147,  141,  189,
 /*   260 */   270,   31,  189,  209,  139,   81,  249,  250,  277,    1,
 /*   270 */   207,  208,  210,   59,  178,  138,   77,  277,  237,   52,
 /*   280 */    51,   45,   46,   47,   48,   49,   50,   41,   42,   43,
 /*   290 */    44,   32,   34,   36,   37,   38,   39,   40,   52,   51,
 /*   300 */    45,   46,   47,   48,   49,   50,   41,   42,   43,   44,
 /*   310 */    32,   34,   36,   37,   38,   39,   40,  141,  189,  271,
 /*   320 */    31,   90,    1,  139,  134,  197,  125,  125,   95,  205,
 /*   330 */   205,   75,   75,  140,  181,   93,   98,  201,  202,   89,
 /*   340 */   268,   83,  137,   52,   51,   45,   46,   47,   48,   49,
 /*   350 */    50,   41,   42,   43,   44,   32,   34,   36,   37,   38,
 /*   360 */    39,   40,   52,   51,   45,   46,   47,   48,   49,   50,
 /*   370 */    41,   42,   43,   44,   32,   34,   36,   37,   38,   39,
 /*   380 */    40,   52,   51,   45,   46,   47,   48,   49,   50,   41,
 /*   390 */    42,   43,   44,   32,   34,   36,   37,   38,   39,   40,
 /*   400 */   188,   78,  244,  100,  243,  184,  244,  101,  243,   68,
 /*   410 */   238,   94,  213,   68,   52,   51,   45,   46,   47,   48,
 /*   420 */    49,   50,   41,   42,   43,   44,   32,   34,   36,   37,
 /*   430 */    38,   39,   40,  148,  152,  243,  243,   67,  194,  214,
 /*   440 */    68,   68,   51,   45,   46,   47,   48,   49,   50,   41,
 /*   450 */    42,   43,   44,   32,   34,   36,   37,   38,   39,   40,
 /*   460 */    45,   46,   47,   48,   49,   50,   41,   42,   43,   44,
 /*   470 */    32,   34,   36,   37,   38,   39,   40,   57,   58,  343,
 /*   480 */    62,  241,   61,  243,  230,  206,   60,   63,   68,  343,
 /*   490 */    74,  155,  204,  196,  200,   24,   57,   58,   97,   62,
 /*   500 */    82,   61,  198,   84,   96,   60,  135,  180,   79,   76,
 /*   510 */   145,  257,    8,   72,  227,  154,  149,  153,   33,  229,
 /*   520 */   261,    2,  258,    1,  189,  209,  108,   24,  130,  273,
 /*   530 */   179,    8,  207,  208,  210,   59,   53,   33,   24,  176,
 /*   540 */   177,  182,  212,  189,  209,  183,   96,   28,   29,   80,
 /*   550 */   190,  207,  208,  210,   59,  211,  126,  221,  232,  233,
 /*   560 */    66,   57,   58,  193,   62,  228,   61,  146,  231,  239,
 /*   570 */    60,  240,    4,  247,  169,  104,  114,    9,  252,  253,
 /*   580 */    57,   58,  245,   62,  174,   61,  212,   24,  246,   60,
 /*   590 */    30,  212,  259,  169,  104,  114,    8,  252,  253,  211,
 /*   600 */   126,  218,   33,  251,  211,  126,  221,   69,  189,  209,
 /*   610 */   212,    5,   26,   25,  269,    1,  207,  208,  210,   59,
 /*   620 */     3,   33,  421,  211,  126,  221,  421,  189,  209,  421,
 /*   630 */   421,  421,  421,  421,  212,  207,  208,  210,   59,    4,
 /*   640 */   247,  169,  104,  114,  421,  252,  253,  211,  126,  219,
 /*   650 */   421,  266,   57,   58,  108,   62,  129,   61,  212,  108,
 /*   660 */   421,   60,    4,  247,  169,  104,  114,  421,  252,  253,
 /*   670 */   212,  211,  126,  221,  267,  212,  421,  421,  421,  421,
 /*   680 */   421,  212,  421,  211,  126,  221,  232,  233,  211,  126,
 /*   690 */   221,  232,  234,   33,  211,  126,  221,  421,  421,  189,
 /*   700 */   209,  108,  421,  151,  421,  421,  421,  207,  208,  210,
 /*   710 */    59,  108,  421,  156,  421,  421,  421,  212,  421,  108,
 /*   720 */   421,  157,  421,  421,  421,  421,  421,  212,  421,  421,
 /*   730 */   211,  126,  221,  232,  233,  212,  421,  108,  421,  158,
 /*   740 */   211,  126,  221,  232,  233,  108,  421,  159,  211,  126,
 /*   750 */   221,  232,  233,  212,  421,  421,  421,  108,  421,  160,
 /*   760 */    77,  212,  421,  108,  421,  161,  211,  126,  221,  232,
 /*   770 */   233,  421,  421,  212,  211,  126,  221,  232,  233,  212,
 /*   780 */   421,  421,  421,  108,  421,  162,  211,  126,  221,  232,
 /*   790 */   233,  421,  211,  126,  221,  232,  233,  421,  421,  212,
 /*   800 */   421,  141,  189,  108,   31,  163,   96,  139,  108,  421,
 /*   810 */   164,  421,  211,  126,  221,  232,  233,  421,  421,  212,
 /*   820 */    77,  421,  421,  421,  212,  421,  108,  421,  165,  421,
 /*   830 */   421,  421,  211,  126,  221,  232,  233,  211,  126,  221,
 /*   840 */   232,  233,  212,  421,  108,  421,  166,  421,  421,  108,
 /*   850 */   421,  167,  421,  421,  421,  211,  126,  221,  232,  233,
 /*   860 */   212,  141,  189,  421,   31,  212,  421,  139,  108,  421,
 /*   870 */   168,  421,  421,  211,  126,  221,  232,  233,  211,  126,
 /*   880 */   221,  232,  233,  421,  212,  171,   85,  114,   77,  110,
 /*   890 */   262,  103,  104,  114,  172,  111,  131,  211,  126,  221,
 /*   900 */   232,  233,  212,  421,  421,  212,  421,  421,  212,  421,
 /*   910 */   421,  212,  236,  421,  421,  211,  126,  221,  211,  126,
 /*   920 */   221,  211,  126,  221,  211,  126,  221,  421,  212,  141,
 /*   930 */   189,  106,   31,  421,  235,  139,   99,  107,  421,  421,
 /*   940 */   222,  211,  126,  221,  421,  421,  421,  212,  421,  421,
 /*   950 */   212,  421,  421,  212,  421,  421,  212,  223,  421,  421,
 /*   960 */   211,  126,  221,  211,  126,  221,  211,  126,  221,  211,
 /*   970 */   126,  221,  421,  212,  224,  421,  421,  225,  421,  421,
 /*   980 */   421,  226,  421,  421,  121,  421,  211,  126,  221,  421,
 /*   990 */   212,  421,  421,  212,  421,  421,  421,  212,  421,  421,
 /*  1000 */   212,  122,  421,  211,  126,  221,  211,  126,  221,  421,
 /*  1010 */   211,  126,  221,  211,  126,  221,   74,  212,  123,  196,
 /*  1020 */   143,  421,  212,  124,   97,  421,  115,  421,  198,  116,
 /*  1030 */   211,  126,  221,  421,  212,  211,  126,  220,  421,  212,
 /*  1040 */   421,  421,  212,  421,  421,  212,  117,  211,  126,  221,
 /*  1050 */   118,  421,  211,  126,  221,  211,  126,  221,  211,  126,
 /*  1060 */   221,  421,  212,  421,  421,  421,  212,  421,  421,  119,
 /*  1070 */   421,  421,  421,  120,  421,  211,  126,  221,  421,  211,
 /*  1080 */   126,  221,  421,  421,  421,  212,  421,  421,  421,  212,
 /*  1090 */   113,  421,  421,  112,  421,  421,  421,  421,  211,  126,
 /*  1100 */   221,  421,  211,  126,  221,  421,  212,  421,  421,  212,
 /*  1110 */   109,  421,  421,  421,  421,   86,  421,  421,   87,  211,
 /*  1120 */   126,  221,  211,  126,  221,  421,  212,  421,  421,  421,
 /*  1130 */   421,  212,  421,  421,  212,  421,  421,  421,  421,  211,
 /*  1140 */   126,  221,  421,  421,  211,  126,  221,  211,  126,  221,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */     9,   10,   56,   12,   91,   14,   93,   94,   75,   18,
 /*    10 */    20,   21,   22,   23,   24,   25,   26,   27,   28,   29,
 /*    20 */    30,   31,   32,   91,   91,   93,   94,   36,   37,   38,
 /*    30 */    39,   40,   56,   42,   43,   44,   45,  104,  105,  106,
 /*    40 */    49,   50,   71,   52,   53,   54,   55,   56,   57,   78,
 /*    50 */    45,   46,   62,   48,   79,   64,   65,   66,   67,   91,
 /*    60 */    92,   91,   94,   95,   94,    9,   10,   99,   12,   91,
 /*    70 */    14,  103,   94,  103,   18,    9,   10,   11,   12,   13,
 /*    80 */    14,   15,   16,   17,   18,   19,   75,   79,   77,   69,
 /*    90 */    70,   35,   36,   37,   38,   39,   40,   50,   42,   43,
 /*   100 */    44,   91,   91,   56,   94,   49,   50,   97,   52,   53,
 /*   110 */    54,   55,   56,   57,   91,  104,  105,  106,  107,  108,
 /*   120 */    64,   65,   66,   67,   35,   81,   50,  104,  105,  106,
 /*   130 */     9,   10,   91,   12,   58,   14,   60,   91,   49,   18,
 /*   140 */    94,   52,   53,   54,   55,  104,  105,  106,   73,   74,
 /*   150 */    75,   76,   91,   78,   79,   94,   50,   36,   37,   38,
 /*   160 */    39,   40,   56,   42,   43,   44,   91,   56,   57,   91,
 /*   170 */    49,   50,   85,   52,   53,   54,   55,   56,   57,  104,
 /*   180 */   105,  106,  104,  105,  106,   64,   65,   66,   67,    1,
 /*   190 */     2,    3,    4,    5,    6,    7,    8,    9,   10,   11,
 /*   200 */    12,   13,   14,   15,   16,   17,   18,   19,   13,   14,
 /*   210 */    15,   16,   17,   18,   19,    9,   10,   14,   12,   87,
 /*   220 */    14,   33,   34,   35,   18,   91,   91,   91,   94,   94,
 /*   230 */    94,   46,   44,   48,    1,    2,    3,    4,    5,    6,
 /*   240 */     7,    8,    9,   10,   11,   12,   13,   14,   15,   16,
 /*   250 */    17,   18,   19,   50,   91,   83,   50,   94,   55,   56,
 /*   260 */    35,   58,   56,   57,   61,   59,   33,   34,   35,   44,
 /*   270 */    64,   65,   66,   67,   85,   91,   14,   44,   94,    1,
 /*   280 */     2,    3,    4,    5,    6,    7,    8,    9,   10,   11,
 /*   290 */    12,   13,   14,   15,   16,   17,   18,   19,    1,    2,
 /*   300 */     3,    4,    5,    6,    7,    8,    9,   10,   11,   12,
 /*   310 */    13,   14,   15,   16,   17,   18,   19,   55,   56,   79,
 /*   320 */    58,   35,   44,   61,   71,   63,   91,   91,   35,   94,
 /*   330 */    94,   96,   96,   98,   85,   86,  101,  102,  102,   82,
 /*   340 */    83,   44,   71,    1,    2,    3,    4,    5,    6,    7,
 /*   350 */     8,    9,   10,   11,   12,   13,   14,   15,   16,   17,
 /*   360 */    18,   19,    1,    2,    3,    4,    5,    6,    7,    8,
 /*   370 */     9,   10,   11,   12,   13,   14,   15,   16,   17,   18,
 /*   380 */    19,    1,    2,    3,    4,    5,    6,    7,    8,    9,
 /*   390 */    10,   11,   12,   13,   14,   15,   16,   17,   18,   19,
 /*   400 */    91,   59,   89,   90,   91,   87,   89,   90,   91,   96,
 /*   410 */    87,   88,   51,   96,    1,    2,    3,    4,    5,    6,
 /*   420 */     7,    8,    9,   10,   11,   12,   13,   14,   15,   16,
 /*   430 */    17,   18,   19,   89,   89,   91,   91,   62,  100,   59,
 /*   440 */    96,   96,    2,    3,    4,    5,    6,    7,    8,    9,
 /*   450 */    10,   11,   12,   13,   14,   15,   16,   17,   18,   19,
 /*   460 */     3,    4,    5,    6,    7,    8,    9,   10,   11,   12,
 /*   470 */    13,   14,   15,   16,   17,   18,   19,    9,   10,   35,
 /*   480 */    12,   89,   14,   91,   51,   71,   18,   35,   96,   45,
 /*   490 */    91,   92,   91,   94,   95,   62,    9,   10,   99,   12,
 /*   500 */    50,   14,  103,   50,   60,   18,   56,   57,   50,   56,
 /*   510 */    45,   35,   44,   35,   56,   91,   71,   71,   50,   51,
 /*   520 */    79,   47,   35,   44,   56,   57,   75,   62,   77,    0,
 /*   530 */    57,   44,   64,   65,   66,   67,   62,   50,   62,   35,
 /*   540 */    51,   35,   91,   56,   57,   51,   60,   44,   50,   50,
 /*   550 */    45,   64,   65,   66,   67,  104,  105,  106,  107,  108,
 /*   560 */    51,    9,   10,   51,   12,   51,   14,   60,   51,   35,
 /*   570 */    18,   51,   72,   73,   74,   75,   76,   20,   78,   79,
 /*   580 */     9,   10,   35,   12,   84,   14,   91,   62,   51,   18,
 /*   590 */    50,   91,   73,   74,   75,   76,   44,   78,   79,  104,
 /*   600 */   105,  106,   50,   35,  104,  105,  106,   51,   56,   57,
 /*   610 */    91,   41,   35,   35,   45,   44,   64,   65,   66,   67,
 /*   620 */    47,   50,  109,  104,  105,  106,  109,   56,   57,  109,
 /*   630 */   109,  109,  109,  109,   91,   64,   65,   66,   67,   72,
 /*   640 */    73,   74,   75,   76,  109,   78,   79,  104,  105,  106,
 /*   650 */   109,   84,    9,   10,   75,   12,   77,   14,   91,   75,
 /*   660 */   109,   18,   72,   73,   74,   75,   76,  109,   78,   79,
 /*   670 */    91,  104,  105,  106,   84,   91,  109,  109,  109,  109,
 /*   680 */   109,   91,  109,  104,  105,  106,  107,  108,  104,  105,
 /*   690 */   106,  107,  108,   50,  104,  105,  106,  109,  109,   56,
 /*   700 */    57,   75,  109,   77,  109,  109,  109,   64,   65,   66,
 /*   710 */    67,   75,  109,   77,  109,  109,  109,   91,  109,   75,
 /*   720 */   109,   77,  109,  109,  109,  109,  109,   91,  109,  109,
 /*   730 */   104,  105,  106,  107,  108,   91,  109,   75,  109,   77,
 /*   740 */   104,  105,  106,  107,  108,   75,  109,   77,  104,  105,
 /*   750 */   106,  107,  108,   91,  109,  109,  109,   75,  109,   77,
 /*   760 */    14,   91,  109,   75,  109,   77,  104,  105,  106,  107,
 /*   770 */   108,  109,  109,   91,  104,  105,  106,  107,  108,   91,
 /*   780 */   109,  109,  109,   75,  109,   77,  104,  105,  106,  107,
 /*   790 */   108,  109,  104,  105,  106,  107,  108,  109,  109,   91,
 /*   800 */   109,   55,   56,   75,   58,   77,   60,   61,   75,  109,
 /*   810 */    77,  109,  104,  105,  106,  107,  108,  109,  109,   91,
 /*   820 */    14,  109,  109,  109,   91,  109,   75,  109,   77,  109,
 /*   830 */   109,  109,  104,  105,  106,  107,  108,  104,  105,  106,
 /*   840 */   107,  108,   91,  109,   75,  109,   77,  109,  109,   75,
 /*   850 */   109,   77,  109,  109,  109,  104,  105,  106,  107,  108,
 /*   860 */    91,   55,   56,  109,   58,   91,  109,   61,   75,  109,
 /*   870 */    77,  109,  109,  104,  105,  106,  107,  108,  104,  105,
 /*   880 */   106,  107,  108,  109,   91,   74,   75,   76,   14,   75,
 /*   890 */    79,   74,   75,   76,   80,   75,   76,  104,  105,  106,
 /*   900 */   107,  108,   91,  109,  109,   91,  109,  109,   91,  109,
 /*   910 */   109,   91,   75,  109,  109,  104,  105,  106,  104,  105,
 /*   920 */   106,  104,  105,  106,  104,  105,  106,  109,   91,   55,
 /*   930 */    56,   75,   58,  109,   75,   61,   62,   75,  109,  109,
 /*   940 */    75,  104,  105,  106,  109,  109,  109,   91,  109,  109,
 /*   950 */    91,  109,  109,   91,  109,  109,   91,   75,  109,  109,
 /*   960 */   104,  105,  106,  104,  105,  106,  104,  105,  106,  104,
 /*   970 */   105,  106,  109,   91,   75,  109,  109,   75,  109,  109,
 /*   980 */   109,   75,  109,  109,   75,  109,  104,  105,  106,  109,
 /*   990 */    91,  109,  109,   91,  109,  109,  109,   91,  109,  109,
 /*  1000 */    91,   75,  109,  104,  105,  106,  104,  105,  106,  109,
 /*  1010 */   104,  105,  106,  104,  105,  106,   91,   91,   75,   94,
 /*  1020 */    95,  109,   91,   75,   99,  109,   75,  109,  103,   75,
 /*  1030 */   104,  105,  106,  109,   91,  104,  105,  106,  109,   91,
 /*  1040 */   109,  109,   91,  109,  109,   91,   75,  104,  105,  106,
 /*  1050 */    75,  109,  104,  105,  106,  104,  105,  106,  104,  105,
 /*  1060 */   106,  109,   91,  109,  109,  109,   91,  109,  109,   75,
 /*  1070 */   109,  109,  109,   75,  109,  104,  105,  106,  109,  104,
 /*  1080 */   105,  106,  109,  109,  109,   91,  109,  109,  109,   91,
 /*  1090 */    75,  109,  109,   75,  109,  109,  109,  109,  104,  105,
 /*  1100 */   106,  109,  104,  105,  106,  109,   91,  109,  109,   91,
 /*  1110 */    75,  109,  109,  109,  109,   75,  109,  109,   75,  104,
 /*  1120 */   105,  106,  104,  105,  106,  109,   91,  109,  109,  109,
 /*  1130 */   109,   91,  109,  109,   91,  109,  109,  109,  109,  104,
 /*  1140 */   105,  106,  109,  109,  104,  105,  106,  104,  105,  106,
};
#define YY_SHIFT_USE_DFLT (-55)
#define YY_SHIFT_MAX 174
static const short yy_shift_ofst[] = {
 /*     0 */   -55,   -9,   56,   56,   56,  121,  468,  487,  552,  552,
 /*    10 */   552,  552,  552,  552,  552,  552,  552,  552,  552,  552,
 /*    20 */   552,  552,  552,  571,  552,  643,  643,  643,  806,  806,
 /*    30 */   806,  206,  643,  643,  643,  643,  643,  643,  643,  643,
 /*    40 */   643,  643,  643,  643,  643,  643,  643,  643,  643,  643,
 /*    50 */   643,  643,  643,  643,  643,  643,  806,  643,  643,  643,
 /*    60 */   643,  643,  643,  806,  -54,  -54,  203,  262,  874,  203,
 /*    70 */    47,  106,  -54,   89,  746,  874,  806,  806,  806,  806,
 /*    80 */   806,  806,  111,  185,  -24,  188,  278,  297,  450,    5,
 /*    90 */   111,  453,  225,  286,  293,  -24,  -54,  375,  452,  -54,
 /*   100 */   478,  478,  -54,  479,  233,  342,  361,  380,  413,  413,
 /*   110 */   413,  413,  440,  457,  -10,   66,   66,   66,   66,   66,
 /*   120 */    66,  195,  195,  195,  195,  444,   76,  458,  433,  465,
 /*   130 */   476,  474,  529,  504,  489,  473,  506,  494,  486,  503,
 /*   140 */   505,  498,  509,  512,  514,  507,  499,  517,  534,  520,
 /*   150 */   557,  525,  547,  537,  540,  556,  525,  525,  525,  525,
 /*   160 */   525,  525,  525,  525,  525,  525,  525,  525,  525,  568,
 /*   170 */   570,  577,  578,  573,  569,
};
#define YY_REDUCE_USE_DFLT (-88)
#define YY_REDUCE_MAX 103
static const short yy_reduce_ofst[] = {
 /*     0 */    20,  500,  567,  590,   75,  519,   11,  451,  579,  626,
 /*    10 */   636,  644,  662,  670,  682,  688,  708,  728,  733,  751,
 /*    20 */   769,  774,  793,  811,  584,  817,  814,  820,  235,  -32,
 /*    30 */   399,  -67,  837,  856,  859,  862,  865,  882,  899,  902,
 /*    40 */   906,  909,  926,  943,  948,  951,  954,  971,  975,  994,
 /*    50 */   998, 1015, 1018, 1035, 1040, 1043,  925,   23,   41,   78,
 /*    60 */   495,  543,  931,  236,  313,  317,  -87,  -30,   10,  -68,
 /*    70 */   344,  345,  392,  -29,  -22,   46,   61,  134,  135,  136,
 /*    80 */   163,  184,  249,  257,  323,  -25,    8,   44,   87,  172,
 /*    90 */   189,  132,  240,  253,  271,  318,  309,  338,  414,  401,
 /*   100 */   445,  446,  424,  441,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   313,  276,  408,  408,  409,  276,  419,  419,  419,  419,
 /*    10 */   419,  419,  419,  419,  419,  419,  419,  419,  419,  419,
 /*    20 */   419,  419,  419,  276,  419,  276,  406,  419,  417,  415,
 /*    30 */   415,  419,  419,  419,  419,  419,  419,  419,  419,  419,
 /*    40 */   419,  419,  419,  419,  419,  419,  419,  419,  419,  419,
 /*    50 */   419,  419,  419,  419,  419,  419,  419,  419,  419,  419,
 /*    60 */   419,  419,  419,  405,  419,  419,  327,  419,  410,  327,
 /*    70 */   419,  419,  405,  404,  343,  419,  419,  419,  419,  419,
 /*    80 */   419,  419,  419,  419,  419,  402,  419,  419,  419,  419,
 /*    90 */   405,  419,  419,  404,  404,  405,  419,  354,  404,  419,
 /*   100 */   404,  404,  419,  419,  402,  419,  419,  419,  398,  403,
 /*   110 */   407,  402,  395,  394,  419,  388,  389,  390,  391,  392,
 /*   120 */   393,  384,  385,  386,  387,  413,  369,  419,  419,  419,
 /*   130 */   419,  419,  419,  419,  419,  419,  419,  419,  343,  419,
 /*   140 */   419,  419,  419,  419,  419,  396,  419,  419,  419,  419,
 /*   150 */   337,  338,  419,  419,  419,  419,  278,  279,  280,  281,
 /*   160 */   282,  283,  284,  285,  286,  287,  288,  289,  290,  419,
 /*   170 */   301,  419,  419,  419,  419,  314,  315,  316,  329,  330,
 /*   180 */   331,  328,  317,  318,  333,  334,  339,  340,  342,  412,
 /*   190 */   344,  351,  325,  326,  346,  350,  352,  353,  349,  345,
 /*   200 */   416,  347,  348,  355,  414,  356,  418,  357,  358,  359,
 /*   210 */   360,  361,  362,  363,  364,  370,  371,  372,  373,  374,
 /*   220 */   375,  376,  379,  380,  381,  382,  383,  365,  366,  367,
 /*   230 */   368,  397,  399,  400,  401,  378,  377,  341,  332,  319,
 /*   240 */   320,  336,  411,  413,  335,  321,  322,  274,  275,  291,
 /*   250 */   292,  293,  294,  295,  296,  297,  298,  299,  300,  302,
 /*   260 */   304,  303,  305,  306,  307,  309,  310,  311,  308,  312,
 /*   270 */   323,  324,  405,
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
  "expr_list",     "cexpr_list",    "decl",          "block_stmt",  
  "oexpr",         "sw_block_stmt",  "sw_case_list",  "sw_case",     
  "ostmt_list",    "import_spec",   "import_spec_list",  "type_spec",   
  "type_spec_list",  "value_spec",    "value_spec_list",  "ident",       
  "ofield_comma_list",  "func_results",  "type",          "field_comma_list",
  "ident_list",    "otype",         "ofield_semi_list_and_osemi",  "nametype_list",
  "ofunc_ellipsis",  "field_semi_list",  "field",         "nametype",    
  "basic_lit",     "pexpr",         "uexpr",         "compound_lit",
  "cexpr",       
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
 /*   5 */ "s_stmt ::= expr_list ASSIGN cexpr_list",
 /*   6 */ "s_stmt ::= expr_list DECLARIZE cexpr_list",
 /*   7 */ "s_stmt ::= expr_list A_DIVIDE cexpr_list",
 /*   8 */ "s_stmt ::= expr_list A_TIMES cexpr_list",
 /*   9 */ "s_stmt ::= expr_list A_MOD cexpr_list",
 /*  10 */ "s_stmt ::= expr_list A_SHIFTL cexpr_list",
 /*  11 */ "s_stmt ::= expr_list A_SHIFTR cexpr_list",
 /*  12 */ "s_stmt ::= expr_list A_AND cexpr_list",
 /*  13 */ "s_stmt ::= expr_list A_ANDNOT cexpr_list",
 /*  14 */ "s_stmt ::= expr_list A_PLUS cexpr_list",
 /*  15 */ "s_stmt ::= expr_list A_MINUS cexpr_list",
 /*  16 */ "s_stmt ::= expr_list A_OR cexpr_list",
 /*  17 */ "s_stmt ::= expr_list A_XOR cexpr_list",
 /*  18 */ "s_stmt ::= expr INC",
 /*  19 */ "s_stmt ::= expr DEC",
 /*  20 */ "stmt ::= s_stmt SEMICOLON",
 /*  21 */ "stmt ::= decl",
 /*  22 */ "stmt ::= block_stmt",
 /*  23 */ "stmt ::= BREAK",
 /*  24 */ "stmt ::= CONTINUE",
 /*  25 */ "stmt ::= FALLTHROUGH",
 /*  26 */ "stmt ::= RETURN cexpr_list SEMICOLON",
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
 /*  65 */ "value_spec ::= ident_list otype ASSIGN cexpr_list",
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
 /*  95 */ "pexpr ::= pexpr LPAREN cexpr_list RPAREN",
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
 /* 123 */ "compound_lit ::= LCURLY cexpr_list RCURLY",
 /* 124 */ "compound_lit ::= LCURLY cexpr_list RCURLY DOT LPAREN type RPAREN",
 /* 125 */ "cexpr ::= expr",
 /* 126 */ "cexpr ::= compound_lit",
 /* 127 */ "cexpr_list ::= cexpr",
 /* 128 */ "cexpr_list ::= cexpr_list COMMA cexpr",
 /* 129 */ "expr_list ::= expr",
 /* 130 */ "expr_list ::= expr_list COMMA expr",
 /* 131 */ "osemi ::=",
 /* 132 */ "osemi ::= SEMICOLON",
 /* 133 */ "oexpr ::=",
 /* 134 */ "oexpr ::= expr",
 /* 135 */ "ostmt_list ::=",
 /* 136 */ "ostmt_list ::= stmt_list",
 /* 137 */ "otype ::=",
 /* 138 */ "otype ::= type",
 /* 139 */ "ident ::= IDENT",
 /* 140 */ "ident_list ::= ident",
 /* 141 */ "ident_list ::= ident_list COMMA ident",
 /* 142 */ "ofield_comma_list ::=",
 /* 143 */ "ofield_comma_list ::= field_comma_list",
 /* 144 */ "ofield_semi_list_and_osemi ::=",
 /* 145 */ "ofield_semi_list_and_osemi ::= field_semi_list osemi",
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
#line 891 "parser.c"
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
  { 108, 1 },
  { 108, 1 },
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
	yygotominor.yy96 = new program_t(yymsp[-1].minor.yy10);
	ctx->ast = yygotominor.yy96;
}
#line 1326 "parser.c"
        break;
      case 1: /* stmt_list ::= stmt */
#line 44 "parser.y"
{ 
	yygotominor.yy10 = new node_vector_t;
	if (yymsp[0].minor.yy60) yygotominor.yy10->push_back(yymsp[0].minor.yy60);
}
#line 1334 "parser.c"
        break;
      case 2: /* stmt_list ::= stmt_list stmt */
#line 48 "parser.y"
{
	if (yymsp[0].minor.yy60) yymsp[-1].minor.yy10->push_back(yymsp[0].minor.yy60);
	yygotominor.yy10 = yymsp[-1].minor.yy10;
}
#line 1342 "parser.c"
        break;
      case 3: /* s_stmt ::= */
      case 133: /* oexpr ::= */ yytestcase(yyruleno==133);
      case 137: /* otype ::= */ yytestcase(yyruleno==137);
#line 61 "parser.y"
{ yygotominor.yy60 = 0; }
#line 1349 "parser.c"
        break;
      case 4: /* s_stmt ::= expr */
#line 62 "parser.y"
{ yygotominor.yy60 = new expr_stmt_t(yymsp[0].minor.yy60); }
#line 1354 "parser.c"
        break;
      case 5: /* s_stmt ::= expr_list ASSIGN cexpr_list */
      case 6: /* s_stmt ::= expr_list DECLARIZE cexpr_list */ yytestcase(yyruleno==6);
      case 7: /* s_stmt ::= expr_list A_DIVIDE cexpr_list */ yytestcase(yyruleno==7);
      case 8: /* s_stmt ::= expr_list A_TIMES cexpr_list */ yytestcase(yyruleno==8);
      case 9: /* s_stmt ::= expr_list A_MOD cexpr_list */ yytestcase(yyruleno==9);
      case 10: /* s_stmt ::= expr_list A_SHIFTL cexpr_list */ yytestcase(yyruleno==10);
      case 11: /* s_stmt ::= expr_list A_SHIFTR cexpr_list */ yytestcase(yyruleno==11);
      case 12: /* s_stmt ::= expr_list A_AND cexpr_list */ yytestcase(yyruleno==12);
      case 13: /* s_stmt ::= expr_list A_ANDNOT cexpr_list */ yytestcase(yyruleno==13);
      case 14: /* s_stmt ::= expr_list A_PLUS cexpr_list */ yytestcase(yyruleno==14);
      case 15: /* s_stmt ::= expr_list A_MINUS cexpr_list */ yytestcase(yyruleno==15);
      case 16: /* s_stmt ::= expr_list A_OR cexpr_list */ yytestcase(yyruleno==16);
      case 17: /* s_stmt ::= expr_list A_XOR cexpr_list */ yytestcase(yyruleno==17);
#line 63 "parser.y"
{ yygotominor.yy60 = new assign_stmt_t(yymsp[-2].minor.yy10, yymsp[0].minor.yy10, yymsp[-1].minor.yy0); }
#line 1371 "parser.c"
        break;
      case 18: /* s_stmt ::= expr INC */
      case 19: /* s_stmt ::= expr DEC */ yytestcase(yyruleno==19);
#line 76 "parser.y"
{ yygotominor.yy60 = new incdec_stmt_t(yymsp[-1].minor.yy60, yymsp[0].minor.yy0); }
#line 1377 "parser.c"
        break;
      case 20: /* stmt ::= s_stmt SEMICOLON */
#line 87 "parser.y"
{ yygotominor.yy60 = yymsp[-1].minor.yy60;   yy_destructor(yypParser,35,&yymsp[0].minor);
}
#line 1383 "parser.c"
        break;
      case 21: /* stmt ::= decl */
#line 88 "parser.y"
{ yygotominor.yy60 = new decl_stmt_t(yymsp[0].minor.yy60); }
#line 1388 "parser.c"
        break;
      case 22: /* stmt ::= block_stmt */
#line 89 "parser.y"
{ yygotominor.yy60 = yymsp[0].minor.yy2; }
#line 1393 "parser.c"
        break;
      case 23: /* stmt ::= BREAK */
      case 24: /* stmt ::= CONTINUE */ yytestcase(yyruleno==24);
      case 25: /* stmt ::= FALLTHROUGH */ yytestcase(yyruleno==25);
#line 90 "parser.y"
{ yygotominor.yy60 = new flow_stmt_t(yymsp[0].minor.yy0); }
#line 1400 "parser.c"
        break;
      case 26: /* stmt ::= RETURN cexpr_list SEMICOLON */
#line 93 "parser.y"
{ yygotominor.yy60 = new return_stmt_t(yymsp[-1].minor.yy10, yymsp[-2].minor.yy0);   yy_destructor(yypParser,35,&yymsp[0].minor);
}
#line 1406 "parser.c"
        break;
      case 27: /* stmt ::= RETURN SEMICOLON */
#line 94 "parser.y"
{ yygotominor.yy60 = new return_stmt_t(0, yymsp[-1].minor.yy0);   yy_destructor(yypParser,35,&yymsp[0].minor);
}
#line 1412 "parser.c"
        break;
      case 28: /* stmt ::= IF expr block_stmt */
#line 96 "parser.y"
{
	yygotominor.yy60 = new ifelse_stmt_t(yymsp[-1].minor.yy60, yymsp[0].minor.yy2, yymsp[-2].minor.yy0);
}
#line 1419 "parser.c"
        break;
      case 29: /* stmt ::= IF expr block_stmt ELSE stmt */
#line 99 "parser.y"
{
	yygotominor.yy60 = new ifelse_stmt_t(yymsp[-3].minor.yy60, yymsp[-2].minor.yy2, yymsp[-4].minor.yy0, yymsp[-1].minor.yy0, yymsp[0].minor.yy60);
}
#line 1426 "parser.c"
        break;
      case 30: /* stmt ::= FOR s_stmt SEMICOLON oexpr SEMICOLON s_stmt block_stmt */
#line 102 "parser.y"
{
	yygotominor.yy60 = new for_stmt_t(yymsp[-5].minor.yy60, yymsp[-3].minor.yy60, yymsp[-1].minor.yy60, yymsp[0].minor.yy2, yymsp[-6].minor.yy0);
  yy_destructor(yypParser,35,&yymsp[-4].minor);
  yy_destructor(yypParser,35,&yymsp[-2].minor);
}
#line 1435 "parser.c"
        break;
      case 31: /* stmt ::= FOR expr block_stmt */
#line 105 "parser.y"
{
	yygotominor.yy60 = new for_stmt_t(0, yymsp[-1].minor.yy60, 0, yymsp[0].minor.yy2, yymsp[-2].minor.yy0);
}
#line 1442 "parser.c"
        break;
      case 32: /* stmt ::= FOR block_stmt */
#line 108 "parser.y"
{
	yygotominor.yy60 = new for_stmt_t(0, 0, 0, yymsp[0].minor.yy2, yymsp[-1].minor.yy0);
}
#line 1449 "parser.c"
        break;
      case 33: /* stmt ::= SWITCH expr sw_block_stmt */
#line 111 "parser.y"
{
	yygotominor.yy60 = new switch_stmt_t(yymsp[-1].minor.yy60, yymsp[0].minor.yy2, yymsp[-2].minor.yy0);
}
#line 1456 "parser.c"
        break;
      case 34: /* sw_block_stmt ::= LCURLY sw_case_list RCURLY */
      case 39: /* block_stmt ::= LCURLY ostmt_list RCURLY */ yytestcase(yyruleno==39);
#line 123 "parser.y"
{
	yygotominor.yy2 = new block_stmt_t(yymsp[-1].minor.yy10, yymsp[-2].minor.yy0, yymsp[0].minor.yy0);
}
#line 1464 "parser.c"
        break;
      case 35: /* sw_case_list ::= sw_case */
#line 128 "parser.y"
{ yygotominor.yy10 = new node_vector_t(1, yymsp[0].minor.yy114); }
#line 1469 "parser.c"
        break;
      case 36: /* sw_case_list ::= sw_case_list sw_case */
#line 129 "parser.y"
{ yymsp[-1].minor.yy10->push_back(yymsp[0].minor.yy114); yygotominor.yy10 = yymsp[-1].minor.yy10; }
#line 1474 "parser.c"
        break;
      case 37: /* sw_case ::= CASE expr_list COLON ostmt_list */
#line 132 "parser.y"
{
	yygotominor.yy114 = new switch_stmt_case_t(yymsp[-2].minor.yy10, yymsp[0].minor.yy10, yymsp[-3].minor.yy0, yymsp[-1].minor.yy0);
}
#line 1481 "parser.c"
        break;
      case 38: /* sw_case ::= DEFAULT COLON ostmt_list */
#line 135 "parser.y"
{
	yygotominor.yy114 = new switch_stmt_case_t(0, yymsp[0].minor.yy10, yymsp[-2].minor.yy0, yymsp[-1].minor.yy0);
}
#line 1488 "parser.c"
        break;
      case 40: /* decl_list ::= */
#line 159 "parser.y"
{ yygotominor.yy10 = new node_vector_t; }
#line 1493 "parser.c"
        break;
      case 41: /* decl_list ::= decl_list decl */
#line 160 "parser.y"
{ yymsp[-1].minor.yy10->push_back(yymsp[0].minor.yy60); yygotominor.yy10 = yymsp[-1].minor.yy10; }
#line 1498 "parser.c"
        break;
      case 42: /* decl ::= IMPORT import_spec SEMICOLON */
#line 176 "parser.y"
{
	yygotominor.yy60 = new import_decl_t(yymsp[-1].minor.yy57, yymsp[-2].minor.yy0);
  yy_destructor(yypParser,35,&yymsp[0].minor);
}
#line 1506 "parser.c"
        break;
      case 43: /* decl ::= IMPORT LPAREN import_spec_list osemi RPAREN */
#line 179 "parser.y"
{
	yygotominor.yy60 = new import_decl_t(yymsp[-2].minor.yy155, yymsp[-4].minor.yy0, yymsp[-3].minor.yy0, yymsp[0].minor.yy0);
}
#line 1513 "parser.c"
        break;
      case 44: /* decl ::= TYPE type_spec SEMICOLON */
#line 182 "parser.y"
{
	yygotominor.yy60 = new type_decl_t(yymsp[-1].minor.yy78, yymsp[-2].minor.yy0);
  yy_destructor(yypParser,35,&yymsp[0].minor);
}
#line 1521 "parser.c"
        break;
      case 45: /* decl ::= TYPE LPAREN type_spec_list osemi RPAREN */
#line 185 "parser.y"
{
	yygotominor.yy60 = new type_decl_t(yymsp[-2].minor.yy168, yymsp[-4].minor.yy0, yymsp[-3].minor.yy0, yymsp[0].minor.yy0);
}
#line 1528 "parser.c"
        break;
      case 46: /* decl ::= CONST value_spec SEMICOLON */
#line 188 "parser.y"
{
	const_decl_t *d = new const_decl_t(yymsp[-1].minor.yy156, yymsp[-2].minor.yy0);
	syntax_check_const_decl(ctx->diag, d);
	yygotominor.yy60 = d;
  yy_destructor(yypParser,35,&yymsp[0].minor);
}
#line 1538 "parser.c"
        break;
      case 47: /* decl ::= CONST LPAREN value_spec_list osemi RPAREN */
#line 193 "parser.y"
{
	const_decl_t *d = new const_decl_t(yymsp[-2].minor.yy107, yymsp[-4].minor.yy0, yymsp[-3].minor.yy0, yymsp[0].minor.yy0);
	syntax_check_const_decl(ctx->diag, d);
	yygotominor.yy60 = d;
}
#line 1547 "parser.c"
        break;
      case 48: /* decl ::= VAR value_spec SEMICOLON */
#line 198 "parser.y"
{
	var_decl_t *d = new var_decl_t(yymsp[-1].minor.yy156, yymsp[-2].minor.yy0);
	syntax_check_var_decl(ctx->diag, d);
	yygotominor.yy60 = d;
  yy_destructor(yypParser,35,&yymsp[0].minor);
}
#line 1557 "parser.c"
        break;
      case 49: /* decl ::= VAR LPAREN value_spec_list osemi RPAREN */
#line 203 "parser.y"
{
	var_decl_t *d = new var_decl_t(yymsp[-2].minor.yy107, yymsp[-4].minor.yy0, yymsp[-3].minor.yy0, yymsp[0].minor.yy0);
	syntax_check_var_decl(ctx->diag, d);
	yygotominor.yy60 = d;
}
#line 1566 "parser.c"
        break;
      case 50: /* decl ::= FUNC ident LPAREN ofield_comma_list RPAREN func_results SEMICOLON */
#line 211 "parser.y"
{
	func_type_t *ftype = new func_type_t(yymsp[-3].minor.yy56, yymsp[-1].minor.yy56, yymsp[-6].minor.yy0);
	yygotominor.yy60 = new func_decl_t(yymsp[-5].minor.yy72, ftype);
  yy_destructor(yypParser,50,&yymsp[-4].minor);
  yy_destructor(yypParser,51,&yymsp[-2].minor);
  yy_destructor(yypParser,35,&yymsp[0].minor);
}
#line 1577 "parser.c"
        break;
      case 51: /* decl ::= FUNC ident LPAREN ofield_comma_list RPAREN func_results block_stmt */
#line 218 "parser.y"
{
	func_type_t *ftype = new func_type_t(yymsp[-3].minor.yy56, yymsp[-1].minor.yy56, yymsp[-6].minor.yy0);
	yygotominor.yy60 = new func_decl_t(yymsp[-5].minor.yy72, ftype, yymsp[0].minor.yy2);
  yy_destructor(yypParser,50,&yymsp[-4].minor);
  yy_destructor(yypParser,51,&yymsp[-2].minor);
}
#line 1587 "parser.c"
        break;
      case 52: /* func_results ::= type */
#line 231 "parser.y"
{
	field_t *f = new field_t(0, yymsp[0].minor.yy60);
	yygotominor.yy56 = new field_vector_t(1, f);
}
#line 1595 "parser.c"
        break;
      case 53: /* func_results ::= LPAREN field_comma_list RPAREN */
#line 235 "parser.y"
{ yygotominor.yy56 = yymsp[-1].minor.yy56;   yy_destructor(yypParser,50,&yymsp[-2].minor);
  yy_destructor(yypParser,51,&yymsp[0].minor);
}
#line 1602 "parser.c"
        break;
      case 54: /* func_results ::= */
      case 142: /* ofield_comma_list ::= */ yytestcase(yyruleno==142);
      case 144: /* ofield_semi_list_and_osemi ::= */ yytestcase(yyruleno==144);
#line 236 "parser.y"
{ yygotominor.yy56 = 0; }
#line 1609 "parser.c"
        break;
      case 55: /* import_spec_list ::= import_spec */
#line 249 "parser.y"
{ yygotominor.yy155 = new import_spec_vector_t(1, yymsp[0].minor.yy57); }
#line 1614 "parser.c"
        break;
      case 56: /* import_spec_list ::= import_spec_list SEMICOLON import_spec */
#line 250 "parser.y"
{
	yymsp[-2].minor.yy155->push_back(yymsp[0].minor.yy57); yygotominor.yy155 = yymsp[-2].minor.yy155;
  yy_destructor(yypParser,35,&yymsp[-1].minor);
}
#line 1622 "parser.c"
        break;
      case 57: /* import_spec ::= IDENT STRING */
#line 255 "parser.y"
{ yygotominor.yy57 = new import_spec_t(yymsp[-1].minor.yy0, yymsp[0].minor.yy0); }
#line 1627 "parser.c"
        break;
      case 58: /* import_spec ::= STRING */
#line 256 "parser.y"
{ yygotominor.yy57 = new import_spec_t(0, yymsp[0].minor.yy0); }
#line 1632 "parser.c"
        break;
      case 59: /* type_spec_list ::= type_spec */
#line 269 "parser.y"
{ yygotominor.yy168 = new type_spec_vector_t(1, yymsp[0].minor.yy78); }
#line 1637 "parser.c"
        break;
      case 60: /* type_spec_list ::= type_spec_list SEMICOLON type_spec */
#line 270 "parser.y"
{
	yymsp[-2].minor.yy168->push_back(yymsp[0].minor.yy78); yygotominor.yy168 = yymsp[-2].minor.yy168;
  yy_destructor(yypParser,35,&yymsp[-1].minor);
}
#line 1645 "parser.c"
        break;
      case 61: /* type_spec ::= IDENT type */
#line 275 "parser.y"
{ yygotominor.yy78 = new type_spec_t(yymsp[-1].minor.yy0, yymsp[0].minor.yy60); }
#line 1650 "parser.c"
        break;
      case 62: /* value_spec_list ::= value_spec */
#line 290 "parser.y"
{ yygotominor.yy107 = new value_spec_vector_t(1, yymsp[0].minor.yy156); }
#line 1655 "parser.c"
        break;
      case 63: /* value_spec_list ::= value_spec_list SEMICOLON value_spec */
#line 291 "parser.y"
{
	yymsp[-2].minor.yy107->push_back(yymsp[0].minor.yy156); yygotominor.yy107 = yymsp[-2].minor.yy107;
  yy_destructor(yypParser,35,&yymsp[-1].minor);
}
#line 1663 "parser.c"
        break;
      case 64: /* value_spec ::= ident_list otype */
#line 296 "parser.y"
{
	yygotominor.yy156 = new value_spec_t(yymsp[-1].minor.yy22, yymsp[0].minor.yy60);
}
#line 1670 "parser.c"
        break;
      case 65: /* value_spec ::= ident_list otype ASSIGN cexpr_list */
#line 299 "parser.y"
{
	yygotominor.yy156 = new value_spec_t(yymsp[-3].minor.yy22, yymsp[-2].minor.yy60, yymsp[0].minor.yy10, yymsp[-1].minor.yy0);
}
#line 1677 "parser.c"
        break;
      case 66: /* type ::= TIMES type */
#line 320 "parser.y"
{ yygotominor.yy60 = new pointer_type_t(yymsp[0].minor.yy60, yymsp[-1].minor.yy0); }
#line 1682 "parser.c"
        break;
      case 67: /* type ::= LSB expr RSB type */
#line 321 "parser.y"
{ yygotominor.yy60 = new array_type_t(yymsp[-2].minor.yy60, yymsp[0].minor.yy60, yymsp[-3].minor.yy0, yymsp[-1].minor.yy0); }
#line 1687 "parser.c"
        break;
      case 68: /* type ::= LSB RSB type */
#line 322 "parser.y"
{ yygotominor.yy60 = new array_type_t(0, yymsp[0].minor.yy60, yymsp[-2].minor.yy0, yymsp[-1].minor.yy0); }
#line 1692 "parser.c"
        break;
      case 69: /* type ::= ident DOT ident */
#line 323 "parser.y"
{ yygotominor.yy60 = new selector_expr_t(yymsp[-2].minor.yy72, yymsp[0].minor.yy72);   yy_destructor(yypParser,60,&yymsp[-1].minor);
}
#line 1698 "parser.c"
        break;
      case 70: /* type ::= ident */
      case 89: /* pexpr ::= ident */ yytestcase(yyruleno==89);
#line 324 "parser.y"
{ yygotominor.yy60 = yymsp[0].minor.yy72; }
#line 1704 "parser.c"
        break;
      case 71: /* type ::= STRUCT LCURLY ofield_semi_list_and_osemi RCURLY */
#line 325 "parser.y"
{
	yygotominor.yy60 = new struct_type_t(yymsp[-1].minor.yy56, yymsp[-3].minor.yy0, yymsp[-2].minor.yy0, yymsp[0].minor.yy0);	
}
#line 1711 "parser.c"
        break;
      case 72: /* type ::= FUNC LPAREN ofield_comma_list RPAREN func_results */
#line 328 "parser.y"
{
	yygotominor.yy60 = new func_type_t(yymsp[-2].minor.yy56, yymsp[0].minor.yy56, yymsp[-4].minor.yy0);
  yy_destructor(yypParser,50,&yymsp[-3].minor);
  yy_destructor(yypParser,51,&yymsp[-1].minor);
}
#line 1720 "parser.c"
        break;
      case 73: /* field_comma_list ::= nametype_list ofunc_ellipsis */
#line 343 "parser.y"
{
	yygotominor.yy56 = nametypev_to_fieldv(ctx->diag, yymsp[-1].minor.yy108, yymsp[0].minor.yy25);
}
#line 1727 "parser.c"
        break;
      case 74: /* field_semi_list ::= field */
#line 348 "parser.y"
{ yygotominor.yy56 = new field_vector_t(1, yymsp[0].minor.yy150); }
#line 1732 "parser.c"
        break;
      case 75: /* field_semi_list ::= field_semi_list SEMICOLON field */
#line 349 "parser.y"
{
	yymsp[-2].minor.yy56->push_back(yymsp[0].minor.yy150); yygotominor.yy56 = yymsp[-2].minor.yy56;
  yy_destructor(yypParser,35,&yymsp[-1].minor);
}
#line 1740 "parser.c"
        break;
      case 76: /* nametype_list ::= nametype */
#line 355 "parser.y"
{ yygotominor.yy108 = new nametype_vector_t(1, yymsp[0].minor.yy63); }
#line 1745 "parser.c"
        break;
      case 77: /* nametype_list ::= nametype_list COMMA nametype */
#line 356 "parser.y"
{
	yymsp[-2].minor.yy108->push_back(yymsp[0].minor.yy63); yygotominor.yy108 = yymsp[-2].minor.yy108;
  yy_destructor(yypParser,62,&yymsp[-1].minor);
}
#line 1753 "parser.c"
        break;
      case 78: /* nametype ::= ident type */
#line 361 "parser.y"
{ yygotominor.yy63.name = yymsp[-1].minor.yy72; yygotominor.yy63.type = yymsp[0].minor.yy60; }
#line 1758 "parser.c"
        break;
      case 79: /* nametype ::= type */
#line 362 "parser.y"
{ yygotominor.yy63.name = 0; yygotominor.yy63.type = yymsp[0].minor.yy60; }
#line 1763 "parser.c"
        break;
      case 80: /* ofunc_ellipsis ::= COMMA ELLIPSIS */
#line 372 "parser.y"
{ yygotominor.yy25 = true;   yy_destructor(yypParser,62,&yymsp[-1].minor);
  yy_destructor(yypParser,63,&yymsp[0].minor);
}
#line 1770 "parser.c"
        break;
      case 81: /* ofunc_ellipsis ::= */
#line 373 "parser.y"
{ yygotominor.yy25 = false; }
#line 1775 "parser.c"
        break;
      case 82: /* field ::= ident_list type */
#line 386 "parser.y"
{ yygotominor.yy150 = new field_t(yymsp[-1].minor.yy22, yymsp[0].minor.yy60); }
#line 1780 "parser.c"
        break;
      case 83: /* field ::= type */
#line 387 "parser.y"
{ yygotominor.yy150 = new field_t(0, yymsp[0].minor.yy60); }
#line 1785 "parser.c"
        break;
      case 84: /* basic_lit ::= INT */
      case 85: /* basic_lit ::= FLOAT */ yytestcase(yyruleno==85);
      case 86: /* basic_lit ::= STRING */ yytestcase(yyruleno==86);
      case 87: /* basic_lit ::= CHAR */ yytestcase(yyruleno==87);
#line 398 "parser.y"
{ yygotominor.yy61 = new basic_lit_expr_t(yymsp[0].minor.yy0); }
#line 1793 "parser.c"
        break;
      case 88: /* pexpr ::= basic_lit */
#line 414 "parser.y"
{ yygotominor.yy60 = yymsp[0].minor.yy61; }
#line 1798 "parser.c"
        break;
      case 90: /* pexpr ::= LPAREN expr RPAREN */
#line 416 "parser.y"
{ yygotominor.yy60 = new paren_expr_t(yymsp[-1].minor.yy60, yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1803 "parser.c"
        break;
      case 91: /* pexpr ::= pexpr LSB expr RSB */
#line 417 "parser.y"
{ yygotominor.yy60 = new index_expr_t(yymsp[-3].minor.yy60, yymsp[-1].minor.yy60, yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1808 "parser.c"
        break;
      case 92: /* pexpr ::= pexpr DOT IDENT */
#line 420 "parser.y"
{
	yygotominor.yy60 = new selector_expr_t(yymsp[-2].minor.yy60, new ident_expr_t(yymsp[0].minor.yy0));
  yy_destructor(yypParser,60,&yymsp[-1].minor);
}
#line 1816 "parser.c"
        break;
      case 93: /* pexpr ::= pexpr DOT LPAREN type RPAREN */
#line 425 "parser.y"
{
	yygotominor.yy60 = new type_cast_expr_t(yymsp[-4].minor.yy60, yymsp[-1].minor.yy60, yymsp[-3].minor.yy0, yymsp[-2].minor.yy0, yymsp[0].minor.yy0);
}
#line 1823 "parser.c"
        break;
      case 94: /* pexpr ::= pexpr LPAREN RPAREN */
#line 431 "parser.y"
{ yygotominor.yy60 = new call_expr_t(yymsp[-2].minor.yy60, 0, yymsp[-1].minor.yy0, yymsp[0].minor.yy0); }
#line 1828 "parser.c"
        break;
      case 95: /* pexpr ::= pexpr LPAREN cexpr_list RPAREN */
#line 434 "parser.y"
{
	yygotominor.yy60 = new call_expr_t(yymsp[-3].minor.yy60, yymsp[-1].minor.yy10, yymsp[-2].minor.yy0, yymsp[0].minor.yy0);
}
#line 1835 "parser.c"
        break;
      case 96: /* uexpr ::= pexpr */
      case 103: /* expr ::= uexpr */ yytestcase(yyruleno==103);
      case 125: /* cexpr ::= expr */ yytestcase(yyruleno==125);
      case 126: /* cexpr ::= compound_lit */ yytestcase(yyruleno==126);
      case 134: /* oexpr ::= expr */ yytestcase(yyruleno==134);
      case 138: /* otype ::= type */ yytestcase(yyruleno==138);
#line 448 "parser.y"
{ yygotominor.yy60 = yymsp[0].minor.yy60; }
#line 1845 "parser.c"
        break;
      case 97: /* uexpr ::= PLUS uexpr */
      case 98: /* uexpr ::= MINUS uexpr */ yytestcase(yyruleno==98);
      case 99: /* uexpr ::= NOT uexpr */ yytestcase(yyruleno==99);
      case 100: /* uexpr ::= AND uexpr */ yytestcase(yyruleno==100);
      case 101: /* uexpr ::= TIMES uexpr */ yytestcase(yyruleno==101);
      case 102: /* uexpr ::= XOR uexpr */ yytestcase(yyruleno==102);
#line 449 "parser.y"
{ yygotominor.yy60 = new unary_expr_t(yymsp[0].minor.yy60, yymsp[-1].minor.yy0); }
#line 1855 "parser.c"
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
{ yygotominor.yy60 = new binary_expr_t(yymsp[-2].minor.yy60, yymsp[0].minor.yy60, yymsp[-1].minor.yy0); }
#line 1878 "parser.c"
        break;
      case 123: /* compound_lit ::= LCURLY cexpr_list RCURLY */
#line 498 "parser.y"
{
	yygotominor.yy60 = new compound_lit_t(yymsp[-1].minor.yy10, 0, yymsp[-2].minor.yy0, yymsp[0].minor.yy0);
}
#line 1885 "parser.c"
        break;
      case 124: /* compound_lit ::= LCURLY cexpr_list RCURLY DOT LPAREN type RPAREN */
#line 501 "parser.y"
{
	yygotominor.yy60 = new compound_lit_t(yymsp[-5].minor.yy10, yymsp[-1].minor.yy60, yymsp[-6].minor.yy0, yymsp[0].minor.yy0);
  yy_destructor(yypParser,45,&yymsp[-4].minor);
  yy_destructor(yypParser,60,&yymsp[-3].minor);
  yy_destructor(yypParser,50,&yymsp[-2].minor);
}
#line 1895 "parser.c"
        break;
      case 127: /* cexpr_list ::= cexpr */
      case 129: /* expr_list ::= expr */ yytestcase(yyruleno==129);
#line 518 "parser.y"
{ yygotominor.yy10 = new node_vector_t(1, yymsp[0].minor.yy60); }
#line 1901 "parser.c"
        break;
      case 128: /* cexpr_list ::= cexpr_list COMMA cexpr */
      case 130: /* expr_list ::= expr_list COMMA expr */ yytestcase(yyruleno==130);
#line 519 "parser.y"
{ yymsp[-2].minor.yy10->push_back(yymsp[0].minor.yy60); yygotominor.yy10 = yymsp[-2].minor.yy10;   yy_destructor(yypParser,62,&yymsp[-1].minor);
}
#line 1908 "parser.c"
        break;
      case 132: /* osemi ::= SEMICOLON */
#line 534 "parser.y"
{
  yy_destructor(yypParser,35,&yymsp[0].minor);
}
#line 1915 "parser.c"
        break;
      case 135: /* ostmt_list ::= */
#line 543 "parser.y"
{ yygotominor.yy10 = 0; }
#line 1920 "parser.c"
        break;
      case 136: /* ostmt_list ::= stmt_list */
#line 544 "parser.y"
{ yygotominor.yy10 = yymsp[0].minor.yy10; }
#line 1925 "parser.c"
        break;
      case 139: /* ident ::= IDENT */
#line 553 "parser.y"
{ yygotominor.yy72 = new ident_expr_t(yymsp[0].minor.yy0); }
#line 1930 "parser.c"
        break;
      case 140: /* ident_list ::= ident */
#line 557 "parser.y"
{ yygotominor.yy22 = new ident_expr_vector_t(1, yymsp[0].minor.yy72); }
#line 1935 "parser.c"
        break;
      case 141: /* ident_list ::= ident_list COMMA ident */
#line 558 "parser.y"
{ yymsp[-2].minor.yy22->push_back(yymsp[0].minor.yy72); yygotominor.yy22 = yymsp[-2].minor.yy22;   yy_destructor(yypParser,62,&yymsp[-1].minor);
}
#line 1941 "parser.c"
        break;
      case 143: /* ofield_comma_list ::= field_comma_list */
#line 563 "parser.y"
{ yygotominor.yy56 = yymsp[0].minor.yy56; }
#line 1946 "parser.c"
        break;
      case 145: /* ofield_semi_list_and_osemi ::= field_semi_list osemi */
#line 568 "parser.y"
{ yygotominor.yy56 = yymsp[-1].minor.yy56; }
#line 1951 "parser.c"
        break;
      default:
      /* (131) osemi ::= */ yytestcase(yyruleno==131);
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
#line 2022 "parser.c"
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
