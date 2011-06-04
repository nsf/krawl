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
#define YYNOCODE 113
#define YYACTIONTYPE unsigned short int
#define ParseTOKENTYPE  token_t* 
typedef union {
  int yyinit;
  ParseTOKENTYPE yy0;
  node_vector_t* yy38;
  program_t* yy62;
  import_spec_vector_t* yy69;
  import_spec_t* yy79;
  basic_lit_expr_t* yy82;
  nametype_vector_t* yy87;
  bool yy89;
  type_spec_t* yy90;
  ident_expr_t* yy92;
  nametype_t yy103;
  field_vector_t* yy104;
  switch_stmt_case_t* yy138;
  value_spec_t* yy157;
  type_spec_vector_t* yy164;
  field_t* yy174;
  value_spec_vector_t* yy187;
  node_t* yy196;
  block_stmt_t* yy200;
  ident_expr_vector_t* yy222;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define ParseARG_SDECL  parser_t *ctx ;
#define ParseARG_PDECL , parser_t *ctx 
#define ParseARG_FETCH  parser_t *ctx  = yypParser->ctx 
#define ParseARG_STORE yypParser->ctx  = ctx 
#define YYNSTATE 283
#define YYNRULE 151
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
 /*     0 */    58,   59,   27,   63,  179,   62,  106,  107,  117,   61,
 /*    10 */    10,   11,   12,   13,   14,   15,   16,   17,   18,   19,
 /*    20 */    20,   21,   22,  220,  435,   77,   65,  264,  265,  266,
 /*    30 */     7,   55,  195,   24,   56,    1,  423,  219,  129,  229,
 /*    40 */    91,   34,  135,   94,   72,   73,  105,  195,  217,  181,
 /*    50 */   138,  186,   75,  147,   54,  202,  215,  216,  218,   60,
 /*    60 */   100,  208,   66,  187,   96,  203,   58,   59,  195,   63,
 /*    70 */   195,   62,  114,  134,    2,   61,   42,   43,   44,   45,
 /*    80 */    33,   35,   37,   38,   39,   40,   41,  141,  220,  204,
 /*    90 */   199,   54,  286,  264,  265,  266,    7,   55,   79,   24,
 /*   100 */    56,    1,  219,  129,  229,  141,   91,   34,  198,   94,
 /*   110 */    72,   73,  105,  195,  217,   33,   35,   37,   38,   39,
 /*   120 */    40,   41,  215,  216,  218,   60,  128,  274,   27,  213,
 /*   130 */   179,   76,   58,   59,   78,   63,  238,   62,  210,  146,
 /*   140 */   195,   61,   32,   92,  278,  142,  144,  207,  270,   23,
 /*   150 */   258,  175,  107,  117,  141,  262,  263,  211,   87,  264,
 /*   160 */   265,  266,    7,   55,   78,   24,   56,    1,  220,  248,
 /*   170 */    97,  220,   91,   34,  176,   94,   72,   73,  105,  195,
 /*   180 */   217,  273,  219,  129,  229,  219,  129,  223,  215,  216,
 /*   190 */   218,   60,   53,   52,   46,   47,   48,   49,   50,   51,
 /*   200 */    42,   43,   44,   45,   33,   35,   37,   38,   39,   40,
 /*   210 */    41,  275,  141,  280,  282,  191,   79,   75,   58,   59,
 /*   220 */   202,   63,    1,   62,  259,  260,  287,   61,   91,  136,
 /*   230 */   201,   94,   72,   73,  105,    1,  184,   53,   52,   46,
 /*   240 */    47,   48,   49,   50,   51,   42,   43,   44,   45,   33,
 /*   250 */    35,   37,   38,   39,   40,   41,  139,  146,  195,   34,
 /*   260 */    32,   81,   99,  142,  144,  195,  217,  235,   84,  259,
 /*   270 */   260,  287,  281,  137,  215,  216,  218,   60,  141,   79,
 /*   280 */   287,  192,   53,   52,   46,   47,   48,   49,   50,   51,
 /*   290 */    42,   43,   44,   45,   33,   35,   37,   38,   39,   40,
 /*   300 */    41,   53,   52,   46,   47,   48,   49,   50,   51,   42,
 /*   310 */    43,   44,   45,   33,   35,   37,   38,   39,   40,   41,
 /*   320 */   146,  195,  151,   32,   93,    1,  142,  144,   98,  102,
 /*   330 */    75,  161,   75,  202,  141,  202,  148,  193,  100,  208,
 /*   340 */   149,   23,  140,  203,   86,  203,   53,   52,   46,   47,
 /*   350 */    48,   49,   50,   51,   42,   43,   44,   45,   33,   35,
 /*   360 */    37,   38,   39,   40,   41,   53,   52,   46,   47,   48,
 /*   370 */    49,   50,   51,   42,   43,   44,   45,   33,   35,   37,
 /*   380 */    38,   39,   40,   41,   53,   52,   46,   47,   48,   49,
 /*   390 */    50,   51,   42,   43,   44,   45,   33,   35,   37,   38,
 /*   400 */    39,   40,   41,  194,   80,  254,  103,  253,  267,  254,
 /*   410 */   104,  253,   69,  190,   68,  221,   69,   53,   52,   46,
 /*   420 */    47,   48,   49,   50,   51,   42,   43,   44,   45,   33,
 /*   430 */    35,   37,   38,   39,   40,   41,  141,   23,  141,  150,
 /*   440 */   141,  252,  222,  153,  156,   52,   46,   47,   48,   49,
 /*   450 */    50,   51,   42,   43,   44,   45,   33,   35,   37,   38,
 /*   460 */    39,   40,   41,   46,   47,   48,   49,   50,   51,   42,
 /*   470 */    43,   44,   45,   33,   35,   37,   38,   39,   40,   41,
 /*   480 */    58,   59,  220,   63,  141,   62,   95,  199,  205,   61,
 /*   490 */    64,  220,  214,  353,  212,  220,  219,  129,  224,   58,
 /*   500 */    59,   74,   63,  353,   62,  219,  129,  225,   61,  219,
 /*   510 */   129,  226,  154,  141,  253,    8,  240,  141,   99,   69,
 /*   520 */   247,   34,  237,   83,  155,  268,   85,  195,  217,  111,
 /*   530 */   160,  131,  138,  186,    8,  159,  215,  216,  218,   60,
 /*   540 */    34,  158,   83,  253,    6,  220,  195,  217,   69,    1,
 /*   550 */   271,  283,   36,  182,  130,  215,  216,  218,   60,  219,
 /*   560 */   129,  229,  241,  242,  243,   58,   59,  183,   63,  185,
 /*   570 */    62,  188,  189,   99,   61,   30,    4,  257,  175,  107,
 /*   580 */   117,   31,  262,  263,   58,   59,  196,   63,  180,   62,
 /*   590 */   197,   67,   28,   61,  200,  220,  269,  175,  107,  117,
 /*   600 */     8,  262,  263,   71,  111,  236,   34,  152,   83,  219,
 /*   610 */   129,  229,  195,  217,  220,   82,  249,  239,  250,    1,
 /*   620 */   220,  215,  216,  218,   60,   34,    9,   23,  219,  129,
 /*   630 */   229,  195,  217,  255,  219,  129,  229,  241,  242,  244,
 /*   640 */   215,  216,  218,   60,    4,  257,  175,  107,  117,   29,
 /*   650 */   262,  263,   58,   59,  256,   63,  276,   62,   70,  261,
 /*   660 */     5,   61,   26,  220,    3,   25,  279,  436,  436,    4,
 /*   670 */   257,  175,  107,  117,  436,  262,  263,  219,  129,  229,
 /*   680 */   436,  277,  436,  436,  251,  111,  253,  133,  220,  436,
 /*   690 */   436,   69,  436,   34,  436,  436,  111,  436,  132,  195,
 /*   700 */   217,  220,  219,  129,  229,  111,  436,  157,  215,  216,
 /*   710 */   218,   60,  220,  436,  436,  219,  129,  229,  241,  242,
 /*   720 */   243,  220,  436,  436,  436,  436,  219,  129,  229,  241,
 /*   730 */   242,  243,  111,  436,  162,  219,  129,  229,  241,  242,
 /*   740 */   243,  436,  436,  436,  436,  436,  436,  436,  220,  436,
 /*   750 */   436,  436,  111,  436,  163,   79,  220,  436,  111,  436,
 /*   760 */   164,  436,  219,  129,  229,  241,  242,  243,  220,  436,
 /*   770 */   219,  129,  227,  436,  220,   79,  436,  436,  111,  436,
 /*   780 */   165,  436,  219,  129,  229,  241,  242,  243,  219,  129,
 /*   790 */   229,  241,  242,  243,  220,  436,  146,  195,  111,   32,
 /*   800 */   166,  436,  142,  144,  111,  102,  167,  436,  219,  129,
 /*   810 */   229,  241,  242,  243,  220,  436,  146,  195,  436,   32,
 /*   820 */   220,   79,  142,  144,  206,  111,  436,  168,  219,  129,
 /*   830 */   229,  241,  242,  243,  219,  129,  229,  241,  242,  243,
 /*   840 */   436,  220,  436,  436,  436,  111,  436,  169,  436,  436,
 /*   850 */   436,  111,  436,  170,  436,  219,  129,  229,  241,  242,
 /*   860 */   243,  220,  146,  195,  436,   32,  436,  220,  142,  144,
 /*   870 */   436,  111,  436,  171,  436,  219,  129,  229,  241,  242,
 /*   880 */   243,  219,  129,  229,  241,  242,  243,  220,  436,  436,
 /*   890 */   436,  111,  436,  172,  436,  220,  436,  111,  436,  173,
 /*   900 */   436,  219,  129,  229,  241,  242,  243,  220,  436,  219,
 /*   910 */   129,  228,  436,  220,  436,  436,  436,  436,  111,   79,
 /*   920 */   174,  219,  129,  229,  241,  242,  243,  219,  129,  229,
 /*   930 */   241,  242,  243,  436,  220,  436,  436,  177,   88,  117,
 /*   940 */   436,  436,  272,  436,  436,  436,  436,  436,  219,  129,
 /*   950 */   229,  241,  242,  243,  220,   57,  436,  113,  436,  436,
 /*   960 */   146,  195,  178,   32,  436,  436,  142,  144,  219,  129,
 /*   970 */   229,  128,  436,  220,  213,  436,   76,  436,  143,  436,
 /*   980 */   108,  436,  101,  209,  246,  436,  436,  219,  129,  229,
 /*   990 */   128,  436,  436,  213,  436,   76,  220,  145,  436,  436,
 /*  1000 */   220,  101,  209,  109,  436,  436,  245,  436,  436,  110,
 /*  1010 */   219,  129,  229,  436,  219,  129,  229,  436,  436,  220,
 /*  1020 */   436,  436,  220,  436,  436,  220,  230,  436,  436,  231,
 /*  1030 */   436,  436,  232,  219,  129,  229,  219,  129,  229,  219,
 /*  1040 */   129,  229,  220,  436,  436,  220,  436,  436,  220,  436,
 /*  1050 */   233,  436,  436,  234,  436,  436,  219,  129,  229,  219,
 /*  1060 */   129,  229,  219,  129,  229,  436,  220,  436,  436,  220,
 /*  1070 */   124,  436,  436,  125,  436,  436,  436,  126,  436,  436,
 /*  1080 */   219,  129,  229,  219,  129,  229,  220,  436,  436,  220,
 /*  1090 */   436,  436,  436,  220,  436,  436,  127,  436,  436,  118,
 /*  1100 */   219,  129,  229,  219,  129,  229,  119,  219,  129,  229,
 /*  1110 */   436,  436,  220,  436,  436,  220,  120,  436,  436,  121,
 /*  1120 */   436,  436,  220,  436,  436,  122,  219,  129,  229,  219,
 /*  1130 */   129,  229,  220,  436,  436,  220,  219,  129,  229,  436,
 /*  1140 */   436,  220,  436,  123,  436,  436,  219,  129,  229,  219,
 /*  1150 */   129,  229,  436,  436,  116,  219,  129,  229,  436,  220,
 /*  1160 */   115,  436,  436,  112,  436,  436,   89,  436,  436,  436,
 /*  1170 */   220,  436,  436,  219,  129,  229,  220,  436,  436,  220,
 /*  1180 */   436,  436,  220,   90,  219,  129,  229,  436,  436,  436,
 /*  1190 */   219,  129,  229,  219,  129,  229,  219,  129,  229,  220,
 /*  1200 */   436,  436,  436,  436,  436,  436,  436,  436,  436,  436,
 /*  1210 */   436,  436,  436,  219,  129,  229,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */     9,   10,   46,   12,   48,   14,   75,   76,   77,   18,
 /*    10 */    20,   21,   22,   23,   24,   25,   26,   27,   28,   29,
 /*    20 */    30,   31,   32,   92,   70,   71,   50,   36,   37,   38,
 /*    30 */    39,   40,   56,   42,   43,   44,   45,  106,  107,  108,
 /*    40 */    49,   50,   72,   52,   53,   54,   55,   56,   57,   79,
 /*    50 */    56,   57,   92,   93,   64,   95,   65,   66,   67,   68,
 /*    60 */   100,  101,   50,   86,   87,  105,    9,   10,   56,   12,
 /*    70 */    56,   14,   76,   77,   47,   18,    9,   10,   11,   12,
 /*    80 */    13,   14,   15,   16,   17,   18,   19,   92,   92,   94,
 /*    90 */    95,   64,   35,   36,   37,   38,   39,   40,   14,   42,
 /*   100 */    43,   44,  106,  107,  108,   92,   49,   50,   95,   52,
 /*   110 */    53,   54,   55,   56,   57,   13,   14,   15,   16,   17,
 /*   120 */    18,   19,   65,   66,   67,   68,   92,   45,   46,   95,
 /*   130 */    48,   97,    9,   10,   56,   12,   51,   14,  104,   55,
 /*   140 */    56,   18,   58,   83,   84,   61,   62,   63,   80,   64,
 /*   150 */    74,   75,   76,   77,   92,   79,   80,   95,   50,   36,
 /*   160 */    37,   38,   39,   40,   56,   42,   43,   44,   92,   88,
 /*   170 */    89,   92,   49,   50,   80,   52,   53,   54,   55,   56,
 /*   180 */    57,   82,  106,  107,  108,  106,  107,  108,   65,   66,
 /*   190 */    67,   68,    1,    2,    3,    4,    5,    6,    7,    8,
 /*   200 */     9,   10,   11,   12,   13,   14,   15,   16,   17,   18,
 /*   210 */    19,   84,   92,   35,   35,   95,   14,   92,    9,   10,
 /*   220 */    95,   12,   44,   14,   33,   34,   35,   18,   49,   86,
 /*   230 */   105,   52,   53,   54,   55,   44,   86,    1,    2,    3,
 /*   240 */     4,    5,    6,    7,    8,    9,   10,   11,   12,   13,
 /*   250 */    14,   15,   16,   17,   18,   19,   88,   55,   56,   50,
 /*   260 */    58,   50,   60,   61,   62,   56,   57,   56,   59,   33,
 /*   270 */    34,   35,   80,   72,   65,   66,   67,   68,   92,   14,
 /*   280 */    44,   95,    1,    2,    3,    4,    5,    6,    7,    8,
 /*   290 */     9,   10,   11,   12,   13,   14,   15,   16,   17,   18,
 /*   300 */    19,    1,    2,    3,    4,    5,    6,    7,    8,    9,
 /*   310 */    10,   11,   12,   13,   14,   15,   16,   17,   18,   19,
 /*   320 */    55,   56,   45,   58,   35,   44,   61,   62,   35,   64,
 /*   330 */    92,   93,   92,   95,   92,   95,   96,   95,  100,  101,
 /*   340 */   100,   64,   72,  105,   44,  105,    1,    2,    3,    4,
 /*   350 */     5,    6,    7,    8,    9,   10,   11,   12,   13,   14,
 /*   360 */    15,   16,   17,   18,   19,    1,    2,    3,    4,    5,
 /*   370 */     6,    7,    8,    9,   10,   11,   12,   13,   14,   15,
 /*   380 */    16,   17,   18,   19,    1,    2,    3,    4,    5,    6,
 /*   390 */     7,    8,    9,   10,   11,   12,   13,   14,   15,   16,
 /*   400 */    17,   18,   19,   92,   59,   90,   91,   92,   35,   90,
 /*   410 */    91,   92,   97,   88,   64,   51,   97,    1,    2,    3,
 /*   420 */     4,    5,    6,    7,    8,    9,   10,   11,   12,   13,
 /*   430 */    14,   15,   16,   17,   18,   19,   92,   64,   92,   95,
 /*   440 */    92,   95,   59,   95,   98,    2,    3,    4,    5,    6,
 /*   450 */     7,    8,    9,   10,   11,   12,   13,   14,   15,   16,
 /*   460 */    17,   18,   19,    3,    4,    5,    6,    7,    8,    9,
 /*   470 */    10,   11,   12,   13,   14,   15,   16,   17,   18,   19,
 /*   480 */     9,   10,   92,   12,   92,   14,   94,   95,  102,   18,
 /*   490 */    35,   92,   72,   35,   92,   92,  106,  107,  108,    9,
 /*   500 */    10,   35,   12,   45,   14,  106,  107,  108,   18,  106,
 /*   510 */   107,  108,   90,   92,   92,   44,   95,   92,   60,   97,
 /*   520 */    95,   50,   51,   52,   72,   35,   50,   56,   57,   76,
 /*   530 */    92,   78,   56,   57,   44,   72,   65,   66,   67,   68,
 /*   540 */    50,   90,   52,   92,   50,   92,   56,   57,   97,   44,
 /*   550 */    80,    0,   58,   35,   60,   65,   66,   67,   68,  106,
 /*   560 */   107,  108,  109,  110,  111,    9,   10,   51,   12,   57,
 /*   570 */    14,   35,   51,   60,   18,   44,   73,   74,   75,   76,
 /*   580 */    77,   44,   79,   80,    9,   10,   45,   12,   85,   14,
 /*   590 */    45,   51,   50,   18,   51,   92,   74,   75,   76,   77,
 /*   600 */    44,   79,   80,   64,   76,   51,   50,   60,   52,  106,
 /*   610 */   107,  108,   56,   57,   92,   50,   35,   51,   51,   44,
 /*   620 */    92,   65,   66,   67,   68,   50,   20,   64,  106,  107,
 /*   630 */   108,   56,   57,   35,  106,  107,  108,  109,  110,  111,
 /*   640 */    65,   66,   67,   68,   73,   74,   75,   76,   77,   50,
 /*   650 */    79,   80,    9,   10,   51,   12,   85,   14,   51,   35,
 /*   660 */    41,   18,   35,   92,   47,   35,   45,  112,  112,   73,
 /*   670 */    74,   75,   76,   77,  112,   79,   80,  106,  107,  108,
 /*   680 */   112,   85,  112,  112,   90,   76,   92,   78,   92,  112,
 /*   690 */   112,   97,  112,   50,  112,  112,   76,  112,   78,   56,
 /*   700 */    57,   92,  106,  107,  108,   76,  112,   78,   65,   66,
 /*   710 */    67,   68,   92,  112,  112,  106,  107,  108,  109,  110,
 /*   720 */   111,   92,  112,  112,  112,  112,  106,  107,  108,  109,
 /*   730 */   110,  111,   76,  112,   78,  106,  107,  108,  109,  110,
 /*   740 */   111,  112,  112,  112,  112,  112,  112,  112,   92,  112,
 /*   750 */   112,  112,   76,  112,   78,   14,   92,  112,   76,  112,
 /*   760 */    78,  112,  106,  107,  108,  109,  110,  111,   92,  112,
 /*   770 */   106,  107,  108,  112,   92,   14,  112,  112,   76,  112,
 /*   780 */    78,  112,  106,  107,  108,  109,  110,  111,  106,  107,
 /*   790 */   108,  109,  110,  111,   92,  112,   55,   56,   76,   58,
 /*   800 */    78,  112,   61,   62,   76,   64,   78,  112,  106,  107,
 /*   810 */   108,  109,  110,  111,   92,  112,   55,   56,  112,   58,
 /*   820 */    92,   14,   61,   62,   63,   76,  112,   78,  106,  107,
 /*   830 */   108,  109,  110,  111,  106,  107,  108,  109,  110,  111,
 /*   840 */   112,   92,  112,  112,  112,   76,  112,   78,  112,  112,
 /*   850 */   112,   76,  112,   78,  112,  106,  107,  108,  109,  110,
 /*   860 */   111,   92,   55,   56,  112,   58,  112,   92,   61,   62,
 /*   870 */   112,   76,  112,   78,  112,  106,  107,  108,  109,  110,
 /*   880 */   111,  106,  107,  108,  109,  110,  111,   92,  112,  112,
 /*   890 */   112,   76,  112,   78,  112,   92,  112,   76,  112,   78,
 /*   900 */   112,  106,  107,  108,  109,  110,  111,   92,  112,  106,
 /*   910 */   107,  108,  112,   92,  112,  112,  112,  112,   76,   14,
 /*   920 */    78,  106,  107,  108,  109,  110,  111,  106,  107,  108,
 /*   930 */   109,  110,  111,  112,   92,  112,  112,   75,   76,   77,
 /*   940 */   112,  112,   80,  112,  112,  112,  112,  112,  106,  107,
 /*   950 */   108,  109,  110,  111,   92,   50,  112,   76,  112,  112,
 /*   960 */    55,   56,   81,   58,  112,  112,   61,   62,  106,  107,
 /*   970 */   108,   92,  112,   92,   95,  112,   97,  112,   99,  112,
 /*   980 */    76,  112,  103,  104,   76,  112,  112,  106,  107,  108,
 /*   990 */    92,  112,  112,   95,  112,   97,   92,   99,  112,  112,
 /*  1000 */    92,  103,  104,   76,  112,  112,   76,  112,  112,   76,
 /*  1010 */   106,  107,  108,  112,  106,  107,  108,  112,  112,   92,
 /*  1020 */   112,  112,   92,  112,  112,   92,   76,  112,  112,   76,
 /*  1030 */   112,  112,   76,  106,  107,  108,  106,  107,  108,  106,
 /*  1040 */   107,  108,   92,  112,  112,   92,  112,  112,   92,  112,
 /*  1050 */    76,  112,  112,   76,  112,  112,  106,  107,  108,  106,
 /*  1060 */   107,  108,  106,  107,  108,  112,   92,  112,  112,   92,
 /*  1070 */    76,  112,  112,   76,  112,  112,  112,   76,  112,  112,
 /*  1080 */   106,  107,  108,  106,  107,  108,   92,  112,  112,   92,
 /*  1090 */   112,  112,  112,   92,  112,  112,   76,  112,  112,   76,
 /*  1100 */   106,  107,  108,  106,  107,  108,   76,  106,  107,  108,
 /*  1110 */   112,  112,   92,  112,  112,   92,   76,  112,  112,   76,
 /*  1120 */   112,  112,   92,  112,  112,   76,  106,  107,  108,  106,
 /*  1130 */   107,  108,   92,  112,  112,   92,  106,  107,  108,  112,
 /*  1140 */   112,   92,  112,   76,  112,  112,  106,  107,  108,  106,
 /*  1150 */   107,  108,  112,  112,   76,  106,  107,  108,  112,   92,
 /*  1160 */    76,  112,  112,   76,  112,  112,   76,  112,  112,  112,
 /*  1170 */    92,  112,  112,  106,  107,  108,   92,  112,  112,   92,
 /*  1180 */   112,  112,   92,   76,  106,  107,  108,  112,  112,  112,
 /*  1190 */   106,  107,  108,  106,  107,  108,  106,  107,  108,   92,
 /*  1200 */   112,  112,  112,  112,  112,  112,  112,  112,  112,  112,
 /*  1210 */   112,  112,  112,  106,  107,  108,
};
#define YY_SHIFT_USE_DFLT (-45)
#define YY_SHIFT_MAX 180
static const short yy_shift_ofst[] = {
 /*     0 */   -45,   -9,   57,   57,   57,  123,  471,  490,  556,  556,
 /*    10 */   556,  556,  556,  556,  556,  556,  556,  556,  556,  556,
 /*    20 */   556,  556,  556,  556,  575,  643,  643,  643,   84,   84,
 /*    30 */   807,  807,  209,  643,  643,  643,  643,  643,  643,  643,
 /*    40 */   643,  643,  643,  643,  643,  643,  643,  643,  643,  643,
 /*    50 */   643,  643,  643,  643,  643,  643,  643,  807,  643,  643,
 /*    60 */   643,  643,  643,  643,  807,   14,   14,  905,  761,  741,
 /*    70 */   905,  807,  -24,   12,   14,  202,  265,  179,  807,  807,
 /*    80 */   807,  807,  807,  807,  807,   -6,  -44,   78,  191,  281,
 /*    90 */   300,  476,   82,   -6,  108,  178,  289,  293,   78,   14,
 /*   100 */   350,  455,   14,  466,  466,   14,  505,  236,  345,  364,
 /*   110 */   383,  416,  416,  416,  416,  443,  460,  -10,   67,   67,
 /*   120 */    67,   67,   67,   67,  102,  102,  102,  102,  458,  494,
 /*   130 */   211,   85,  277,  373,   27,  551,  518,  516,  512,  536,
 /*   140 */   521,  513,  531,  541,  537,  545,  542,  540,  543,  539,
 /*   150 */   554,  547,  565,  566,  581,  567,  606,  563,  598,  603,
 /*   160 */   599,  607,  563,  563,  563,  563,  563,  563,  563,  563,
 /*   170 */   563,  563,  563,  563,  563,  624,  619,  627,  630,  617,
 /*   180 */   621,
};
#define YY_REDUCE_USE_DFLT (-70)
#define YY_REDUCE_MAX 106
static const short yy_reduce_ofst[] = {
 /*     0 */   -46,  503,  571,  596,   76,  522,  453,  609,  620,  629,
 /*    10 */   656,  676,  682,  702,  722,  728,  749,  769,  775,  795,
 /*    20 */   815,  821,  842,  528,  862,  -69,  881,   -4,  -40,  238,
 /*    30 */   879,  898,  904,  908,  927,  930,  933,  950,  953,  956,
 /*    40 */   974,  977,  994,  997, 1001, 1020, 1023, 1030, 1040, 1043,
 /*    50 */  1049, 1067, 1078, 1084, 1087, 1090, 1107,  240,   79,  390,
 /*    60 */   399,  403,  664,  803,   34,  315,  319,   -5,  125,  346,
 /*    70 */   392,  125,  422,  451,  594,   13,   62,  -30,  120,  186,
 /*    80 */   242,  344,  348,  421,  425,  -23,   60,   81,   68,   94,
 /*    90 */    99,  143,  127,  150,  168,  192,  201,  270,  325,  311,
 /*   100 */   386,  420,  402,  452,  463,  438,  470,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   323,  286,  423,  423,  424,  286,  434,  434,  434,  434,
 /*    10 */   434,  434,  434,  434,  434,  434,  434,  434,  434,  434,
 /*    20 */   434,  434,  434,  434,  286,  286,  421,  434,  430,  430,
 /*    30 */   432,  432,  434,  434,  434,  434,  434,  434,  434,  434,
 /*    40 */   434,  434,  434,  434,  434,  434,  434,  434,  434,  434,
 /*    50 */   434,  434,  434,  434,  434,  434,  434,  434,  434,  434,
 /*    60 */   434,  434,  434,  434,  420,  434,  434,  337,  434,  425,
 /*    70 */   337,  434,  434,  434,  420,  353,  434,  419,  434,  434,
 /*    80 */   434,  434,  434,  434,  434,  434,  434,  434,  417,  434,
 /*    90 */   434,  434,  434,  420,  434,  434,  419,  419,  420,  434,
 /*   100 */   367,  419,  434,  419,  419,  434,  434,  417,  434,  434,
 /*   110 */   434,  412,  418,  422,  417,  408,  407,  434,  401,  402,
 /*   120 */   403,  404,  405,  406,  397,  398,  399,  400,  428,  382,
 /*   130 */   434,  434,  434,  434,  434,  434,  434,  434,  434,  434,
 /*   140 */   434,  353,  434,  434,  434,  434,  434,  434,  434,  357,
 /*   150 */   434,  409,  434,  434,  434,  434,  347,  348,  434,  434,
 /*   160 */   434,  434,  288,  289,  290,  291,  292,  293,  294,  295,
 /*   170 */   296,  297,  298,  299,  300,  434,  311,  434,  434,  434,
 /*   180 */   434,  324,  325,  326,  339,  340,  341,  338,  327,  328,
 /*   190 */   343,  344,  349,  350,  352,  427,  354,  355,  364,  335,
 /*   200 */   336,  363,  365,  362,  356,  358,  366,  359,  431,  360,
 /*   210 */   361,  368,  429,  369,  433,  370,  371,  372,  373,  374,
 /*   220 */   375,  376,  377,  383,  384,  385,  386,  387,  388,  389,
 /*   230 */   392,  393,  394,  395,  396,  378,  379,  380,  381,  410,
 /*   240 */   411,  413,  414,  415,  416,  391,  390,  351,  342,  329,
 /*   250 */   330,  346,  426,  428,  345,  331,  332,  284,  285,  301,
 /*   260 */   302,  303,  304,  305,  306,  307,  308,  309,  310,  312,
 /*   270 */   314,  313,  315,  316,  317,  319,  320,  321,  318,  322,
 /*   280 */   333,  334,  420,
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
  "DOT",           "STRUCT",        "UNION",         "ELLIPSIS",    
  "COMMA",         "INT",           "FLOAT",         "CHAR",        
  "NOT",           "error",         "program",       "decl_list",   
  "osemi",         "stmt_list",     "stmt",          "s_stmt",      
  "expr",          "expr_list",     "iexpr_list",    "decl",        
  "block_stmt",    "oexpr",         "sw_block_stmt",  "sw_case_list",
  "sw_case",       "ostmt_list",    "import_spec",   "import_spec_list",
  "type_spec",     "type_spec_list",  "value_spec",    "value_spec_list",
  "ident",         "oargs_comma_list",  "func_results",  "type",        
  "field_comma_list",  "ident_list",    "otype",         "ofield_semi_list_and_osemi",
  "nametype_list",  "args_comma_list",  "ofunc_ellipsis",  "field_semi_list",
  "field",         "nametype",      "basic_lit",     "pexpr",       
  "uexpr",         "compound_lit",  "type_expr",     "iexpr",       
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
 /*  72 */ "type ::= UNION LCURLY ofield_semi_list_and_osemi RCURLY",
 /*  73 */ "type ::= FUNC LPAREN oargs_comma_list RPAREN func_results",
 /*  74 */ "field_comma_list ::= nametype_list",
 /*  75 */ "args_comma_list ::= nametype_list ofunc_ellipsis",
 /*  76 */ "args_comma_list ::= ELLIPSIS",
 /*  77 */ "field_semi_list ::= field",
 /*  78 */ "field_semi_list ::= field_semi_list SEMICOLON field",
 /*  79 */ "nametype_list ::= nametype",
 /*  80 */ "nametype_list ::= nametype_list COMMA nametype",
 /*  81 */ "nametype ::= ident type",
 /*  82 */ "nametype ::= type",
 /*  83 */ "ofunc_ellipsis ::= COMMA ELLIPSIS",
 /*  84 */ "ofunc_ellipsis ::=",
 /*  85 */ "field ::= ident_list type",
 /*  86 */ "field ::= type",
 /*  87 */ "basic_lit ::= INT",
 /*  88 */ "basic_lit ::= FLOAT",
 /*  89 */ "basic_lit ::= STRING",
 /*  90 */ "basic_lit ::= CHAR",
 /*  91 */ "pexpr ::= basic_lit",
 /*  92 */ "pexpr ::= ident",
 /*  93 */ "pexpr ::= LPAREN expr RPAREN",
 /*  94 */ "pexpr ::= pexpr LSB expr RSB",
 /*  95 */ "pexpr ::= pexpr DOT IDENT",
 /*  96 */ "pexpr ::= pexpr DOT LPAREN type RPAREN",
 /*  97 */ "pexpr ::= pexpr LPAREN RPAREN",
 /*  98 */ "pexpr ::= pexpr LPAREN iexpr_list RPAREN",
 /*  99 */ "uexpr ::= pexpr",
 /* 100 */ "uexpr ::= PLUS uexpr",
 /* 101 */ "uexpr ::= MINUS uexpr",
 /* 102 */ "uexpr ::= NOT uexpr",
 /* 103 */ "uexpr ::= AND uexpr",
 /* 104 */ "uexpr ::= TIMES uexpr",
 /* 105 */ "uexpr ::= XOR uexpr",
 /* 106 */ "expr ::= uexpr",
 /* 107 */ "expr ::= expr DIVIDE expr",
 /* 108 */ "expr ::= expr TIMES expr",
 /* 109 */ "expr ::= expr MOD expr",
 /* 110 */ "expr ::= expr SHIFTL expr",
 /* 111 */ "expr ::= expr SHIFTR expr",
 /* 112 */ "expr ::= expr AND expr",
 /* 113 */ "expr ::= expr ANDNOT expr",
 /* 114 */ "expr ::= expr PLUS expr",
 /* 115 */ "expr ::= expr MINUS expr",
 /* 116 */ "expr ::= expr OR expr",
 /* 117 */ "expr ::= expr XOR expr",
 /* 118 */ "expr ::= expr EQ expr",
 /* 119 */ "expr ::= expr NEQ expr",
 /* 120 */ "expr ::= expr LT expr",
 /* 121 */ "expr ::= expr LE expr",
 /* 122 */ "expr ::= expr GT expr",
 /* 123 */ "expr ::= expr GE expr",
 /* 124 */ "expr ::= expr ANDAND expr",
 /* 125 */ "expr ::= expr OROR expr",
 /* 126 */ "compound_lit ::= LCURLY iexpr_list RCURLY",
 /* 127 */ "compound_lit ::= LCURLY iexpr_list RCURLY DOT LPAREN type RPAREN",
 /* 128 */ "type_expr ::= TYPE type",
 /* 129 */ "iexpr ::= expr",
 /* 130 */ "iexpr ::= compound_lit",
 /* 131 */ "iexpr ::= type_expr",
 /* 132 */ "iexpr_list ::= iexpr",
 /* 133 */ "iexpr_list ::= iexpr_list COMMA iexpr",
 /* 134 */ "expr_list ::= expr",
 /* 135 */ "expr_list ::= expr_list COMMA expr",
 /* 136 */ "osemi ::=",
 /* 137 */ "osemi ::= SEMICOLON",
 /* 138 */ "oexpr ::=",
 /* 139 */ "oexpr ::= expr",
 /* 140 */ "ostmt_list ::=",
 /* 141 */ "ostmt_list ::= stmt_list",
 /* 142 */ "otype ::=",
 /* 143 */ "otype ::= type",
 /* 144 */ "ident ::= IDENT",
 /* 145 */ "ident_list ::= ident",
 /* 146 */ "ident_list ::= ident_list COMMA ident",
 /* 147 */ "oargs_comma_list ::=",
 /* 148 */ "oargs_comma_list ::= args_comma_list",
 /* 149 */ "ofield_semi_list_and_osemi ::=",
 /* 150 */ "ofield_semi_list_and_osemi ::= field_semi_list osemi",
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
    case 62: /* UNION */
    case 63: /* ELLIPSIS */
    case 64: /* COMMA */
    case 65: /* INT */
    case 66: /* FLOAT */
    case 67: /* CHAR */
    case 68: /* NOT */
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
  { 70, 2 },
  { 73, 1 },
  { 73, 2 },
  { 75, 0 },
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
  { 75, 2 },
  { 75, 2 },
  { 74, 2 },
  { 74, 1 },
  { 74, 1 },
  { 74, 1 },
  { 74, 1 },
  { 74, 1 },
  { 74, 3 },
  { 74, 2 },
  { 74, 3 },
  { 74, 5 },
  { 74, 7 },
  { 74, 3 },
  { 74, 2 },
  { 74, 3 },
  { 82, 3 },
  { 83, 1 },
  { 83, 2 },
  { 84, 4 },
  { 84, 3 },
  { 80, 3 },
  { 71, 0 },
  { 71, 2 },
  { 79, 3 },
  { 79, 5 },
  { 79, 3 },
  { 79, 5 },
  { 79, 3 },
  { 79, 5 },
  { 79, 3 },
  { 79, 5 },
  { 79, 7 },
  { 79, 7 },
  { 94, 1 },
  { 94, 3 },
  { 94, 0 },
  { 87, 1 },
  { 87, 3 },
  { 86, 2 },
  { 86, 1 },
  { 89, 1 },
  { 89, 3 },
  { 88, 2 },
  { 91, 1 },
  { 91, 3 },
  { 90, 2 },
  { 90, 4 },
  { 95, 2 },
  { 95, 4 },
  { 95, 3 },
  { 95, 3 },
  { 95, 1 },
  { 95, 4 },
  { 95, 4 },
  { 95, 5 },
  { 96, 1 },
  { 101, 2 },
  { 101, 1 },
  { 103, 1 },
  { 103, 3 },
  { 100, 1 },
  { 100, 3 },
  { 105, 2 },
  { 105, 1 },
  { 102, 2 },
  { 102, 0 },
  { 104, 2 },
  { 104, 1 },
  { 106, 1 },
  { 106, 1 },
  { 106, 1 },
  { 106, 1 },
  { 107, 1 },
  { 107, 1 },
  { 107, 3 },
  { 107, 4 },
  { 107, 3 },
  { 107, 5 },
  { 107, 3 },
  { 107, 4 },
  { 108, 1 },
  { 108, 2 },
  { 108, 2 },
  { 108, 2 },
  { 108, 2 },
  { 108, 2 },
  { 108, 2 },
  { 76, 1 },
  { 76, 3 },
  { 76, 3 },
  { 76, 3 },
  { 76, 3 },
  { 76, 3 },
  { 76, 3 },
  { 76, 3 },
  { 76, 3 },
  { 76, 3 },
  { 76, 3 },
  { 76, 3 },
  { 76, 3 },
  { 76, 3 },
  { 76, 3 },
  { 76, 3 },
  { 76, 3 },
  { 76, 3 },
  { 76, 3 },
  { 76, 3 },
  { 109, 3 },
  { 109, 7 },
  { 110, 2 },
  { 111, 1 },
  { 111, 1 },
  { 111, 1 },
  { 78, 1 },
  { 78, 3 },
  { 77, 1 },
  { 77, 3 },
  { 72, 0 },
  { 72, 1 },
  { 81, 0 },
  { 81, 1 },
  { 85, 0 },
  { 85, 1 },
  { 98, 0 },
  { 98, 1 },
  { 92, 1 },
  { 97, 1 },
  { 97, 3 },
  { 93, 0 },
  { 93, 1 },
  { 99, 0 },
  { 99, 2 },
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
	yygotominor.yy62 = new program_t(yymsp[-1].minor.yy38);
	ctx->ast = yygotominor.yy62;
}
#line 1353 "parser.c"
        break;
      case 1: /* stmt_list ::= stmt */
#line 44 "parser.y"
{ 
	yygotominor.yy38 = new node_vector_t;
	if (yymsp[0].minor.yy196) yygotominor.yy38->push_back(yymsp[0].minor.yy196);
}
#line 1361 "parser.c"
        break;
      case 2: /* stmt_list ::= stmt_list stmt */
#line 48 "parser.y"
{
	if (yymsp[0].minor.yy196) yymsp[-1].minor.yy38->push_back(yymsp[0].minor.yy196);
	yygotominor.yy38 = yymsp[-1].minor.yy38;
}
#line 1369 "parser.c"
        break;
      case 3: /* s_stmt ::= */
      case 138: /* oexpr ::= */ yytestcase(yyruleno==138);
      case 142: /* otype ::= */ yytestcase(yyruleno==142);
#line 61 "parser.y"
{ yygotominor.yy196 = 0; }
#line 1376 "parser.c"
        break;
      case 4: /* s_stmt ::= expr */
#line 62 "parser.y"
{ yygotominor.yy196 = new expr_stmt_t(yymsp[0].minor.yy196); }
#line 1381 "parser.c"
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
{ yygotominor.yy196 = new assign_stmt_t(yymsp[-2].minor.yy38, yymsp[0].minor.yy38, yymsp[-1].minor.yy0); }
#line 1398 "parser.c"
        break;
      case 18: /* s_stmt ::= expr INC */
      case 19: /* s_stmt ::= expr DEC */ yytestcase(yyruleno==19);
#line 76 "parser.y"
{ yygotominor.yy196 = new incdec_stmt_t(yymsp[-1].minor.yy196, yymsp[0].minor.yy0); }
#line 1404 "parser.c"
        break;
      case 20: /* stmt ::= s_stmt SEMICOLON */
#line 87 "parser.y"
{ yygotominor.yy196 = yymsp[-1].minor.yy196;   yy_destructor(yypParser,35,&yymsp[0].minor);
}
#line 1410 "parser.c"
        break;
      case 21: /* stmt ::= decl */
#line 88 "parser.y"
{ yygotominor.yy196 = new decl_stmt_t(yymsp[0].minor.yy196); }
#line 1415 "parser.c"
        break;
      case 22: /* stmt ::= block_stmt */
#line 89 "parser.y"
{ yygotominor.yy196 = yymsp[0].minor.yy200; }
#line 1420 "parser.c"
        break;
      case 23: /* stmt ::= BREAK */
      case 24: /* stmt ::= CONTINUE */ yytestcase(yyruleno==24);
      case 25: /* stmt ::= FALLTHROUGH */ yytestcase(yyruleno==25);
#line 90 "parser.y"
{ yygotominor.yy196 = new flow_stmt_t(yymsp[0].minor.yy0); }
#line 1427 "parser.c"
        break;
      case 26: /* stmt ::= RETURN iexpr_list SEMICOLON */
#line 93 "parser.y"
{ yygotominor.yy196 = new return_stmt_t(yymsp[-1].minor.yy38, yymsp[-2].minor.yy0);   yy_destructor(yypParser,35,&yymsp[0].minor);
}
#line 1433 "parser.c"
        break;
      case 27: /* stmt ::= RETURN SEMICOLON */
#line 94 "parser.y"
{ yygotominor.yy196 = new return_stmt_t(0, yymsp[-1].minor.yy0);   yy_destructor(yypParser,35,&yymsp[0].minor);
}
#line 1439 "parser.c"
        break;
      case 28: /* stmt ::= IF expr block_stmt */
#line 96 "parser.y"
{
	yygotominor.yy196 = new ifelse_stmt_t(yymsp[-1].minor.yy196, yymsp[0].minor.yy200, yymsp[-2].minor.yy0);
}
#line 1446 "parser.c"
        break;
      case 29: /* stmt ::= IF expr block_stmt ELSE stmt */
#line 99 "parser.y"
{
	yygotominor.yy196 = new ifelse_stmt_t(yymsp[-3].minor.yy196, yymsp[-2].minor.yy200, yymsp[-4].minor.yy0, yymsp[-1].minor.yy0, yymsp[0].minor.yy196);
}
#line 1453 "parser.c"
        break;
      case 30: /* stmt ::= FOR s_stmt SEMICOLON oexpr SEMICOLON s_stmt block_stmt */
#line 102 "parser.y"
{
	yygotominor.yy196 = new for_stmt_t(yymsp[-5].minor.yy196, yymsp[-3].minor.yy196, yymsp[-1].minor.yy196, yymsp[0].minor.yy200, yymsp[-6].minor.yy0);
  yy_destructor(yypParser,35,&yymsp[-4].minor);
  yy_destructor(yypParser,35,&yymsp[-2].minor);
}
#line 1462 "parser.c"
        break;
      case 31: /* stmt ::= FOR expr block_stmt */
#line 105 "parser.y"
{
	yygotominor.yy196 = new for_stmt_t(0, yymsp[-1].minor.yy196, 0, yymsp[0].minor.yy200, yymsp[-2].minor.yy0);
}
#line 1469 "parser.c"
        break;
      case 32: /* stmt ::= FOR block_stmt */
#line 108 "parser.y"
{
	yygotominor.yy196 = new for_stmt_t(0, 0, 0, yymsp[0].minor.yy200, yymsp[-1].minor.yy0);
}
#line 1476 "parser.c"
        break;
      case 33: /* stmt ::= SWITCH expr sw_block_stmt */
#line 111 "parser.y"
{
	yygotominor.yy196 = new switch_stmt_t(yymsp[-1].minor.yy196, yymsp[0].minor.yy200, yymsp[-2].minor.yy0);
}
#line 1483 "parser.c"
        break;
      case 34: /* sw_block_stmt ::= LCURLY sw_case_list RCURLY */
      case 39: /* block_stmt ::= LCURLY ostmt_list RCURLY */ yytestcase(yyruleno==39);
#line 123 "parser.y"
{
	yygotominor.yy200 = new block_stmt_t(yymsp[-1].minor.yy38, yymsp[-2].minor.yy0, yymsp[0].minor.yy0);
}
#line 1491 "parser.c"
        break;
      case 35: /* sw_case_list ::= sw_case */
#line 128 "parser.y"
{ yygotominor.yy38 = new node_vector_t(1, yymsp[0].minor.yy138); }
#line 1496 "parser.c"
        break;
      case 36: /* sw_case_list ::= sw_case_list sw_case */
#line 129 "parser.y"
{ yymsp[-1].minor.yy38->push_back(yymsp[0].minor.yy138); yygotominor.yy38 = yymsp[-1].minor.yy38; }
#line 1501 "parser.c"
        break;
      case 37: /* sw_case ::= CASE expr_list COLON ostmt_list */
#line 132 "parser.y"
{
	yygotominor.yy138 = new switch_stmt_case_t(yymsp[-2].minor.yy38, yymsp[0].minor.yy38, yymsp[-3].minor.yy0, yymsp[-1].minor.yy0);
}
#line 1508 "parser.c"
        break;
      case 38: /* sw_case ::= DEFAULT COLON ostmt_list */
#line 135 "parser.y"
{
	yygotominor.yy138 = new switch_stmt_case_t(0, yymsp[0].minor.yy38, yymsp[-2].minor.yy0, yymsp[-1].minor.yy0);
}
#line 1515 "parser.c"
        break;
      case 40: /* decl_list ::= */
#line 159 "parser.y"
{ yygotominor.yy38 = new node_vector_t; }
#line 1520 "parser.c"
        break;
      case 41: /* decl_list ::= decl_list decl */
#line 160 "parser.y"
{ yymsp[-1].minor.yy38->push_back(yymsp[0].minor.yy196); yygotominor.yy38 = yymsp[-1].minor.yy38; }
#line 1525 "parser.c"
        break;
      case 42: /* decl ::= IMPORT import_spec SEMICOLON */
#line 176 "parser.y"
{
	yygotominor.yy196 = new import_decl_t(yymsp[-1].minor.yy79, yymsp[-2].minor.yy0);
  yy_destructor(yypParser,35,&yymsp[0].minor);
}
#line 1533 "parser.c"
        break;
      case 43: /* decl ::= IMPORT LPAREN import_spec_list osemi RPAREN */
#line 179 "parser.y"
{
	yygotominor.yy196 = new import_decl_t(yymsp[-2].minor.yy69, yymsp[-4].minor.yy0, yymsp[-3].minor.yy0, yymsp[0].minor.yy0);
}
#line 1540 "parser.c"
        break;
      case 44: /* decl ::= TYPE type_spec SEMICOLON */
#line 182 "parser.y"
{
	yygotominor.yy196 = new type_decl_t(yymsp[-1].minor.yy90, yymsp[-2].minor.yy0);
  yy_destructor(yypParser,35,&yymsp[0].minor);
}
#line 1548 "parser.c"
        break;
      case 45: /* decl ::= TYPE LPAREN type_spec_list osemi RPAREN */
#line 185 "parser.y"
{
	yygotominor.yy196 = new type_decl_t(yymsp[-2].minor.yy164, yymsp[-4].minor.yy0, yymsp[-3].minor.yy0, yymsp[0].minor.yy0);
}
#line 1555 "parser.c"
        break;
      case 46: /* decl ::= CONST value_spec SEMICOLON */
#line 188 "parser.y"
{
	const_decl_t *d = new const_decl_t(yymsp[-1].minor.yy157, yymsp[-2].minor.yy0);
	syntax_check_const_decl(ctx->diag, d);
	yygotominor.yy196 = d;
  yy_destructor(yypParser,35,&yymsp[0].minor);
}
#line 1565 "parser.c"
        break;
      case 47: /* decl ::= CONST LPAREN value_spec_list osemi RPAREN */
#line 193 "parser.y"
{
	const_decl_t *d = new const_decl_t(yymsp[-2].minor.yy187, yymsp[-4].minor.yy0, yymsp[-3].minor.yy0, yymsp[0].minor.yy0);
	syntax_check_const_decl(ctx->diag, d);
	yygotominor.yy196 = d;
}
#line 1574 "parser.c"
        break;
      case 48: /* decl ::= VAR value_spec SEMICOLON */
#line 198 "parser.y"
{
	var_decl_t *d = new var_decl_t(yymsp[-1].minor.yy157, yymsp[-2].minor.yy0);
	syntax_check_var_decl(ctx->diag, d);
	yygotominor.yy196 = d;
  yy_destructor(yypParser,35,&yymsp[0].minor);
}
#line 1584 "parser.c"
        break;
      case 49: /* decl ::= VAR LPAREN value_spec_list osemi RPAREN */
#line 203 "parser.y"
{
	var_decl_t *d = new var_decl_t(yymsp[-2].minor.yy187, yymsp[-4].minor.yy0, yymsp[-3].minor.yy0, yymsp[0].minor.yy0);
	syntax_check_var_decl(ctx->diag, d);
	yygotominor.yy196 = d;
}
#line 1593 "parser.c"
        break;
      case 50: /* decl ::= FUNC ident LPAREN oargs_comma_list RPAREN func_results SEMICOLON */
#line 211 "parser.y"
{
	func_type_t *ftype = new func_type_t(yymsp[-3].minor.yy104, yymsp[-1].minor.yy104, yymsp[-6].minor.yy0);
	yygotominor.yy196 = new func_decl_t(yymsp[-5].minor.yy92, ftype);
  yy_destructor(yypParser,50,&yymsp[-4].minor);
  yy_destructor(yypParser,51,&yymsp[-2].minor);
  yy_destructor(yypParser,35,&yymsp[0].minor);
}
#line 1604 "parser.c"
        break;
      case 51: /* decl ::= FUNC ident LPAREN oargs_comma_list RPAREN func_results block_stmt */
#line 218 "parser.y"
{
	func_type_t *ftype = new func_type_t(yymsp[-3].minor.yy104, yymsp[-1].minor.yy104, yymsp[-6].minor.yy0);
	yygotominor.yy196 = new func_decl_t(yymsp[-5].minor.yy92, ftype, yymsp[0].minor.yy200);
  yy_destructor(yypParser,50,&yymsp[-4].minor);
  yy_destructor(yypParser,51,&yymsp[-2].minor);
}
#line 1614 "parser.c"
        break;
      case 52: /* func_results ::= type */
#line 231 "parser.y"
{
	field_t *f = new field_t(0, yymsp[0].minor.yy196);
	yygotominor.yy104 = new field_vector_t(1, f);
}
#line 1622 "parser.c"
        break;
      case 53: /* func_results ::= LPAREN field_comma_list RPAREN */
#line 235 "parser.y"
{ yygotominor.yy104 = yymsp[-1].minor.yy104;   yy_destructor(yypParser,50,&yymsp[-2].minor);
  yy_destructor(yypParser,51,&yymsp[0].minor);
}
#line 1629 "parser.c"
        break;
      case 54: /* func_results ::= */
      case 147: /* oargs_comma_list ::= */ yytestcase(yyruleno==147);
      case 149: /* ofield_semi_list_and_osemi ::= */ yytestcase(yyruleno==149);
#line 236 "parser.y"
{ yygotominor.yy104 = 0; }
#line 1636 "parser.c"
        break;
      case 55: /* import_spec_list ::= import_spec */
#line 249 "parser.y"
{ yygotominor.yy69 = new import_spec_vector_t(1, yymsp[0].minor.yy79); }
#line 1641 "parser.c"
        break;
      case 56: /* import_spec_list ::= import_spec_list SEMICOLON import_spec */
#line 250 "parser.y"
{
	yymsp[-2].minor.yy69->push_back(yymsp[0].minor.yy79); yygotominor.yy69 = yymsp[-2].minor.yy69;
  yy_destructor(yypParser,35,&yymsp[-1].minor);
}
#line 1649 "parser.c"
        break;
      case 57: /* import_spec ::= IDENT STRING */
#line 255 "parser.y"
{ yygotominor.yy79 = new import_spec_t(yymsp[-1].minor.yy0, yymsp[0].minor.yy0); }
#line 1654 "parser.c"
        break;
      case 58: /* import_spec ::= STRING */
#line 256 "parser.y"
{ yygotominor.yy79 = new import_spec_t(0, yymsp[0].minor.yy0); }
#line 1659 "parser.c"
        break;
      case 59: /* type_spec_list ::= type_spec */
#line 269 "parser.y"
{ yygotominor.yy164 = new type_spec_vector_t(1, yymsp[0].minor.yy90); }
#line 1664 "parser.c"
        break;
      case 60: /* type_spec_list ::= type_spec_list SEMICOLON type_spec */
#line 270 "parser.y"
{
	yymsp[-2].minor.yy164->push_back(yymsp[0].minor.yy90); yygotominor.yy164 = yymsp[-2].minor.yy164;
  yy_destructor(yypParser,35,&yymsp[-1].minor);
}
#line 1672 "parser.c"
        break;
      case 61: /* type_spec ::= IDENT type */
#line 275 "parser.y"
{ yygotominor.yy90 = new type_spec_t(yymsp[-1].minor.yy0, yymsp[0].minor.yy196); }
#line 1677 "parser.c"
        break;
      case 62: /* value_spec_list ::= value_spec */
#line 290 "parser.y"
{ yygotominor.yy187 = new value_spec_vector_t(1, yymsp[0].minor.yy157); }
#line 1682 "parser.c"
        break;
      case 63: /* value_spec_list ::= value_spec_list SEMICOLON value_spec */
#line 291 "parser.y"
{
	yymsp[-2].minor.yy187->push_back(yymsp[0].minor.yy157); yygotominor.yy187 = yymsp[-2].minor.yy187;
  yy_destructor(yypParser,35,&yymsp[-1].minor);
}
#line 1690 "parser.c"
        break;
      case 64: /* value_spec ::= ident_list otype */
#line 296 "parser.y"
{
	yygotominor.yy157 = new value_spec_t(yymsp[-1].minor.yy222, yymsp[0].minor.yy196);
}
#line 1697 "parser.c"
        break;
      case 65: /* value_spec ::= ident_list otype ASSIGN iexpr_list */
#line 299 "parser.y"
{
	yygotominor.yy157 = new value_spec_t(yymsp[-3].minor.yy222, yymsp[-2].minor.yy196, yymsp[0].minor.yy38, yymsp[-1].minor.yy0);
}
#line 1704 "parser.c"
        break;
      case 66: /* type ::= TIMES type */
#line 320 "parser.y"
{ yygotominor.yy196 = new pointer_type_t(yymsp[0].minor.yy196, yymsp[-1].minor.yy0); }
#line 1709 "parser.c"
        break;
      case 67: /* type ::= LSB expr RSB type */
#line 321 "parser.y"
{ yygotominor.yy196 = new array_type_t(yymsp[-2].minor.yy196, yymsp[0].minor.yy196, yymsp[-3].minor.yy0, yymsp[-1].minor.yy0); }
#line 1714 "parser.c"
        break;
      case 68: /* type ::= LSB RSB type */
#line 322 "parser.y"
{ yygotominor.yy196 = new array_type_t(0, yymsp[0].minor.yy196, yymsp[-2].minor.yy0, yymsp[-1].minor.yy0); }
#line 1719 "parser.c"
        break;
      case 69: /* type ::= ident DOT ident */
#line 323 "parser.y"
{ yygotominor.yy196 = new selector_expr_t(yymsp[-2].minor.yy92, yymsp[0].minor.yy92);   yy_destructor(yypParser,60,&yymsp[-1].minor);
}
#line 1725 "parser.c"
        break;
      case 70: /* type ::= ident */
      case 92: /* pexpr ::= ident */ yytestcase(yyruleno==92);
#line 324 "parser.y"
{ yygotominor.yy196 = yymsp[0].minor.yy92; }
#line 1731 "parser.c"
        break;
      case 71: /* type ::= STRUCT LCURLY ofield_semi_list_and_osemi RCURLY */
      case 72: /* type ::= UNION LCURLY ofield_semi_list_and_osemi RCURLY */ yytestcase(yyruleno==72);
#line 325 "parser.y"
{
	yygotominor.yy196 = new struct_type_t(yymsp[-1].minor.yy104, yymsp[-3].minor.yy0, yymsp[-2].minor.yy0, yymsp[0].minor.yy0);	
}
#line 1739 "parser.c"
        break;
      case 73: /* type ::= FUNC LPAREN oargs_comma_list RPAREN func_results */
#line 331 "parser.y"
{
	yygotominor.yy196 = new func_type_t(yymsp[-2].minor.yy104, yymsp[0].minor.yy104, yymsp[-4].minor.yy0);
  yy_destructor(yypParser,50,&yymsp[-3].minor);
  yy_destructor(yypParser,51,&yymsp[-1].minor);
}
#line 1748 "parser.c"
        break;
      case 74: /* field_comma_list ::= nametype_list */
#line 346 "parser.y"
{
	yygotominor.yy104 = nametypev_to_fieldv(ctx->diag, yymsp[0].minor.yy87, false);
}
#line 1755 "parser.c"
        break;
      case 75: /* args_comma_list ::= nametype_list ofunc_ellipsis */
#line 351 "parser.y"
{
	yygotominor.yy104 = nametypev_to_fieldv(ctx->diag, yymsp[-1].minor.yy87, yymsp[0].minor.yy89);
}
#line 1762 "parser.c"
        break;
      case 76: /* args_comma_list ::= ELLIPSIS */
#line 354 "parser.y"
{
	yygotominor.yy104 = nametypev_to_fieldv(ctx->diag, new nametype_vector_t, true);
  yy_destructor(yypParser,63,&yymsp[0].minor);
}
#line 1770 "parser.c"
        break;
      case 77: /* field_semi_list ::= field */
#line 359 "parser.y"
{ yygotominor.yy104 = new field_vector_t(1, yymsp[0].minor.yy174); }
#line 1775 "parser.c"
        break;
      case 78: /* field_semi_list ::= field_semi_list SEMICOLON field */
#line 360 "parser.y"
{
	yymsp[-2].minor.yy104->push_back(yymsp[0].minor.yy174); yygotominor.yy104 = yymsp[-2].minor.yy104;
  yy_destructor(yypParser,35,&yymsp[-1].minor);
}
#line 1783 "parser.c"
        break;
      case 79: /* nametype_list ::= nametype */
#line 366 "parser.y"
{ yygotominor.yy87 = new nametype_vector_t(1, yymsp[0].minor.yy103); }
#line 1788 "parser.c"
        break;
      case 80: /* nametype_list ::= nametype_list COMMA nametype */
#line 367 "parser.y"
{
	yymsp[-2].minor.yy87->push_back(yymsp[0].minor.yy103); yygotominor.yy87 = yymsp[-2].minor.yy87;
  yy_destructor(yypParser,64,&yymsp[-1].minor);
}
#line 1796 "parser.c"
        break;
      case 81: /* nametype ::= ident type */
#line 372 "parser.y"
{ yygotominor.yy103.name = yymsp[-1].minor.yy92; yygotominor.yy103.type = yymsp[0].minor.yy196; }
#line 1801 "parser.c"
        break;
      case 82: /* nametype ::= type */
#line 373 "parser.y"
{ yygotominor.yy103.name = 0; yygotominor.yy103.type = yymsp[0].minor.yy196; }
#line 1806 "parser.c"
        break;
      case 83: /* ofunc_ellipsis ::= COMMA ELLIPSIS */
#line 383 "parser.y"
{ yygotominor.yy89 = true;   yy_destructor(yypParser,64,&yymsp[-1].minor);
  yy_destructor(yypParser,63,&yymsp[0].minor);
}
#line 1813 "parser.c"
        break;
      case 84: /* ofunc_ellipsis ::= */
#line 384 "parser.y"
{ yygotominor.yy89 = false; }
#line 1818 "parser.c"
        break;
      case 85: /* field ::= ident_list type */
#line 397 "parser.y"
{ yygotominor.yy174 = new field_t(yymsp[-1].minor.yy222, yymsp[0].minor.yy196); }
#line 1823 "parser.c"
        break;
      case 86: /* field ::= type */
#line 398 "parser.y"
{ yygotominor.yy174 = new field_t(0, yymsp[0].minor.yy196); }
#line 1828 "parser.c"
        break;
      case 87: /* basic_lit ::= INT */
      case 88: /* basic_lit ::= FLOAT */ yytestcase(yyruleno==88);
      case 89: /* basic_lit ::= STRING */ yytestcase(yyruleno==89);
      case 90: /* basic_lit ::= CHAR */ yytestcase(yyruleno==90);
#line 409 "parser.y"
{ yygotominor.yy82 = new basic_lit_expr_t(yymsp[0].minor.yy0); }
#line 1836 "parser.c"
        break;
      case 91: /* pexpr ::= basic_lit */
#line 425 "parser.y"
{ yygotominor.yy196 = yymsp[0].minor.yy82; }
#line 1841 "parser.c"
        break;
      case 93: /* pexpr ::= LPAREN expr RPAREN */
#line 427 "parser.y"
{ yygotominor.yy196 = new paren_expr_t(yymsp[-1].minor.yy196, yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1846 "parser.c"
        break;
      case 94: /* pexpr ::= pexpr LSB expr RSB */
#line 428 "parser.y"
{ yygotominor.yy196 = new index_expr_t(yymsp[-3].minor.yy196, yymsp[-1].minor.yy196, yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1851 "parser.c"
        break;
      case 95: /* pexpr ::= pexpr DOT IDENT */
#line 431 "parser.y"
{
	yygotominor.yy196 = new selector_expr_t(yymsp[-2].minor.yy196, new ident_expr_t(yymsp[0].minor.yy0));
  yy_destructor(yypParser,60,&yymsp[-1].minor);
}
#line 1859 "parser.c"
        break;
      case 96: /* pexpr ::= pexpr DOT LPAREN type RPAREN */
#line 436 "parser.y"
{
	yygotominor.yy196 = new type_cast_expr_t(yymsp[-4].minor.yy196, yymsp[-1].minor.yy196, yymsp[-3].minor.yy0, yymsp[-2].minor.yy0, yymsp[0].minor.yy0);
}
#line 1866 "parser.c"
        break;
      case 97: /* pexpr ::= pexpr LPAREN RPAREN */
#line 442 "parser.y"
{ yygotominor.yy196 = new call_expr_t(yymsp[-2].minor.yy196, 0, yymsp[-1].minor.yy0, yymsp[0].minor.yy0); }
#line 1871 "parser.c"
        break;
      case 98: /* pexpr ::= pexpr LPAREN iexpr_list RPAREN */
#line 445 "parser.y"
{
	yygotominor.yy196 = new call_expr_t(yymsp[-3].minor.yy196, yymsp[-1].minor.yy38, yymsp[-2].minor.yy0, yymsp[0].minor.yy0);
}
#line 1878 "parser.c"
        break;
      case 99: /* uexpr ::= pexpr */
      case 106: /* expr ::= uexpr */ yytestcase(yyruleno==106);
      case 129: /* iexpr ::= expr */ yytestcase(yyruleno==129);
      case 130: /* iexpr ::= compound_lit */ yytestcase(yyruleno==130);
      case 131: /* iexpr ::= type_expr */ yytestcase(yyruleno==131);
      case 139: /* oexpr ::= expr */ yytestcase(yyruleno==139);
      case 143: /* otype ::= type */ yytestcase(yyruleno==143);
#line 459 "parser.y"
{ yygotominor.yy196 = yymsp[0].minor.yy196; }
#line 1889 "parser.c"
        break;
      case 100: /* uexpr ::= PLUS uexpr */
      case 101: /* uexpr ::= MINUS uexpr */ yytestcase(yyruleno==101);
      case 102: /* uexpr ::= NOT uexpr */ yytestcase(yyruleno==102);
      case 103: /* uexpr ::= AND uexpr */ yytestcase(yyruleno==103);
      case 104: /* uexpr ::= TIMES uexpr */ yytestcase(yyruleno==104);
      case 105: /* uexpr ::= XOR uexpr */ yytestcase(yyruleno==105);
#line 460 "parser.y"
{ yygotominor.yy196 = new unary_expr_t(yymsp[0].minor.yy196, yymsp[-1].minor.yy0); }
#line 1899 "parser.c"
        break;
      case 107: /* expr ::= expr DIVIDE expr */
      case 108: /* expr ::= expr TIMES expr */ yytestcase(yyruleno==108);
      case 109: /* expr ::= expr MOD expr */ yytestcase(yyruleno==109);
      case 110: /* expr ::= expr SHIFTL expr */ yytestcase(yyruleno==110);
      case 111: /* expr ::= expr SHIFTR expr */ yytestcase(yyruleno==111);
      case 112: /* expr ::= expr AND expr */ yytestcase(yyruleno==112);
      case 113: /* expr ::= expr ANDNOT expr */ yytestcase(yyruleno==113);
      case 114: /* expr ::= expr PLUS expr */ yytestcase(yyruleno==114);
      case 115: /* expr ::= expr MINUS expr */ yytestcase(yyruleno==115);
      case 116: /* expr ::= expr OR expr */ yytestcase(yyruleno==116);
      case 117: /* expr ::= expr XOR expr */ yytestcase(yyruleno==117);
      case 118: /* expr ::= expr EQ expr */ yytestcase(yyruleno==118);
      case 119: /* expr ::= expr NEQ expr */ yytestcase(yyruleno==119);
      case 120: /* expr ::= expr LT expr */ yytestcase(yyruleno==120);
      case 121: /* expr ::= expr LE expr */ yytestcase(yyruleno==121);
      case 122: /* expr ::= expr GT expr */ yytestcase(yyruleno==122);
      case 123: /* expr ::= expr GE expr */ yytestcase(yyruleno==123);
      case 124: /* expr ::= expr ANDAND expr */ yytestcase(yyruleno==124);
      case 125: /* expr ::= expr OROR expr */ yytestcase(yyruleno==125);
#line 478 "parser.y"
{ yygotominor.yy196 = new binary_expr_t(yymsp[-2].minor.yy196, yymsp[0].minor.yy196, yymsp[-1].minor.yy0); }
#line 1922 "parser.c"
        break;
      case 126: /* compound_lit ::= LCURLY iexpr_list RCURLY */
#line 509 "parser.y"
{
	yygotominor.yy196 = new compound_lit_t(yymsp[-1].minor.yy38, 0, yymsp[-2].minor.yy0, yymsp[0].minor.yy0);
}
#line 1929 "parser.c"
        break;
      case 127: /* compound_lit ::= LCURLY iexpr_list RCURLY DOT LPAREN type RPAREN */
#line 512 "parser.y"
{
	yygotominor.yy196 = new compound_lit_t(yymsp[-5].minor.yy38, yymsp[-1].minor.yy196, yymsp[-6].minor.yy0, yymsp[0].minor.yy0);
  yy_destructor(yypParser,45,&yymsp[-4].minor);
  yy_destructor(yypParser,60,&yymsp[-3].minor);
  yy_destructor(yypParser,50,&yymsp[-2].minor);
}
#line 1939 "parser.c"
        break;
      case 128: /* type_expr ::= TYPE type */
#line 526 "parser.y"
{ yygotominor.yy196 = new type_expr_t(yymsp[0].minor.yy196, yymsp[-1].minor.yy0); }
#line 1944 "parser.c"
        break;
      case 132: /* iexpr_list ::= iexpr */
      case 134: /* expr_list ::= expr */ yytestcase(yyruleno==134);
#line 542 "parser.y"
{ yygotominor.yy38 = new node_vector_t(1, yymsp[0].minor.yy196); }
#line 1950 "parser.c"
        break;
      case 133: /* iexpr_list ::= iexpr_list COMMA iexpr */
      case 135: /* expr_list ::= expr_list COMMA expr */ yytestcase(yyruleno==135);
#line 543 "parser.y"
{ yymsp[-2].minor.yy38->push_back(yymsp[0].minor.yy196); yygotominor.yy38 = yymsp[-2].minor.yy38;   yy_destructor(yypParser,64,&yymsp[-1].minor);
}
#line 1957 "parser.c"
        break;
      case 137: /* osemi ::= SEMICOLON */
#line 558 "parser.y"
{
  yy_destructor(yypParser,35,&yymsp[0].minor);
}
#line 1964 "parser.c"
        break;
      case 140: /* ostmt_list ::= */
#line 567 "parser.y"
{ yygotominor.yy38 = 0; }
#line 1969 "parser.c"
        break;
      case 141: /* ostmt_list ::= stmt_list */
#line 568 "parser.y"
{ yygotominor.yy38 = yymsp[0].minor.yy38; }
#line 1974 "parser.c"
        break;
      case 144: /* ident ::= IDENT */
#line 577 "parser.y"
{ yygotominor.yy92 = new ident_expr_t(yymsp[0].minor.yy0); }
#line 1979 "parser.c"
        break;
      case 145: /* ident_list ::= ident */
#line 581 "parser.y"
{ yygotominor.yy222 = new ident_expr_vector_t(1, yymsp[0].minor.yy92); }
#line 1984 "parser.c"
        break;
      case 146: /* ident_list ::= ident_list COMMA ident */
#line 582 "parser.y"
{ yymsp[-2].minor.yy222->push_back(yymsp[0].minor.yy92); yygotominor.yy222 = yymsp[-2].minor.yy222;   yy_destructor(yypParser,64,&yymsp[-1].minor);
}
#line 1990 "parser.c"
        break;
      case 148: /* oargs_comma_list ::= args_comma_list */
#line 587 "parser.y"
{ yygotominor.yy104 = yymsp[0].minor.yy104; }
#line 1995 "parser.c"
        break;
      case 150: /* ofield_semi_list_and_osemi ::= field_semi_list osemi */
#line 592 "parser.y"
{ yygotominor.yy104 = yymsp[-1].minor.yy104; }
#line 2000 "parser.c"
        break;
      default:
      /* (136) osemi ::= */ yytestcase(yyruleno==136);
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
#line 2071 "parser.c"
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
