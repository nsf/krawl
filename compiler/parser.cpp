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
#define YYNOCODE 112
#define YYACTIONTYPE unsigned short int
#define ParseTOKENTYPE  token_t* 
typedef union {
  int yyinit;
  ParseTOKENTYPE yy0;
  nametype_t yy13;
  nametype_vector_t* yy21;
  field_t* yy32;
  block_stmt_t* yy42;
  type_spec_t* yy54;
  node_t* yy100;
  basic_lit_expr_t* yy126;
  program_t* yy140;
  import_spec_t* yy161;
  value_spec_t* yy165;
  value_spec_vector_t* yy166;
  node_vector_t* yy168;
  import_spec_vector_t* yy183;
  switch_stmt_case_t* yy188;
  ident_expr_t* yy196;
  type_spec_vector_t* yy198;
  field_vector_t* yy210;
  ident_expr_vector_t* yy214;
  bool yy215;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define ParseARG_SDECL  parser_t *ctx ;
#define ParseARG_PDECL , parser_t *ctx 
#define ParseARG_FETCH  parser_t *ctx  = yypParser->ctx 
#define ParseARG_STORE yypParser->ctx  = ctx 
#define YYNSTATE 279
#define YYNRULE 150
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
 /*     0 */    57,   58,   27,   62,  176,   61,  105,  106,  116,   60,
 /*    10 */    10,   11,   12,   13,   14,   15,   16,   17,   18,   19,
 /*    20 */    20,   21,   22,  216,  430,   74,   64,  260,  261,  262,
 /*    30 */     7,   54,  192,   24,   55,    1,  418,  215,  128,  225,
 /*    40 */    90,   33,  134,   93,   71,   72,  104,  192,  213,  178,
 /*    50 */   192,   75,  144,   53,  198,  211,  212,  214,   59,   99,
 /*    60 */   204,   65,  137,  183,  199,   57,   58,  192,   62,  263,
 /*    70 */    61,  113,  133,   77,   60,   41,   42,   43,   44,   32,
 /*    80 */    34,   36,   37,   38,   39,   40,  140,  216,  200,  195,
 /*    90 */   269,  282,  260,  261,  262,    7,   54,   23,   24,   55,
 /*   100 */     1,  215,  128,  225,  140,   90,   33,  194,   93,   71,
 /*   110 */    72,  104,  192,  213,  127,  140,  135,  209,  207,   76,
 /*   120 */   211,  212,  214,   59,  270,   27,  206,  176,  216,  127,
 /*   130 */    57,   58,  209,   62,   76,   61,  142,    6,  148,   60,
 /*   140 */   100,  205,  215,  128,  219,   35,  266,  129,  254,  172,
 /*   150 */   106,  116,  140,  258,  259,  188,   23,  260,  261,  262,
 /*   160 */     7,   54,   84,   24,   55,    1,  216,  234,  137,  183,
 /*   170 */    90,   33,  271,   93,   71,   72,  104,  192,  213,   23,
 /*   180 */   215,  128,  225,   91,  274,  211,  212,  214,   59,   52,
 /*   190 */    51,   45,   46,   47,   48,   49,   50,   41,   42,   43,
 /*   200 */    44,   32,   34,   36,   37,   38,   39,   40,   32,   34,
 /*   210 */    36,   37,   38,   39,   40,   57,   58,   78,   62,   86,
 /*   220 */    61,  255,  256,  283,   60,   77,  250,  102,  249,  184,
 /*   230 */    95,  173,    1,   68,   52,   51,   45,   46,   47,   48,
 /*   240 */    49,   50,   41,   42,   43,   44,   32,   34,   36,   37,
 /*   250 */    38,   39,   40,   56,  140,  181,   33,  189,  143,  192,
 /*   260 */   276,   31,  192,  213,  141,   83,  255,  256,  283,    1,
 /*   270 */   211,  212,  214,   59,  244,   96,   78,  283,  138,   52,
 /*   280 */    51,   45,   46,   47,   48,   49,   50,   41,   42,   43,
 /*   290 */    44,   32,   34,   36,   37,   38,   39,   40,   52,   51,
 /*   300 */    45,   46,   47,   48,   49,   50,   41,   42,   43,   44,
 /*   310 */    32,   34,   36,   37,   38,   39,   40,  143,  192,  277,
 /*   320 */    31,   92,    1,  141,  202,   75,   75,  158,  198,  198,
 /*   330 */   250,  103,  249,  140,   99,  204,  190,   68,  197,  199,
 /*   340 */   136,   85,   97,   52,   51,   45,   46,   47,   48,   49,
 /*   350 */    50,   41,   42,   43,   44,   32,   34,   36,   37,   38,
 /*   360 */    39,   40,   52,   51,   45,   46,   47,   48,   49,   50,
 /*   370 */    41,   42,   43,   44,   32,   34,   36,   37,   38,   39,
 /*   380 */    40,   52,   51,   45,   46,   47,   48,   49,   50,   41,
 /*   390 */    42,   43,   44,   32,   34,   36,   37,   38,   39,   40,
 /*   400 */    75,   79,  140,  198,  145,  147,  139,  140,  146,   94,
 /*   410 */   195,  187,  217,  199,   52,   51,   45,   46,   47,   48,
 /*   420 */    49,   50,   41,   42,   43,   44,   32,   34,   36,   37,
 /*   430 */    38,   39,   40,   67,  191,  140,   63,  140,  248,  218,
 /*   440 */   150,  153,   51,   45,   46,   47,   48,   49,   50,   41,
 /*   450 */    42,   43,   44,   32,   34,   36,   37,   38,   39,   40,
 /*   460 */    45,   46,   47,   48,   49,   50,   41,   42,   43,   44,
 /*   470 */    32,   34,   36,   37,   38,   39,   40,   57,   58,  216,
 /*   480 */    62,  151,   61,  249,  201,  155,   60,  249,   68,  210,
 /*   490 */   278,  208,   68,  215,  128,  220,   57,   58,  112,   62,
 /*   500 */    80,   61,   73,  175,   90,   60,  231,   93,   71,   72,
 /*   510 */   104,  349,    8,  152,  216,  156,   78,  157,   33,  233,
 /*   520 */    82,  349,  264,  140,  192,  213,  236,    1,  215,  128,
 /*   530 */   225,    8,  211,  212,  214,   59,   98,   33,  247,   82,
 /*   540 */   249,  267,  279,  192,  213,   68,  179,  140,  180,  182,
 /*   550 */   243,  211,  212,  214,   59,    2,  185,  143,  192,   98,
 /*   560 */    31,   57,   58,  141,   62,  101,   61,  186,   30,   66,
 /*   570 */    60,   53,    4,  253,  172,  106,  116,  193,  258,  259,
 /*   580 */    57,   58,  196,   62,  177,   61,   28,  149,   70,   60,
 /*   590 */   232,  216,  265,  172,  106,  116,    8,  258,  259,   81,
 /*   600 */   245,  235,   33,    9,   82,  215,  128,  225,  192,  213,
 /*   610 */   216,  246,  251,   78,   23,    1,  211,  212,  214,   59,
 /*   620 */   252,   33,   29,  257,  215,  128,  225,  192,  213,   69,
 /*   630 */     5,   26,   25,    3,  431,  211,  212,  214,   59,    4,
 /*   640 */   253,  172,  106,  116,  431,  258,  259,   57,   58,  275,
 /*   650 */    62,  272,   61,  431,  143,  192,   60,   31,  216,  431,
 /*   660 */   141,  203,  431,  431,    4,  253,  172,  106,  116,  431,
 /*   670 */   258,  259,  215,  128,  225,  431,  273,  431,  431,  431,
 /*   680 */   110,  431,  130,  216,  431,  431,  431,  431,   33,   78,
 /*   690 */   431,  110,  431,  132,  192,  213,  216,  215,  128,  225,
 /*   700 */   431,  431,  211,  212,  214,   59,  431,  216,  431,  431,
 /*   710 */   215,  128,  225,  237,  238,  239,  110,  431,  131,  431,
 /*   720 */   431,  215,  128,  225,  237,  238,  239,  110,  431,  154,
 /*   730 */   143,  192,  216,   31,  431,   98,  141,  431,  431,  431,
 /*   740 */   110,  431,  159,  216,  431,  431,  215,  128,  225,  237,
 /*   750 */   238,  239,  110,  431,  160,  431,  216,  215,  128,  225,
 /*   760 */   237,  238,  239,  216,  110,   78,  161,  431,  216,  431,
 /*   770 */   215,  128,  225,  237,  238,  239,  431,  215,  128,  221,
 /*   780 */   216,  431,  215,  128,  225,  237,  238,  239,  110,  431,
 /*   790 */   162,  431,  431,  431,  215,  128,  225,  237,  238,  239,
 /*   800 */   110,  431,  163,  431,  216,  431,  143,  192,  110,   31,
 /*   810 */   164,  431,  141,  216,  431,  431,  216,  431,  215,  128,
 /*   820 */   225,  237,  238,  239,  216,  431,  431,  215,  128,  222,
 /*   830 */   215,  128,  225,  237,  238,  239,  431,  107,  215,  128,
 /*   840 */   225,  237,  238,  239,  110,  431,  165,  431,  216,  431,
 /*   850 */   110,  431,  166,  216,  431,  431,  110,  431,  167,  431,
 /*   860 */   216,  431,  215,  128,  223,  431,  216,  215,  128,  225,
 /*   870 */   431,  431,  216,  431,  215,  128,  225,  237,  238,  239,
 /*   880 */   215,  128,  225,  237,  238,  239,  215,  128,  225,  237,
 /*   890 */   238,  239,  110,  431,  168,  431,  216,  431,  110,  431,
 /*   900 */   169,  431,  431,  431,  431,  431,  431,  431,  216,  431,
 /*   910 */   215,  128,  224,  431,  216,  431,  431,  431,  110,  431,
 /*   920 */   170,  431,  215,  128,  225,  237,  238,  239,  215,  128,
 /*   930 */   225,  237,  238,  239,  216,  431,  431,  431,  431,  431,
 /*   940 */   431,  431,  110,  431,  171,  431,  431,  431,  215,  128,
 /*   950 */   225,  237,  238,  239,  110,  431,  431,  431,  216,  431,
 /*   960 */   431,  174,   87,  116,  431,  242,  268,  431,  431,  431,
 /*   970 */   216,  431,  215,  128,  225,  237,  238,  239,  216,  431,
 /*   980 */   431,  216,  431,  431,  215,  128,  225,  237,  238,  240,
 /*   990 */   108,  431,  215,  128,  225,  215,  128,  225,  241,  431,
 /*  1000 */   431,  109,  431,  431,  431,  431,  216,  226,  431,  431,
 /*  1010 */   227,  431,  431,  431,  216,  431,  431,  216,  228,  431,
 /*  1020 */   215,  128,  225,  216,  431,  431,  216,  229,  215,  128,
 /*  1030 */   225,  215,  128,  225,  216,  431,  431,  215,  128,  225,
 /*  1040 */   215,  128,  225,  216,  431,  431,  230,  431,  215,  128,
 /*  1050 */   225,  123,  431,  431,  124,  431,  431,  215,  128,  225,
 /*  1060 */   431,  431,  216,  125,  431,  431,  431,  216,  431,  431,
 /*  1070 */   216,  126,  431,  431,  117,  431,  215,  128,  225,  216,
 /*  1080 */   431,  215,  128,  225,  215,  128,  225,  216,  431,  431,
 /*  1090 */   216,  431,  431,  215,  128,  225,  118,  431,  431,  119,
 /*  1100 */   431,  215,  128,  225,  215,  128,  225,  120,  431,  431,
 /*  1110 */   431,  431,  216,  431,  431,  216,  121,  431,  431,  122,
 /*  1120 */   431,  431,  431,  216,  115,  431,  215,  128,  225,  215,
 /*  1130 */   128,  225,  216,  431,  431,  216,  431,  215,  128,  225,
 /*  1140 */   216,  431,  431,  114,  431,  431,  215,  128,  225,  215,
 /*  1150 */   128,  225,  111,  431,  215,  128,  225,  431,  431,  216,
 /*  1160 */    88,  431,  431,   89,  431,  431,  431,  431,  216,  431,
 /*  1170 */   431,  431,  431,  215,  128,  225,  216,  431,  431,  216,
 /*  1180 */   431,  431,  215,  128,  225,  431,  431,  431,  431,  431,
 /*  1190 */   215,  128,  225,  215,  128,  225,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */     9,   10,   46,   12,   48,   14,   74,   75,   76,   18,
 /*    10 */    20,   21,   22,   23,   24,   25,   26,   27,   28,   29,
 /*    20 */    30,   31,   32,   91,   69,   70,   50,   36,   37,   38,
 /*    30 */    39,   40,   56,   42,   43,   44,   45,  105,  106,  107,
 /*    40 */    49,   50,   71,   52,   53,   54,   55,   56,   57,   78,
 /*    50 */    56,   91,   92,   63,   94,   64,   65,   66,   67,   99,
 /*    60 */   100,   50,   56,   57,  104,    9,   10,   56,   12,   35,
 /*    70 */    14,   75,   76,   56,   18,    9,   10,   11,   12,   13,
 /*    80 */    14,   15,   16,   17,   18,   19,   91,   91,   93,   94,
 /*    90 */    81,   35,   36,   37,   38,   39,   40,   63,   42,   43,
 /*   100 */    44,  105,  106,  107,   91,   49,   50,   94,   52,   53,
 /*   110 */    54,   55,   56,   57,   91,   91,   85,   94,   94,   96,
 /*   120 */    64,   65,   66,   67,   45,   46,  103,   48,   91,   91,
 /*   130 */     9,   10,   94,   12,   96,   14,   98,   50,   45,   18,
 /*   140 */   102,  103,  105,  106,  107,   58,   79,   60,   73,   74,
 /*   150 */    75,   76,   91,   78,   79,   94,   63,   36,   37,   38,
 /*   160 */    39,   40,   50,   42,   43,   44,   91,   51,   56,   57,
 /*   170 */    49,   50,   83,   52,   53,   54,   55,   56,   57,   63,
 /*   180 */   105,  106,  107,   82,   83,   64,   65,   66,   67,    1,
 /*   190 */     2,    3,    4,    5,    6,    7,    8,    9,   10,   11,
 /*   200 */    12,   13,   14,   15,   16,   17,   18,   19,   13,   14,
 /*   210 */    15,   16,   17,   18,   19,    9,   10,   14,   12,   50,
 /*   220 */    14,   33,   34,   35,   18,   56,   89,   90,   91,   85,
 /*   230 */    86,   79,   44,   96,    1,    2,    3,    4,    5,    6,
 /*   240 */     7,    8,    9,   10,   11,   12,   13,   14,   15,   16,
 /*   250 */    17,   18,   19,   50,   91,   85,   50,   94,   55,   56,
 /*   260 */    35,   58,   56,   57,   61,   59,   33,   34,   35,   44,
 /*   270 */    64,   65,   66,   67,   87,   88,   14,   44,   87,    1,
 /*   280 */     2,    3,    4,    5,    6,    7,    8,    9,   10,   11,
 /*   290 */    12,   13,   14,   15,   16,   17,   18,   19,    1,    2,
 /*   300 */     3,    4,    5,    6,    7,    8,    9,   10,   11,   12,
 /*   310 */    13,   14,   15,   16,   17,   18,   19,   55,   56,   79,
 /*   320 */    58,   35,   44,   61,   62,   91,   91,   92,   94,   94,
 /*   330 */    89,   90,   91,   91,   99,  100,   94,   96,  104,  104,
 /*   340 */    71,   44,   35,    1,    2,    3,    4,    5,    6,    7,
 /*   350 */     8,    9,   10,   11,   12,   13,   14,   15,   16,   17,
 /*   360 */    18,   19,    1,    2,    3,    4,    5,    6,    7,    8,
 /*   370 */     9,   10,   11,   12,   13,   14,   15,   16,   17,   18,
 /*   380 */    19,    1,    2,    3,    4,    5,    6,    7,    8,    9,
 /*   390 */    10,   11,   12,   13,   14,   15,   16,   17,   18,   19,
 /*   400 */    91,   59,   91,   94,   95,   94,   71,   91,   99,   93,
 /*   410 */    94,   87,   51,  104,    1,    2,    3,    4,    5,    6,
 /*   420 */     7,    8,    9,   10,   11,   12,   13,   14,   15,   16,
 /*   430 */    17,   18,   19,   63,   91,   91,   35,   91,   94,   59,
 /*   440 */    94,   97,    2,    3,    4,    5,    6,    7,    8,    9,
 /*   450 */    10,   11,   12,   13,   14,   15,   16,   17,   18,   19,
 /*   460 */     3,    4,    5,    6,    7,    8,    9,   10,   11,   12,
 /*   470 */    13,   14,   15,   16,   17,   18,   19,    9,   10,   91,
 /*   480 */    12,   89,   14,   91,  101,   89,   18,   91,   96,   71,
 /*   490 */    35,   91,   96,  105,  106,  107,    9,   10,   75,   12,
 /*   500 */    50,   14,   35,   80,   49,   18,   56,   52,   53,   54,
 /*   510 */    55,   35,   44,   71,   91,   71,   14,   91,   50,   51,
 /*   520 */    52,   45,   35,   91,   56,   57,   94,   44,  105,  106,
 /*   530 */   107,   44,   64,   65,   66,   67,   60,   50,   89,   52,
 /*   540 */    91,   79,    0,   56,   57,   96,   35,   91,   51,   57,
 /*   550 */    94,   64,   65,   66,   67,   47,   35,   55,   56,   60,
 /*   560 */    58,    9,   10,   61,   12,   63,   14,   51,   44,   51,
 /*   570 */    18,   63,   72,   73,   74,   75,   76,   45,   78,   79,
 /*   580 */     9,   10,   51,   12,   84,   14,   50,   60,   63,   18,
 /*   590 */    51,   91,   73,   74,   75,   76,   44,   78,   79,   50,
 /*   600 */    35,   51,   50,   20,   52,  105,  106,  107,   56,   57,
 /*   610 */    91,   51,   35,   14,   63,   44,   64,   65,   66,   67,
 /*   620 */    51,   50,   50,   35,  105,  106,  107,   56,   57,   51,
 /*   630 */    41,   35,   35,   47,  111,   64,   65,   66,   67,   72,
 /*   640 */    73,   74,   75,   76,  111,   78,   79,    9,   10,   45,
 /*   650 */    12,   84,   14,  111,   55,   56,   18,   58,   91,  111,
 /*   660 */    61,   62,  111,  111,   72,   73,   74,   75,   76,  111,
 /*   670 */    78,   79,  105,  106,  107,  111,   84,  111,  111,  111,
 /*   680 */    75,  111,   77,   91,  111,  111,  111,  111,   50,   14,
 /*   690 */   111,   75,  111,   77,   56,   57,   91,  105,  106,  107,
 /*   700 */   111,  111,   64,   65,   66,   67,  111,   91,  111,  111,
 /*   710 */   105,  106,  107,  108,  109,  110,   75,  111,   77,  111,
 /*   720 */   111,  105,  106,  107,  108,  109,  110,   75,  111,   77,
 /*   730 */    55,   56,   91,   58,  111,   60,   61,  111,  111,  111,
 /*   740 */    75,  111,   77,   91,  111,  111,  105,  106,  107,  108,
 /*   750 */   109,  110,   75,  111,   77,  111,   91,  105,  106,  107,
 /*   760 */   108,  109,  110,   91,   75,   14,   77,  111,   91,  111,
 /*   770 */   105,  106,  107,  108,  109,  110,  111,  105,  106,  107,
 /*   780 */    91,  111,  105,  106,  107,  108,  109,  110,   75,  111,
 /*   790 */    77,  111,  111,  111,  105,  106,  107,  108,  109,  110,
 /*   800 */    75,  111,   77,  111,   91,  111,   55,   56,   75,   58,
 /*   810 */    77,  111,   61,   91,  111,  111,   91,  111,  105,  106,
 /*   820 */   107,  108,  109,  110,   91,  111,  111,  105,  106,  107,
 /*   830 */   105,  106,  107,  108,  109,  110,  111,   75,  105,  106,
 /*   840 */   107,  108,  109,  110,   75,  111,   77,  111,   91,  111,
 /*   850 */    75,  111,   77,   91,  111,  111,   75,  111,   77,  111,
 /*   860 */    91,  111,  105,  106,  107,  111,   91,  105,  106,  107,
 /*   870 */   111,  111,   91,  111,  105,  106,  107,  108,  109,  110,
 /*   880 */   105,  106,  107,  108,  109,  110,  105,  106,  107,  108,
 /*   890 */   109,  110,   75,  111,   77,  111,   91,  111,   75,  111,
 /*   900 */    77,  111,  111,  111,  111,  111,  111,  111,   91,  111,
 /*   910 */   105,  106,  107,  111,   91,  111,  111,  111,   75,  111,
 /*   920 */    77,  111,  105,  106,  107,  108,  109,  110,  105,  106,
 /*   930 */   107,  108,  109,  110,   91,  111,  111,  111,  111,  111,
 /*   940 */   111,  111,   75,  111,   77,  111,  111,  111,  105,  106,
 /*   950 */   107,  108,  109,  110,   75,  111,  111,  111,   91,  111,
 /*   960 */   111,   74,   75,   76,  111,   75,   79,  111,  111,  111,
 /*   970 */    91,  111,  105,  106,  107,  108,  109,  110,   91,  111,
 /*   980 */   111,   91,  111,  111,  105,  106,  107,  108,  109,  110,
 /*   990 */    75,  111,  105,  106,  107,  105,  106,  107,   75,  111,
 /*  1000 */   111,   75,  111,  111,  111,  111,   91,   75,  111,  111,
 /*  1010 */    75,  111,  111,  111,   91,  111,  111,   91,   75,  111,
 /*  1020 */   105,  106,  107,   91,  111,  111,   91,   75,  105,  106,
 /*  1030 */   107,  105,  106,  107,   91,  111,  111,  105,  106,  107,
 /*  1040 */   105,  106,  107,   91,  111,  111,   75,  111,  105,  106,
 /*  1050 */   107,   75,  111,  111,   75,  111,  111,  105,  106,  107,
 /*  1060 */   111,  111,   91,   75,  111,  111,  111,   91,  111,  111,
 /*  1070 */    91,   75,  111,  111,   75,  111,  105,  106,  107,   91,
 /*  1080 */   111,  105,  106,  107,  105,  106,  107,   91,  111,  111,
 /*  1090 */    91,  111,  111,  105,  106,  107,   75,  111,  111,   75,
 /*  1100 */   111,  105,  106,  107,  105,  106,  107,   75,  111,  111,
 /*  1110 */   111,  111,   91,  111,  111,   91,   75,  111,  111,   75,
 /*  1120 */   111,  111,  111,   91,   75,  111,  105,  106,  107,  105,
 /*  1130 */   106,  107,   91,  111,  111,   91,  111,  105,  106,  107,
 /*  1140 */    91,  111,  111,   75,  111,  111,  105,  106,  107,  105,
 /*  1150 */   106,  107,   75,  111,  105,  106,  107,  111,  111,   91,
 /*  1160 */    75,  111,  111,   75,  111,  111,  111,  111,   91,  111,
 /*  1170 */   111,  111,  111,  105,  106,  107,   91,  111,  111,   91,
 /*  1180 */   111,  111,  105,  106,  107,  111,  111,  111,  111,  111,
 /*  1190 */   105,  106,  107,  105,  106,  107,
};
#define YY_SHIFT_USE_DFLT (-45)
#define YY_SHIFT_MAX 177
static const short yy_shift_ofst[] = {
 /*     0 */   -45,   -9,   56,   56,   56,  121,  468,  487,  552,  552,
 /*    10 */   552,  552,  552,  552,  552,  552,  552,  552,  552,  552,
 /*    20 */   552,  552,  552,  552,  571,  638,  638,  638,  599,  599,
 /*    30 */   751,  206,  638,  638,  638,  638,  638,  638,  638,  638,
 /*    40 */   638,  638,  638,  638,  638,  638,  638,  638,  638,  638,
 /*    50 */   638,  638,  638,  638,  638,  638,  751,  638,  638,  638,
 /*    60 */   638,  638,  638,  751,   -6,   -6,  203,  262,  502,  203,
 /*    70 */   751,  -24,   11,   -6,  455,  675,  502,  751,  751,  751,
 /*    80 */   751,  751,  751,  751,    6,  -44,   17,  188,  278,  297,
 /*    90 */   112,   79,    6,  169,  225,  286,  307,   17,   -6,  370,
 /*   100 */   401,   -6,  467,  467,   -6,  483,  233,  342,  361,  380,
 /*   110 */   413,  413,  413,  413,  440,  457,  -10,   66,   66,   66,
 /*   120 */    66,   66,   66,  195,  195,  195,  195,  476,   87,  450,
 /*   130 */   116,   93,   34,  508,  542,  511,  497,  492,  521,  516,
 /*   140 */   499,  524,  532,  536,  518,  531,  525,  539,  527,  549,
 /*   150 */   550,  565,  560,  583,  551,  577,  569,  572,  578,  551,
 /*   160 */   551,  551,  551,  551,  551,  551,  551,  551,  551,  551,
 /*   170 */   551,  551,  588,  589,  596,  597,  586,  604,
};
#define YY_REDUCE_USE_DFLT (-69)
#define YY_REDUCE_MAX 105
static const short yy_reduce_ofst[] = {
 /*     0 */   -45,  500,  567,  592,   75,  519,  605,  616,  641,  652,
 /*    10 */   665,  677,  689,  713,  725,  733,  769,  775,  781,  817,
 /*    20 */   823,  843,  867,  879,  887,  -68,  423,   -4,  -40,  235,
 /*    30 */    38,  762,  890,  915,  923,  926,  932,  935,  943,  952,
 /*    40 */   971,  976,  979,  988,  996,  999, 1021, 1024, 1032, 1041,
 /*    50 */  1044, 1049, 1068, 1077, 1085, 1088,  309,   37,  388,  672,
 /*    60 */   722,  757,  805,   23,  137,  241,   -5,  234,  344,  316,
 /*    70 */   234,  392,  396,  449,  -29,   13,   24,   61,  163,  242,
 /*    80 */   311,  346,  432,  456,  144,  101,  187,   67,  152,    9,
 /*    90 */    31,   89,  170,  191,  240,  269,  335,  324,  343,  383,
 /*   100 */   418,  400,  442,  444,  426,  462,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   319,  282,  418,  418,  419,  282,  429,  429,  429,  429,
 /*    10 */   429,  429,  429,  429,  429,  429,  429,  429,  429,  429,
 /*    20 */   429,  429,  429,  429,  282,  282,  416,  429,  425,  425,
 /*    30 */   427,  429,  429,  429,  429,  429,  429,  429,  429,  429,
 /*    40 */   429,  429,  429,  429,  429,  429,  429,  429,  429,  429,
 /*    50 */   429,  429,  429,  429,  429,  429,  429,  429,  429,  429,
 /*    60 */   429,  429,  429,  415,  429,  429,  333,  429,  420,  333,
 /*    70 */   429,  429,  429,  415,  414,  349,  429,  429,  429,  429,
 /*    80 */   429,  429,  429,  429,  429,  429,  429,  412,  429,  429,
 /*    90 */   429,  429,  415,  429,  429,  414,  414,  415,  429,  362,
 /*   100 */   414,  429,  414,  414,  429,  429,  412,  429,  429,  429,
 /*   110 */   407,  413,  417,  412,  403,  402,  429,  396,  397,  398,
 /*   120 */   399,  400,  401,  392,  393,  394,  395,  423,  377,  429,
 /*   130 */   429,  429,  429,  429,  429,  429,  429,  429,  429,  429,
 /*   140 */   349,  429,  429,  429,  429,  429,  352,  429,  404,  429,
 /*   150 */   429,  429,  429,  343,  344,  429,  429,  429,  429,  284,
 /*   160 */   285,  286,  287,  288,  289,  290,  291,  292,  293,  294,
 /*   170 */   295,  296,  429,  307,  429,  429,  429,  429,  320,  321,
 /*   180 */   322,  335,  336,  337,  334,  323,  324,  339,  340,  345,
 /*   190 */   346,  348,  422,  350,  359,  331,  332,  358,  360,  357,
 /*   200 */   351,  353,  361,  354,  426,  355,  356,  363,  424,  364,
 /*   210 */   428,  365,  366,  367,  368,  369,  370,  371,  372,  378,
 /*   220 */   379,  380,  381,  382,  383,  384,  387,  388,  389,  390,
 /*   230 */   391,  373,  374,  375,  376,  405,  406,  408,  409,  410,
 /*   240 */   411,  386,  385,  347,  338,  325,  326,  342,  421,  423,
 /*   250 */   341,  327,  328,  280,  281,  297,  298,  299,  300,  301,
 /*   260 */   302,  303,  304,  305,  306,  308,  310,  309,  311,  312,
 /*   270 */   313,  315,  316,  317,  314,  318,  329,  330,  415,
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
  "DOT",           "STRUCT",        "ELLIPSIS",      "COMMA",       
  "INT",           "FLOAT",         "CHAR",          "NOT",         
  "error",         "program",       "decl_list",     "osemi",       
  "stmt_list",     "stmt",          "s_stmt",        "expr",        
  "expr_list",     "iexpr_list",    "decl",          "block_stmt",  
  "oexpr",         "sw_block_stmt",  "sw_case_list",  "sw_case",     
  "ostmt_list",    "import_spec",   "import_spec_list",  "type_spec",   
  "type_spec_list",  "value_spec",    "value_spec_list",  "ident",       
  "oargs_comma_list",  "func_results",  "type",          "field_comma_list",
  "ident_list",    "otype",         "ofield_semi_list_and_osemi",  "nametype_list",
  "args_comma_list",  "ofunc_ellipsis",  "field_semi_list",  "field",       
  "nametype",      "basic_lit",     "pexpr",         "uexpr",       
  "compound_lit",  "type_expr",     "iexpr",       
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
 /*  50 */ "decl ::= FUNC ident LPAREN oargs_comma_list RPAREN func_results SEMICOLON",
 /*  51 */ "decl ::= FUNC ident LPAREN oargs_comma_list RPAREN func_results block_stmt",
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
 /*  72 */ "type ::= FUNC LPAREN oargs_comma_list RPAREN func_results",
 /*  73 */ "field_comma_list ::= nametype_list",
 /*  74 */ "args_comma_list ::= nametype_list ofunc_ellipsis",
 /*  75 */ "args_comma_list ::= ELLIPSIS",
 /*  76 */ "field_semi_list ::= field",
 /*  77 */ "field_semi_list ::= field_semi_list SEMICOLON field",
 /*  78 */ "nametype_list ::= nametype",
 /*  79 */ "nametype_list ::= nametype_list COMMA nametype",
 /*  80 */ "nametype ::= ident type",
 /*  81 */ "nametype ::= type",
 /*  82 */ "ofunc_ellipsis ::= COMMA ELLIPSIS",
 /*  83 */ "ofunc_ellipsis ::=",
 /*  84 */ "field ::= ident_list type",
 /*  85 */ "field ::= type",
 /*  86 */ "basic_lit ::= INT",
 /*  87 */ "basic_lit ::= FLOAT",
 /*  88 */ "basic_lit ::= STRING",
 /*  89 */ "basic_lit ::= CHAR",
 /*  90 */ "pexpr ::= basic_lit",
 /*  91 */ "pexpr ::= ident",
 /*  92 */ "pexpr ::= LPAREN expr RPAREN",
 /*  93 */ "pexpr ::= pexpr LSB expr RSB",
 /*  94 */ "pexpr ::= pexpr DOT IDENT",
 /*  95 */ "pexpr ::= pexpr DOT LPAREN type RPAREN",
 /*  96 */ "pexpr ::= pexpr LPAREN RPAREN",
 /*  97 */ "pexpr ::= pexpr LPAREN iexpr_list RPAREN",
 /*  98 */ "uexpr ::= pexpr",
 /*  99 */ "uexpr ::= PLUS uexpr",
 /* 100 */ "uexpr ::= MINUS uexpr",
 /* 101 */ "uexpr ::= NOT uexpr",
 /* 102 */ "uexpr ::= AND uexpr",
 /* 103 */ "uexpr ::= TIMES uexpr",
 /* 104 */ "uexpr ::= XOR uexpr",
 /* 105 */ "expr ::= uexpr",
 /* 106 */ "expr ::= expr DIVIDE expr",
 /* 107 */ "expr ::= expr TIMES expr",
 /* 108 */ "expr ::= expr MOD expr",
 /* 109 */ "expr ::= expr SHIFTL expr",
 /* 110 */ "expr ::= expr SHIFTR expr",
 /* 111 */ "expr ::= expr AND expr",
 /* 112 */ "expr ::= expr ANDNOT expr",
 /* 113 */ "expr ::= expr PLUS expr",
 /* 114 */ "expr ::= expr MINUS expr",
 /* 115 */ "expr ::= expr OR expr",
 /* 116 */ "expr ::= expr XOR expr",
 /* 117 */ "expr ::= expr EQ expr",
 /* 118 */ "expr ::= expr NEQ expr",
 /* 119 */ "expr ::= expr LT expr",
 /* 120 */ "expr ::= expr LE expr",
 /* 121 */ "expr ::= expr GT expr",
 /* 122 */ "expr ::= expr GE expr",
 /* 123 */ "expr ::= expr ANDAND expr",
 /* 124 */ "expr ::= expr OROR expr",
 /* 125 */ "compound_lit ::= LCURLY iexpr_list RCURLY",
 /* 126 */ "compound_lit ::= LCURLY iexpr_list RCURLY DOT LPAREN type RPAREN",
 /* 127 */ "type_expr ::= TYPE type",
 /* 128 */ "iexpr ::= expr",
 /* 129 */ "iexpr ::= compound_lit",
 /* 130 */ "iexpr ::= type_expr",
 /* 131 */ "iexpr_list ::= iexpr",
 /* 132 */ "iexpr_list ::= iexpr_list COMMA iexpr",
 /* 133 */ "expr_list ::= expr",
 /* 134 */ "expr_list ::= expr_list COMMA expr",
 /* 135 */ "osemi ::=",
 /* 136 */ "osemi ::= SEMICOLON",
 /* 137 */ "oexpr ::=",
 /* 138 */ "oexpr ::= expr",
 /* 139 */ "ostmt_list ::=",
 /* 140 */ "ostmt_list ::= stmt_list",
 /* 141 */ "otype ::=",
 /* 142 */ "otype ::= type",
 /* 143 */ "ident ::= IDENT",
 /* 144 */ "ident_list ::= ident",
 /* 145 */ "ident_list ::= ident_list COMMA ident",
 /* 146 */ "oargs_comma_list ::=",
 /* 147 */ "oargs_comma_list ::= args_comma_list",
 /* 148 */ "ofield_semi_list_and_osemi ::=",
 /* 149 */ "ofield_semi_list_and_osemi ::= field_semi_list osemi",
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
    case 62: /* ELLIPSIS */
    case 63: /* COMMA */
    case 64: /* INT */
    case 65: /* FLOAT */
    case 66: /* CHAR */
    case 67: /* NOT */
{
#line 2 "parser.y"
 delete (yypminor->yy0); 
#line 905 "parser.c"
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
  { 95, 1 },
  { 100, 2 },
  { 100, 1 },
  { 102, 1 },
  { 102, 3 },
  { 99, 1 },
  { 99, 3 },
  { 104, 2 },
  { 104, 1 },
  { 101, 2 },
  { 101, 0 },
  { 103, 2 },
  { 103, 1 },
  { 105, 1 },
  { 105, 1 },
  { 105, 1 },
  { 105, 1 },
  { 106, 1 },
  { 106, 1 },
  { 106, 3 },
  { 106, 4 },
  { 106, 3 },
  { 106, 5 },
  { 106, 3 },
  { 106, 4 },
  { 107, 1 },
  { 107, 2 },
  { 107, 2 },
  { 107, 2 },
  { 107, 2 },
  { 107, 2 },
  { 107, 2 },
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
  { 108, 3 },
  { 108, 7 },
  { 109, 2 },
  { 110, 1 },
  { 110, 1 },
  { 110, 1 },
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
	yygotominor.yy140 = new program_t(yymsp[-1].minor.yy168);
	ctx->ast = yygotominor.yy140;
}
#line 1344 "parser.c"
        break;
      case 1: /* stmt_list ::= stmt */
#line 44 "parser.y"
{ 
	yygotominor.yy168 = new node_vector_t;
	if (yymsp[0].minor.yy100) yygotominor.yy168->push_back(yymsp[0].minor.yy100);
}
#line 1352 "parser.c"
        break;
      case 2: /* stmt_list ::= stmt_list stmt */
#line 48 "parser.y"
{
	if (yymsp[0].minor.yy100) yymsp[-1].minor.yy168->push_back(yymsp[0].minor.yy100);
	yygotominor.yy168 = yymsp[-1].minor.yy168;
}
#line 1360 "parser.c"
        break;
      case 3: /* s_stmt ::= */
      case 137: /* oexpr ::= */ yytestcase(yyruleno==137);
      case 141: /* otype ::= */ yytestcase(yyruleno==141);
#line 61 "parser.y"
{ yygotominor.yy100 = 0; }
#line 1367 "parser.c"
        break;
      case 4: /* s_stmt ::= expr */
#line 62 "parser.y"
{ yygotominor.yy100 = new expr_stmt_t(yymsp[0].minor.yy100); }
#line 1372 "parser.c"
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
{ yygotominor.yy100 = new assign_stmt_t(yymsp[-2].minor.yy168, yymsp[0].minor.yy168, yymsp[-1].minor.yy0); }
#line 1389 "parser.c"
        break;
      case 18: /* s_stmt ::= expr INC */
      case 19: /* s_stmt ::= expr DEC */ yytestcase(yyruleno==19);
#line 76 "parser.y"
{ yygotominor.yy100 = new incdec_stmt_t(yymsp[-1].minor.yy100, yymsp[0].minor.yy0); }
#line 1395 "parser.c"
        break;
      case 20: /* stmt ::= s_stmt SEMICOLON */
#line 87 "parser.y"
{ yygotominor.yy100 = yymsp[-1].minor.yy100;   yy_destructor(yypParser,35,&yymsp[0].minor);
}
#line 1401 "parser.c"
        break;
      case 21: /* stmt ::= decl */
#line 88 "parser.y"
{ yygotominor.yy100 = new decl_stmt_t(yymsp[0].minor.yy100); }
#line 1406 "parser.c"
        break;
      case 22: /* stmt ::= block_stmt */
#line 89 "parser.y"
{ yygotominor.yy100 = yymsp[0].minor.yy42; }
#line 1411 "parser.c"
        break;
      case 23: /* stmt ::= BREAK */
      case 24: /* stmt ::= CONTINUE */ yytestcase(yyruleno==24);
      case 25: /* stmt ::= FALLTHROUGH */ yytestcase(yyruleno==25);
#line 90 "parser.y"
{ yygotominor.yy100 = new flow_stmt_t(yymsp[0].minor.yy0); }
#line 1418 "parser.c"
        break;
      case 26: /* stmt ::= RETURN iexpr_list SEMICOLON */
#line 93 "parser.y"
{ yygotominor.yy100 = new return_stmt_t(yymsp[-1].minor.yy168, yymsp[-2].minor.yy0);   yy_destructor(yypParser,35,&yymsp[0].minor);
}
#line 1424 "parser.c"
        break;
      case 27: /* stmt ::= RETURN SEMICOLON */
#line 94 "parser.y"
{ yygotominor.yy100 = new return_stmt_t(0, yymsp[-1].minor.yy0);   yy_destructor(yypParser,35,&yymsp[0].minor);
}
#line 1430 "parser.c"
        break;
      case 28: /* stmt ::= IF expr block_stmt */
#line 96 "parser.y"
{
	yygotominor.yy100 = new ifelse_stmt_t(yymsp[-1].minor.yy100, yymsp[0].minor.yy42, yymsp[-2].minor.yy0);
}
#line 1437 "parser.c"
        break;
      case 29: /* stmt ::= IF expr block_stmt ELSE stmt */
#line 99 "parser.y"
{
	yygotominor.yy100 = new ifelse_stmt_t(yymsp[-3].minor.yy100, yymsp[-2].minor.yy42, yymsp[-4].minor.yy0, yymsp[-1].minor.yy0, yymsp[0].minor.yy100);
}
#line 1444 "parser.c"
        break;
      case 30: /* stmt ::= FOR s_stmt SEMICOLON oexpr SEMICOLON s_stmt block_stmt */
#line 102 "parser.y"
{
	yygotominor.yy100 = new for_stmt_t(yymsp[-5].minor.yy100, yymsp[-3].minor.yy100, yymsp[-1].minor.yy100, yymsp[0].minor.yy42, yymsp[-6].minor.yy0);
  yy_destructor(yypParser,35,&yymsp[-4].minor);
  yy_destructor(yypParser,35,&yymsp[-2].minor);
}
#line 1453 "parser.c"
        break;
      case 31: /* stmt ::= FOR expr block_stmt */
#line 105 "parser.y"
{
	yygotominor.yy100 = new for_stmt_t(0, yymsp[-1].minor.yy100, 0, yymsp[0].minor.yy42, yymsp[-2].minor.yy0);
}
#line 1460 "parser.c"
        break;
      case 32: /* stmt ::= FOR block_stmt */
#line 108 "parser.y"
{
	yygotominor.yy100 = new for_stmt_t(0, 0, 0, yymsp[0].minor.yy42, yymsp[-1].minor.yy0);
}
#line 1467 "parser.c"
        break;
      case 33: /* stmt ::= SWITCH expr sw_block_stmt */
#line 111 "parser.y"
{
	yygotominor.yy100 = new switch_stmt_t(yymsp[-1].minor.yy100, yymsp[0].minor.yy42, yymsp[-2].minor.yy0);
}
#line 1474 "parser.c"
        break;
      case 34: /* sw_block_stmt ::= LCURLY sw_case_list RCURLY */
      case 39: /* block_stmt ::= LCURLY ostmt_list RCURLY */ yytestcase(yyruleno==39);
#line 123 "parser.y"
{
	yygotominor.yy42 = new block_stmt_t(yymsp[-1].minor.yy168, yymsp[-2].minor.yy0, yymsp[0].minor.yy0);
}
#line 1482 "parser.c"
        break;
      case 35: /* sw_case_list ::= sw_case */
#line 128 "parser.y"
{ yygotominor.yy168 = new node_vector_t(1, yymsp[0].minor.yy188); }
#line 1487 "parser.c"
        break;
      case 36: /* sw_case_list ::= sw_case_list sw_case */
#line 129 "parser.y"
{ yymsp[-1].minor.yy168->push_back(yymsp[0].minor.yy188); yygotominor.yy168 = yymsp[-1].minor.yy168; }
#line 1492 "parser.c"
        break;
      case 37: /* sw_case ::= CASE expr_list COLON ostmt_list */
#line 132 "parser.y"
{
	yygotominor.yy188 = new switch_stmt_case_t(yymsp[-2].minor.yy168, yymsp[0].minor.yy168, yymsp[-3].minor.yy0, yymsp[-1].minor.yy0);
}
#line 1499 "parser.c"
        break;
      case 38: /* sw_case ::= DEFAULT COLON ostmt_list */
#line 135 "parser.y"
{
	yygotominor.yy188 = new switch_stmt_case_t(0, yymsp[0].minor.yy168, yymsp[-2].minor.yy0, yymsp[-1].minor.yy0);
}
#line 1506 "parser.c"
        break;
      case 40: /* decl_list ::= */
#line 159 "parser.y"
{ yygotominor.yy168 = new node_vector_t; }
#line 1511 "parser.c"
        break;
      case 41: /* decl_list ::= decl_list decl */
#line 160 "parser.y"
{ yymsp[-1].minor.yy168->push_back(yymsp[0].minor.yy100); yygotominor.yy168 = yymsp[-1].minor.yy168; }
#line 1516 "parser.c"
        break;
      case 42: /* decl ::= IMPORT import_spec SEMICOLON */
#line 176 "parser.y"
{
	yygotominor.yy100 = new import_decl_t(yymsp[-1].minor.yy161, yymsp[-2].minor.yy0);
  yy_destructor(yypParser,35,&yymsp[0].minor);
}
#line 1524 "parser.c"
        break;
      case 43: /* decl ::= IMPORT LPAREN import_spec_list osemi RPAREN */
#line 179 "parser.y"
{
	yygotominor.yy100 = new import_decl_t(yymsp[-2].minor.yy183, yymsp[-4].minor.yy0, yymsp[-3].minor.yy0, yymsp[0].minor.yy0);
}
#line 1531 "parser.c"
        break;
      case 44: /* decl ::= TYPE type_spec SEMICOLON */
#line 182 "parser.y"
{
	yygotominor.yy100 = new type_decl_t(yymsp[-1].minor.yy54, yymsp[-2].minor.yy0);
  yy_destructor(yypParser,35,&yymsp[0].minor);
}
#line 1539 "parser.c"
        break;
      case 45: /* decl ::= TYPE LPAREN type_spec_list osemi RPAREN */
#line 185 "parser.y"
{
	yygotominor.yy100 = new type_decl_t(yymsp[-2].minor.yy198, yymsp[-4].minor.yy0, yymsp[-3].minor.yy0, yymsp[0].minor.yy0);
}
#line 1546 "parser.c"
        break;
      case 46: /* decl ::= CONST value_spec SEMICOLON */
#line 188 "parser.y"
{
	const_decl_t *d = new const_decl_t(yymsp[-1].minor.yy165, yymsp[-2].minor.yy0);
	syntax_check_const_decl(ctx->diag, d);
	yygotominor.yy100 = d;
  yy_destructor(yypParser,35,&yymsp[0].minor);
}
#line 1556 "parser.c"
        break;
      case 47: /* decl ::= CONST LPAREN value_spec_list osemi RPAREN */
#line 193 "parser.y"
{
	const_decl_t *d = new const_decl_t(yymsp[-2].minor.yy166, yymsp[-4].minor.yy0, yymsp[-3].minor.yy0, yymsp[0].minor.yy0);
	syntax_check_const_decl(ctx->diag, d);
	yygotominor.yy100 = d;
}
#line 1565 "parser.c"
        break;
      case 48: /* decl ::= VAR value_spec SEMICOLON */
#line 198 "parser.y"
{
	var_decl_t *d = new var_decl_t(yymsp[-1].minor.yy165, yymsp[-2].minor.yy0);
	syntax_check_var_decl(ctx->diag, d);
	yygotominor.yy100 = d;
  yy_destructor(yypParser,35,&yymsp[0].minor);
}
#line 1575 "parser.c"
        break;
      case 49: /* decl ::= VAR LPAREN value_spec_list osemi RPAREN */
#line 203 "parser.y"
{
	var_decl_t *d = new var_decl_t(yymsp[-2].minor.yy166, yymsp[-4].minor.yy0, yymsp[-3].minor.yy0, yymsp[0].minor.yy0);
	syntax_check_var_decl(ctx->diag, d);
	yygotominor.yy100 = d;
}
#line 1584 "parser.c"
        break;
      case 50: /* decl ::= FUNC ident LPAREN oargs_comma_list RPAREN func_results SEMICOLON */
#line 211 "parser.y"
{
	func_type_t *ftype = new func_type_t(yymsp[-3].minor.yy210, yymsp[-1].minor.yy210, yymsp[-6].minor.yy0);
	yygotominor.yy100 = new func_decl_t(yymsp[-5].minor.yy196, ftype);
  yy_destructor(yypParser,50,&yymsp[-4].minor);
  yy_destructor(yypParser,51,&yymsp[-2].minor);
  yy_destructor(yypParser,35,&yymsp[0].minor);
}
#line 1595 "parser.c"
        break;
      case 51: /* decl ::= FUNC ident LPAREN oargs_comma_list RPAREN func_results block_stmt */
#line 218 "parser.y"
{
	func_type_t *ftype = new func_type_t(yymsp[-3].minor.yy210, yymsp[-1].minor.yy210, yymsp[-6].minor.yy0);
	yygotominor.yy100 = new func_decl_t(yymsp[-5].minor.yy196, ftype, yymsp[0].minor.yy42);
  yy_destructor(yypParser,50,&yymsp[-4].minor);
  yy_destructor(yypParser,51,&yymsp[-2].minor);
}
#line 1605 "parser.c"
        break;
      case 52: /* func_results ::= type */
#line 231 "parser.y"
{
	field_t *f = new field_t(0, yymsp[0].minor.yy100);
	yygotominor.yy210 = new field_vector_t(1, f);
}
#line 1613 "parser.c"
        break;
      case 53: /* func_results ::= LPAREN field_comma_list RPAREN */
#line 235 "parser.y"
{ yygotominor.yy210 = yymsp[-1].minor.yy210;   yy_destructor(yypParser,50,&yymsp[-2].minor);
  yy_destructor(yypParser,51,&yymsp[0].minor);
}
#line 1620 "parser.c"
        break;
      case 54: /* func_results ::= */
      case 146: /* oargs_comma_list ::= */ yytestcase(yyruleno==146);
      case 148: /* ofield_semi_list_and_osemi ::= */ yytestcase(yyruleno==148);
#line 236 "parser.y"
{ yygotominor.yy210 = 0; }
#line 1627 "parser.c"
        break;
      case 55: /* import_spec_list ::= import_spec */
#line 249 "parser.y"
{ yygotominor.yy183 = new import_spec_vector_t(1, yymsp[0].minor.yy161); }
#line 1632 "parser.c"
        break;
      case 56: /* import_spec_list ::= import_spec_list SEMICOLON import_spec */
#line 250 "parser.y"
{
	yymsp[-2].minor.yy183->push_back(yymsp[0].minor.yy161); yygotominor.yy183 = yymsp[-2].minor.yy183;
  yy_destructor(yypParser,35,&yymsp[-1].minor);
}
#line 1640 "parser.c"
        break;
      case 57: /* import_spec ::= IDENT STRING */
#line 255 "parser.y"
{ yygotominor.yy161 = new import_spec_t(yymsp[-1].minor.yy0, yymsp[0].minor.yy0); }
#line 1645 "parser.c"
        break;
      case 58: /* import_spec ::= STRING */
#line 256 "parser.y"
{ yygotominor.yy161 = new import_spec_t(0, yymsp[0].minor.yy0); }
#line 1650 "parser.c"
        break;
      case 59: /* type_spec_list ::= type_spec */
#line 269 "parser.y"
{ yygotominor.yy198 = new type_spec_vector_t(1, yymsp[0].minor.yy54); }
#line 1655 "parser.c"
        break;
      case 60: /* type_spec_list ::= type_spec_list SEMICOLON type_spec */
#line 270 "parser.y"
{
	yymsp[-2].minor.yy198->push_back(yymsp[0].minor.yy54); yygotominor.yy198 = yymsp[-2].minor.yy198;
  yy_destructor(yypParser,35,&yymsp[-1].minor);
}
#line 1663 "parser.c"
        break;
      case 61: /* type_spec ::= IDENT type */
#line 275 "parser.y"
{ yygotominor.yy54 = new type_spec_t(yymsp[-1].minor.yy0, yymsp[0].minor.yy100); }
#line 1668 "parser.c"
        break;
      case 62: /* value_spec_list ::= value_spec */
#line 290 "parser.y"
{ yygotominor.yy166 = new value_spec_vector_t(1, yymsp[0].minor.yy165); }
#line 1673 "parser.c"
        break;
      case 63: /* value_spec_list ::= value_spec_list SEMICOLON value_spec */
#line 291 "parser.y"
{
	yymsp[-2].minor.yy166->push_back(yymsp[0].minor.yy165); yygotominor.yy166 = yymsp[-2].minor.yy166;
  yy_destructor(yypParser,35,&yymsp[-1].minor);
}
#line 1681 "parser.c"
        break;
      case 64: /* value_spec ::= ident_list otype */
#line 296 "parser.y"
{
	yygotominor.yy165 = new value_spec_t(yymsp[-1].minor.yy214, yymsp[0].minor.yy100);
}
#line 1688 "parser.c"
        break;
      case 65: /* value_spec ::= ident_list otype ASSIGN iexpr_list */
#line 299 "parser.y"
{
	yygotominor.yy165 = new value_spec_t(yymsp[-3].minor.yy214, yymsp[-2].minor.yy100, yymsp[0].minor.yy168, yymsp[-1].minor.yy0);
}
#line 1695 "parser.c"
        break;
      case 66: /* type ::= TIMES type */
#line 320 "parser.y"
{ yygotominor.yy100 = new pointer_type_t(yymsp[0].minor.yy100, yymsp[-1].minor.yy0); }
#line 1700 "parser.c"
        break;
      case 67: /* type ::= LSB expr RSB type */
#line 321 "parser.y"
{ yygotominor.yy100 = new array_type_t(yymsp[-2].minor.yy100, yymsp[0].minor.yy100, yymsp[-3].minor.yy0, yymsp[-1].minor.yy0); }
#line 1705 "parser.c"
        break;
      case 68: /* type ::= LSB RSB type */
#line 322 "parser.y"
{ yygotominor.yy100 = new array_type_t(0, yymsp[0].minor.yy100, yymsp[-2].minor.yy0, yymsp[-1].minor.yy0); }
#line 1710 "parser.c"
        break;
      case 69: /* type ::= ident DOT ident */
#line 323 "parser.y"
{ yygotominor.yy100 = new selector_expr_t(yymsp[-2].minor.yy196, yymsp[0].minor.yy196);   yy_destructor(yypParser,60,&yymsp[-1].minor);
}
#line 1716 "parser.c"
        break;
      case 70: /* type ::= ident */
      case 91: /* pexpr ::= ident */ yytestcase(yyruleno==91);
#line 324 "parser.y"
{ yygotominor.yy100 = yymsp[0].minor.yy196; }
#line 1722 "parser.c"
        break;
      case 71: /* type ::= STRUCT LCURLY ofield_semi_list_and_osemi RCURLY */
#line 325 "parser.y"
{
	yygotominor.yy100 = new struct_type_t(yymsp[-1].minor.yy210, yymsp[-3].minor.yy0, yymsp[-2].minor.yy0, yymsp[0].minor.yy0);	
}
#line 1729 "parser.c"
        break;
      case 72: /* type ::= FUNC LPAREN oargs_comma_list RPAREN func_results */
#line 328 "parser.y"
{
	yygotominor.yy100 = new func_type_t(yymsp[-2].minor.yy210, yymsp[0].minor.yy210, yymsp[-4].minor.yy0);
  yy_destructor(yypParser,50,&yymsp[-3].minor);
  yy_destructor(yypParser,51,&yymsp[-1].minor);
}
#line 1738 "parser.c"
        break;
      case 73: /* field_comma_list ::= nametype_list */
#line 343 "parser.y"
{
	yygotominor.yy210 = nametypev_to_fieldv(ctx->diag, yymsp[0].minor.yy21, false);
}
#line 1745 "parser.c"
        break;
      case 74: /* args_comma_list ::= nametype_list ofunc_ellipsis */
#line 348 "parser.y"
{
	yygotominor.yy210 = nametypev_to_fieldv(ctx->diag, yymsp[-1].minor.yy21, yymsp[0].minor.yy215);
}
#line 1752 "parser.c"
        break;
      case 75: /* args_comma_list ::= ELLIPSIS */
#line 351 "parser.y"
{
	yygotominor.yy210 = nametypev_to_fieldv(ctx->diag, new nametype_vector_t, true);
  yy_destructor(yypParser,62,&yymsp[0].minor);
}
#line 1760 "parser.c"
        break;
      case 76: /* field_semi_list ::= field */
#line 356 "parser.y"
{ yygotominor.yy210 = new field_vector_t(1, yymsp[0].minor.yy32); }
#line 1765 "parser.c"
        break;
      case 77: /* field_semi_list ::= field_semi_list SEMICOLON field */
#line 357 "parser.y"
{
	yymsp[-2].minor.yy210->push_back(yymsp[0].minor.yy32); yygotominor.yy210 = yymsp[-2].minor.yy210;
  yy_destructor(yypParser,35,&yymsp[-1].minor);
}
#line 1773 "parser.c"
        break;
      case 78: /* nametype_list ::= nametype */
#line 363 "parser.y"
{ yygotominor.yy21 = new nametype_vector_t(1, yymsp[0].minor.yy13); }
#line 1778 "parser.c"
        break;
      case 79: /* nametype_list ::= nametype_list COMMA nametype */
#line 364 "parser.y"
{
	yymsp[-2].minor.yy21->push_back(yymsp[0].minor.yy13); yygotominor.yy21 = yymsp[-2].minor.yy21;
  yy_destructor(yypParser,63,&yymsp[-1].minor);
}
#line 1786 "parser.c"
        break;
      case 80: /* nametype ::= ident type */
#line 369 "parser.y"
{ yygotominor.yy13.name = yymsp[-1].minor.yy196; yygotominor.yy13.type = yymsp[0].minor.yy100; }
#line 1791 "parser.c"
        break;
      case 81: /* nametype ::= type */
#line 370 "parser.y"
{ yygotominor.yy13.name = 0; yygotominor.yy13.type = yymsp[0].minor.yy100; }
#line 1796 "parser.c"
        break;
      case 82: /* ofunc_ellipsis ::= COMMA ELLIPSIS */
#line 380 "parser.y"
{ yygotominor.yy215 = true;   yy_destructor(yypParser,63,&yymsp[-1].minor);
  yy_destructor(yypParser,62,&yymsp[0].minor);
}
#line 1803 "parser.c"
        break;
      case 83: /* ofunc_ellipsis ::= */
#line 381 "parser.y"
{ yygotominor.yy215 = false; }
#line 1808 "parser.c"
        break;
      case 84: /* field ::= ident_list type */
#line 394 "parser.y"
{ yygotominor.yy32 = new field_t(yymsp[-1].minor.yy214, yymsp[0].minor.yy100); }
#line 1813 "parser.c"
        break;
      case 85: /* field ::= type */
#line 395 "parser.y"
{ yygotominor.yy32 = new field_t(0, yymsp[0].minor.yy100); }
#line 1818 "parser.c"
        break;
      case 86: /* basic_lit ::= INT */
      case 87: /* basic_lit ::= FLOAT */ yytestcase(yyruleno==87);
      case 88: /* basic_lit ::= STRING */ yytestcase(yyruleno==88);
      case 89: /* basic_lit ::= CHAR */ yytestcase(yyruleno==89);
#line 406 "parser.y"
{ yygotominor.yy126 = new basic_lit_expr_t(yymsp[0].minor.yy0); }
#line 1826 "parser.c"
        break;
      case 90: /* pexpr ::= basic_lit */
#line 422 "parser.y"
{ yygotominor.yy100 = yymsp[0].minor.yy126; }
#line 1831 "parser.c"
        break;
      case 92: /* pexpr ::= LPAREN expr RPAREN */
#line 424 "parser.y"
{ yygotominor.yy100 = new paren_expr_t(yymsp[-1].minor.yy100, yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1836 "parser.c"
        break;
      case 93: /* pexpr ::= pexpr LSB expr RSB */
#line 425 "parser.y"
{ yygotominor.yy100 = new index_expr_t(yymsp[-3].minor.yy100, yymsp[-1].minor.yy100, yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1841 "parser.c"
        break;
      case 94: /* pexpr ::= pexpr DOT IDENT */
#line 428 "parser.y"
{
	yygotominor.yy100 = new selector_expr_t(yymsp[-2].minor.yy100, new ident_expr_t(yymsp[0].minor.yy0));
  yy_destructor(yypParser,60,&yymsp[-1].minor);
}
#line 1849 "parser.c"
        break;
      case 95: /* pexpr ::= pexpr DOT LPAREN type RPAREN */
#line 433 "parser.y"
{
	yygotominor.yy100 = new type_cast_expr_t(yymsp[-4].minor.yy100, yymsp[-1].minor.yy100, yymsp[-3].minor.yy0, yymsp[-2].minor.yy0, yymsp[0].minor.yy0);
}
#line 1856 "parser.c"
        break;
      case 96: /* pexpr ::= pexpr LPAREN RPAREN */
#line 439 "parser.y"
{ yygotominor.yy100 = new call_expr_t(yymsp[-2].minor.yy100, 0, yymsp[-1].minor.yy0, yymsp[0].minor.yy0); }
#line 1861 "parser.c"
        break;
      case 97: /* pexpr ::= pexpr LPAREN iexpr_list RPAREN */
#line 442 "parser.y"
{
	yygotominor.yy100 = new call_expr_t(yymsp[-3].minor.yy100, yymsp[-1].minor.yy168, yymsp[-2].minor.yy0, yymsp[0].minor.yy0);
}
#line 1868 "parser.c"
        break;
      case 98: /* uexpr ::= pexpr */
      case 105: /* expr ::= uexpr */ yytestcase(yyruleno==105);
      case 128: /* iexpr ::= expr */ yytestcase(yyruleno==128);
      case 129: /* iexpr ::= compound_lit */ yytestcase(yyruleno==129);
      case 130: /* iexpr ::= type_expr */ yytestcase(yyruleno==130);
      case 138: /* oexpr ::= expr */ yytestcase(yyruleno==138);
      case 142: /* otype ::= type */ yytestcase(yyruleno==142);
#line 456 "parser.y"
{ yygotominor.yy100 = yymsp[0].minor.yy100; }
#line 1879 "parser.c"
        break;
      case 99: /* uexpr ::= PLUS uexpr */
      case 100: /* uexpr ::= MINUS uexpr */ yytestcase(yyruleno==100);
      case 101: /* uexpr ::= NOT uexpr */ yytestcase(yyruleno==101);
      case 102: /* uexpr ::= AND uexpr */ yytestcase(yyruleno==102);
      case 103: /* uexpr ::= TIMES uexpr */ yytestcase(yyruleno==103);
      case 104: /* uexpr ::= XOR uexpr */ yytestcase(yyruleno==104);
#line 457 "parser.y"
{ yygotominor.yy100 = new unary_expr_t(yymsp[0].minor.yy100, yymsp[-1].minor.yy0); }
#line 1889 "parser.c"
        break;
      case 106: /* expr ::= expr DIVIDE expr */
      case 107: /* expr ::= expr TIMES expr */ yytestcase(yyruleno==107);
      case 108: /* expr ::= expr MOD expr */ yytestcase(yyruleno==108);
      case 109: /* expr ::= expr SHIFTL expr */ yytestcase(yyruleno==109);
      case 110: /* expr ::= expr SHIFTR expr */ yytestcase(yyruleno==110);
      case 111: /* expr ::= expr AND expr */ yytestcase(yyruleno==111);
      case 112: /* expr ::= expr ANDNOT expr */ yytestcase(yyruleno==112);
      case 113: /* expr ::= expr PLUS expr */ yytestcase(yyruleno==113);
      case 114: /* expr ::= expr MINUS expr */ yytestcase(yyruleno==114);
      case 115: /* expr ::= expr OR expr */ yytestcase(yyruleno==115);
      case 116: /* expr ::= expr XOR expr */ yytestcase(yyruleno==116);
      case 117: /* expr ::= expr EQ expr */ yytestcase(yyruleno==117);
      case 118: /* expr ::= expr NEQ expr */ yytestcase(yyruleno==118);
      case 119: /* expr ::= expr LT expr */ yytestcase(yyruleno==119);
      case 120: /* expr ::= expr LE expr */ yytestcase(yyruleno==120);
      case 121: /* expr ::= expr GT expr */ yytestcase(yyruleno==121);
      case 122: /* expr ::= expr GE expr */ yytestcase(yyruleno==122);
      case 123: /* expr ::= expr ANDAND expr */ yytestcase(yyruleno==123);
      case 124: /* expr ::= expr OROR expr */ yytestcase(yyruleno==124);
#line 475 "parser.y"
{ yygotominor.yy100 = new binary_expr_t(yymsp[-2].minor.yy100, yymsp[0].minor.yy100, yymsp[-1].minor.yy0); }
#line 1912 "parser.c"
        break;
      case 125: /* compound_lit ::= LCURLY iexpr_list RCURLY */
#line 506 "parser.y"
{
	yygotominor.yy100 = new compound_lit_t(yymsp[-1].minor.yy168, 0, yymsp[-2].minor.yy0, yymsp[0].minor.yy0);
}
#line 1919 "parser.c"
        break;
      case 126: /* compound_lit ::= LCURLY iexpr_list RCURLY DOT LPAREN type RPAREN */
#line 509 "parser.y"
{
	yygotominor.yy100 = new compound_lit_t(yymsp[-5].minor.yy168, yymsp[-1].minor.yy100, yymsp[-6].minor.yy0, yymsp[0].minor.yy0);
  yy_destructor(yypParser,45,&yymsp[-4].minor);
  yy_destructor(yypParser,60,&yymsp[-3].minor);
  yy_destructor(yypParser,50,&yymsp[-2].minor);
}
#line 1929 "parser.c"
        break;
      case 127: /* type_expr ::= TYPE type */
#line 523 "parser.y"
{ yygotominor.yy100 = new type_expr_t(yymsp[0].minor.yy100, yymsp[-1].minor.yy0); }
#line 1934 "parser.c"
        break;
      case 131: /* iexpr_list ::= iexpr */
      case 133: /* expr_list ::= expr */ yytestcase(yyruleno==133);
#line 539 "parser.y"
{ yygotominor.yy168 = new node_vector_t(1, yymsp[0].minor.yy100); }
#line 1940 "parser.c"
        break;
      case 132: /* iexpr_list ::= iexpr_list COMMA iexpr */
      case 134: /* expr_list ::= expr_list COMMA expr */ yytestcase(yyruleno==134);
#line 540 "parser.y"
{ yymsp[-2].minor.yy168->push_back(yymsp[0].minor.yy100); yygotominor.yy168 = yymsp[-2].minor.yy168;   yy_destructor(yypParser,63,&yymsp[-1].minor);
}
#line 1947 "parser.c"
        break;
      case 136: /* osemi ::= SEMICOLON */
#line 555 "parser.y"
{
  yy_destructor(yypParser,35,&yymsp[0].minor);
}
#line 1954 "parser.c"
        break;
      case 139: /* ostmt_list ::= */
#line 564 "parser.y"
{ yygotominor.yy168 = 0; }
#line 1959 "parser.c"
        break;
      case 140: /* ostmt_list ::= stmt_list */
#line 565 "parser.y"
{ yygotominor.yy168 = yymsp[0].minor.yy168; }
#line 1964 "parser.c"
        break;
      case 143: /* ident ::= IDENT */
#line 574 "parser.y"
{ yygotominor.yy196 = new ident_expr_t(yymsp[0].minor.yy0); }
#line 1969 "parser.c"
        break;
      case 144: /* ident_list ::= ident */
#line 578 "parser.y"
{ yygotominor.yy214 = new ident_expr_vector_t(1, yymsp[0].minor.yy196); }
#line 1974 "parser.c"
        break;
      case 145: /* ident_list ::= ident_list COMMA ident */
#line 579 "parser.y"
{ yymsp[-2].minor.yy214->push_back(yymsp[0].minor.yy196); yygotominor.yy214 = yymsp[-2].minor.yy214;   yy_destructor(yypParser,63,&yymsp[-1].minor);
}
#line 1980 "parser.c"
        break;
      case 147: /* oargs_comma_list ::= args_comma_list */
#line 584 "parser.y"
{ yygotominor.yy210 = yymsp[0].minor.yy210; }
#line 1985 "parser.c"
        break;
      case 149: /* ofield_semi_list_and_osemi ::= field_semi_list osemi */
#line 589 "parser.y"
{ yygotominor.yy210 = yymsp[-1].minor.yy210; }
#line 1990 "parser.c"
        break;
      default:
      /* (135) osemi ::= */ yytestcase(yyruleno==135);
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
#line 2061 "parser.c"
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
