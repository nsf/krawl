/* Driver template for the LEMON parser generator.
** The author disclaims copyright to this source code.
*/
/* First off, code is included that follows the "include" declaration
** in the input grammar file. */
#include <stdio.h>
#line 10 "parser.y"

	#include "krawl.hpp"
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
#define YYNSTATE 282
#define YYNRULE 153
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
 /*     0 */    59,   60,  196,   64,  143,   63,  205,  200,  114,   62,
 /*    10 */     9,   10,   11,   12,   13,   14,   15,   16,   17,   18,
 /*    20 */    19,   20,   21,  221,  141,  187,   66,  265,  266,  267,
 /*    30 */    22,   56,  196,   25,   57,    1,  279,  220,  135,  230,
 /*    40 */    92,   35,   78,   95,   73,   74,  107,  196,  218,  143,
 /*    50 */   269,  143,  253,   55,  199,  158,  216,  217,  219,   61,
 /*    60 */    54,   53,   47,   48,   49,   50,   51,   52,   43,   44,
 /*    70 */    45,   46,   34,   36,   38,   39,   40,   41,   42,  285,
 /*    80 */   177,   59,   60,  272,   64,  143,   63,  108,  200,   76,
 /*    90 */    62,  182,  203,  260,  261,   34,   36,   38,   39,   40,
 /*   100 */    41,   42,  202,  285,  285,  285,  183,  285,  265,  266,
 /*   110 */   267,   22,   56,  274,   25,   57,    1,   67,  185,  221,
 /*   120 */    91,   92,   35,  196,   95,   73,   74,  107,  196,  218,
 /*   130 */   188,   97,  139,  220,  135,  224,  189,  216,  217,  219,
 /*   140 */    61,   54,   53,   47,   48,   49,   50,   51,   52,   43,
 /*   150 */    44,   45,   46,   34,   36,   38,   39,   40,   41,   42,
 /*   160 */   285,   43,   44,   45,   46,   34,   36,   38,   39,   40,
 /*   170 */    41,   42,   93,  277,  260,  261,   92,  249,   98,   95,
 /*   180 */    73,   74,  107,   79,    1,   94,   54,   53,   47,   48,
 /*   190 */    49,   50,   51,   52,   43,   44,   45,   46,   34,   36,
 /*   200 */    38,   39,   40,   41,   42,   54,   53,   47,   48,   49,
 /*   210 */    50,   51,   52,   43,   44,   45,   46,   34,   36,   38,
 /*   220 */    39,   40,   41,   42,  148,  196,   28,   33,  180,    1,
 /*   230 */   144,  146,  208,   76,  149,  134,  203,  250,  214,  254,
 /*   240 */    77,  101,  209,   87,   70,  140,  204,  211,   86,   78,
 /*   250 */    54,   53,   47,   48,   49,   50,   51,   52,   43,   44,
 /*   260 */    45,   46,   34,   36,   38,   39,   40,   41,   42,   54,
 /*   270 */    53,   47,   48,   49,   50,   51,   52,   43,   44,   45,
 /*   280 */    46,   34,   36,   38,   39,   40,   41,   42,   54,   53,
 /*   290 */    47,   48,   49,   50,   51,   52,   43,   44,   45,   46,
 /*   300 */    34,   36,   38,   39,   40,   41,   42,   76,   80,  143,
 /*   310 */   203,  150,  212,  143,  143,  151,  192,  193,   99,  222,
 /*   320 */   204,   54,   53,   47,   48,   49,   50,   51,   52,   43,
 /*   330 */    44,   45,   46,   34,   36,   38,   39,   40,   41,   42,
 /*   340 */   255,  105,  254,  142,  191,  143,  223,   70,  194,   53,
 /*   350 */    47,   48,   49,   50,   51,   52,   43,   44,   45,   46,
 /*   360 */    34,   36,   38,   39,   40,   41,   42,   47,   48,   49,
 /*   370 */    50,   51,   52,   43,   44,   45,   46,   34,   36,   38,
 /*   380 */    39,   40,   41,   42,   59,   60,  195,   64,   69,   63,
 /*   390 */   255,  106,  254,   62,  256,  206,  254,   70,  110,  113,
 /*   400 */   123,   70,  221,   59,   60,  119,   64,  221,   63,   81,
 /*   410 */   179,  239,   62,   65,  221,  236,  220,  135,  225,    7,
 /*   420 */   221,  220,  135,  226,   23,   35,  238,   83,  220,  135,
 /*   430 */   230,  196,  218,  215,  220,  135,  230,  213,    7,  153,
 /*   440 */   216,  217,  219,   61,   35,   24,   83,  436,   96,   75,
 /*   450 */   196,  218,  273,   28,  143,  180,  281,  152,  157,  216,
 /*   460 */   217,  219,   61,  160,  117,   79,  137,  161,    4,   59,
 /*   470 */    60,    2,   64,  252,   63,  254,    6,    1,   62,  221,
 /*   480 */    70,  280,   59,   60,   37,   64,  136,   63,   55,  143,
 /*   490 */   181,   62,  156,  220,  135,  230,  242,  143,  243,  245,
 /*   500 */   241,   58,  143,  270,    1,  248,  148,  196,  275,   33,
 /*   510 */    35,  276,  144,  146,  282,  184,  196,  218,  186,  120,
 /*   520 */   138,  221,  190,   35,   31,  216,  217,  219,   61,  196,
 /*   530 */   218,  197,   84,   32,  221,  220,  135,  227,  216,  217,
 /*   540 */   219,   61,   59,   60,  100,   64,  198,   63,  220,  135,
 /*   550 */   230,   62,  109,  258,  262,  113,  123,  354,  263,  264,
 /*   560 */    29,  111,  258,  262,  113,  123,  134,  263,  264,  214,
 /*   570 */   221,   77,   68,  145,  201,   72,  237,  102,  210,  221,
 /*   580 */   154,  247,  354,   35,  220,  135,  230,  155,   82,  196,
 /*   590 */   218,  240,  251,  220,  135,  230,  221,  100,  216,  217,
 /*   600 */   219,   61,  112,  258,  262,  113,  123,   23,  263,  264,
 /*   610 */   220,  135,  230,  259,  262,  113,  123,  221,  263,  264,
 /*   620 */   221,    8,  268,  262,  113,  123,  257,  263,  264,   30,
 /*   630 */   221,  220,  135,  228,  220,  135,  230,   79,   71,  221,
 /*   640 */     5,  117,   27,  104,  220,  135,  230,   26,    3,  437,
 /*   650 */   278,  221,  437,  220,  135,  230,  221,  437,   79,  437,
 /*   660 */   437,  437,  117,  437,  159,  220,  135,  229,  437,  437,
 /*   670 */   220,  135,  230,  242,  437,  243,  245,  221,  148,  196,
 /*   680 */   117,   33,  163,  437,  144,  146,  207,  437,  437,  115,
 /*   690 */   437,  220,  135,  230,  242,  221,  243,  245,  437,  148,
 /*   700 */   196,  117,   33,  164,  221,  144,  146,  437,  103,  220,
 /*   710 */   135,  230,  242,  246,  243,  245,  221,  437,  220,  135,
 /*   720 */   230,  437,  117,  437,  165,   79,  437,  437,  221,  437,
 /*   730 */   220,  135,  230,  242,  437,  243,  245,  221,  437,  437,
 /*   740 */   437,  437,  220,  135,  230,  437,  437,  117,  437,  166,
 /*   750 */   437,  220,  135,  230,  242,  437,  243,  245,  437,  117,
 /*   760 */   437,  167,  221,  437,   79,  437,  148,  196,  117,   33,
 /*   770 */   168,  100,  144,  146,  221,  437,  220,  135,  230,  242,
 /*   780 */   437,  243,  245,  221,  437,  437,  437,  437,  220,  135,
 /*   790 */   230,  242,  437,  243,  245,  437,  437,  220,  135,  230,
 /*   800 */   242,  437,  243,  245,  437,  148,  196,  117,   33,  169,
 /*   810 */   437,  144,  146,  437,  117,  437,  170,   76,  162,  437,
 /*   820 */   203,   85,  221,  437,  437,  101,  209,  141,  187,  221,
 /*   830 */   204,  437,  437,  437,  437,  437,  220,  135,  230,  242,
 /*   840 */   437,  243,  245,  220,  135,  230,  242,  437,  243,  245,
 /*   850 */   437,  117,  437,  171,  437,  437,  134,  437,  117,  214,
 /*   860 */   172,   77,  437,  147,  437,  437,  221,  102,  210,  437,
 /*   870 */   437,  437,  437,  221,  437,  437,  117,  437,  173,  437,
 /*   880 */   220,  135,  230,  242,  437,  243,  245,  220,  135,  230,
 /*   890 */   242,  221,  243,  245,  116,  437,  437,  437,  437,  117,
 /*   900 */   437,  174,  437,  437,  437,  220,  135,  230,  242,  221,
 /*   910 */   243,  245,  437,  437,  221,  437,  437,  437,  437,  437,
 /*   920 */   117,  437,  175,  220,  135,  230,  437,  437,  220,  135,
 /*   930 */   230,  242,  437,  243,  245,  221,  437,  437,  437,  437,
 /*   940 */   437,  437,  437,  117,  437,  176,  437,  437,  437,  220,
 /*   950 */   135,  230,  242,  117,  243,  245,  437,  437,  221,  437,
 /*   960 */   437,  437,  437,  437,  178,   88,  123,  437,  221,  271,
 /*   970 */   437,  231,  220,  135,  230,  242,  232,  243,  245,  437,
 /*   980 */   221,  437,  220,  135,  230,  242,  221,  243,  244,  437,
 /*   990 */   437,  221,  437,  437,  220,  135,  230,  233,  437,  437,
 /*  1000 */   220,  135,  230,  437,  437,  220,  135,  230,  234,  437,
 /*  1010 */   437,  437,  221,  437,  437,  437,  437,  437,  437,  437,
 /*  1020 */   235,  437,  437,  221,  437,  437,  220,  135,  230,  130,
 /*  1030 */   437,  437,  131,  437,  437,  221,  437,  220,  135,  230,
 /*  1040 */   437,  132,  437,  437,  221,  437,  437,  221,  437,  220,
 /*  1050 */   135,  230,  133,  437,  437,  437,  221,  437,  220,  135,
 /*  1060 */   230,  220,  135,  230,  124,  437,  437,  221,  437,  437,
 /*  1070 */   220,  135,  230,  125,  437,  437,  437,  126,  437,  221,
 /*  1080 */   437,  220,  135,  230,  127,  437,  437,  437,  221,  437,
 /*  1090 */   437,  437,  221,  220,  135,  230,  437,  128,  437,  221,
 /*  1100 */   437,  437,  220,  135,  230,  129,  220,  135,  230,  122,
 /*  1110 */   437,  437,  221,  220,  135,  230,  121,  437,  437,  437,
 /*  1120 */   221,  437,  437,  437,  221,  437,  220,  135,  230,  118,
 /*  1130 */   437,  221,  437,   89,  220,  135,  230,   90,  220,  135,
 /*  1140 */   230,  437,  437,  437,  221,  220,  135,  230,  221,  437,
 /*  1150 */   437,  437,  221,  437,  437,  437,  437,  437,  220,  135,
 /*  1160 */   230,  437,  220,  135,  230,  437,  220,  135,  230,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */     9,   10,   56,   12,   91,   14,   93,   94,   76,   18,
 /*    10 */    21,   22,   23,   24,   25,   26,   27,   28,   29,   30,
 /*    20 */    31,   32,   33,   91,   56,   57,   50,   36,   37,   38,
 /*    30 */    39,   40,   56,   42,   43,   44,   45,  105,  106,  107,
 /*    40 */    49,   50,   56,   52,   53,   54,   55,   56,   57,   91,
 /*    50 */    80,   91,   94,   64,   94,   97,   65,   66,   67,   68,
 /*    60 */     1,    2,    3,    4,    5,    6,    7,    8,    9,   10,
 /*    70 */    11,   12,   13,   14,   15,   16,   17,   18,   19,   20,
 /*    80 */    80,    9,   10,   82,   12,   91,   14,   93,   94,   91,
 /*    90 */    18,   79,   94,   34,   35,   13,   14,   15,   16,   17,
 /*   100 */    18,   19,  104,   44,   45,   46,   85,   48,   36,   37,
 /*   110 */    38,   39,   40,   84,   42,   43,   44,   50,   85,   91,
 /*   120 */    20,   49,   50,   56,   52,   53,   54,   55,   56,   57,
 /*   130 */    85,   86,   72,  105,  106,  107,   87,   65,   66,   67,
 /*   140 */    68,    1,    2,    3,    4,    5,    6,    7,    8,    9,
 /*   150 */    10,   11,   12,   13,   14,   15,   16,   17,   18,   19,
 /*   160 */    20,    9,   10,   11,   12,   13,   14,   15,   16,   17,
 /*   170 */    18,   19,   83,   84,   34,   35,   49,   87,   88,   52,
 /*   180 */    53,   54,   55,   14,   44,   20,    1,    2,    3,    4,
 /*   190 */     5,    6,    7,    8,    9,   10,   11,   12,   13,   14,
 /*   200 */    15,   16,   17,   18,   19,    1,    2,    3,    4,    5,
 /*   210 */     6,    7,    8,    9,   10,   11,   12,   13,   14,   15,
 /*   220 */    16,   17,   18,   19,   55,   56,   46,   58,   48,   44,
 /*   230 */    61,   62,   63,   91,   92,   91,   94,   89,   94,   91,
 /*   240 */    96,   99,  100,   50,   96,   72,  104,  103,   44,   56,
 /*   250 */     1,    2,    3,    4,    5,    6,    7,    8,    9,   10,
 /*   260 */    11,   12,   13,   14,   15,   16,   17,   18,   19,    1,
 /*   270 */     2,    3,    4,    5,    6,    7,    8,    9,   10,   11,
 /*   280 */    12,   13,   14,   15,   16,   17,   18,   19,    1,    2,
 /*   290 */     3,    4,    5,    6,    7,    8,    9,   10,   11,   12,
 /*   300 */    13,   14,   15,   16,   17,   18,   19,   91,   59,   91,
 /*   310 */    94,   95,   94,   91,   91,   99,   94,   94,   20,   51,
 /*   320 */   104,    1,    2,    3,    4,    5,    6,    7,    8,    9,
 /*   330 */    10,   11,   12,   13,   14,   15,   16,   17,   18,   19,
 /*   340 */    89,   90,   91,   72,   87,   91,   59,   96,   94,    2,
 /*   350 */     3,    4,    5,    6,    7,    8,    9,   10,   11,   12,
 /*   360 */    13,   14,   15,   16,   17,   18,   19,    3,    4,    5,
 /*   370 */     6,    7,    8,    9,   10,   11,   12,   13,   14,   15,
 /*   380 */    16,   17,   18,   19,    9,   10,   91,   12,   64,   14,
 /*   390 */    89,   90,   91,   18,   89,  101,   91,   96,   75,   76,
 /*   400 */    77,   96,   91,    9,   10,   76,   12,   91,   14,   50,
 /*   410 */    81,   51,   18,   20,   91,   56,  105,  106,  107,   44,
 /*   420 */    91,  105,  106,  107,   64,   50,   51,   52,  105,  106,
 /*   430 */   107,   56,   57,   72,  105,  106,  107,   91,   44,  109,
 /*   440 */    65,   66,   67,   68,   50,   64,   52,   70,   71,   20,
 /*   450 */    56,   57,   45,   46,   91,   48,   79,   94,   72,   65,
 /*   460 */    66,   67,   68,   72,   76,   14,   78,   91,   20,    9,
 /*   470 */    10,   47,   12,   89,   14,   91,   50,   44,   18,   91,
 /*   480 */    96,   80,    9,   10,   58,   12,   60,   14,   64,   91,
 /*   490 */    72,   18,   94,  105,  106,  107,  108,   91,  110,  111,
 /*   500 */    94,   50,   91,   80,   44,   94,   55,   56,   72,   58,
 /*   510 */    50,   72,   61,   62,    0,   51,   56,   57,   57,   76,
 /*   520 */    77,   91,   51,   50,   44,   65,   66,   67,   68,   56,
 /*   530 */    57,   45,   59,   44,   91,  105,  106,  107,   65,   66,
 /*   540 */    67,   68,    9,   10,   60,   12,   45,   14,  105,  106,
 /*   550 */   107,   18,   73,   74,   75,   76,   77,   20,   79,   80,
 /*   560 */    50,   73,   74,   75,   76,   77,   91,   79,   80,   94,
 /*   570 */    91,   96,   51,   98,   51,   64,   51,  102,  103,   91,
 /*   580 */    45,   76,   45,   50,  105,  106,  107,   60,   50,   56,
 /*   590 */    57,   51,   51,  105,  106,  107,   91,   60,   65,   66,
 /*   600 */    67,   68,   73,   74,   75,   76,   77,   64,   79,   80,
 /*   610 */   105,  106,  107,   74,   75,   76,   77,   91,   79,   80,
 /*   620 */    91,   21,   74,   75,   76,   77,   51,   79,   80,   50,
 /*   630 */    91,  105,  106,  107,  105,  106,  107,   14,   51,   91,
 /*   640 */    41,   76,   20,   78,  105,  106,  107,   20,   47,  112,
 /*   650 */    45,   91,  112,  105,  106,  107,   91,  112,   14,  112,
 /*   660 */   112,  112,   76,  112,   78,  105,  106,  107,  112,  112,
 /*   670 */   105,  106,  107,  108,  112,  110,  111,   91,   55,   56,
 /*   680 */    76,   58,   78,  112,   61,   62,   63,  112,  112,   76,
 /*   690 */   112,  105,  106,  107,  108,   91,  110,  111,  112,   55,
 /*   700 */    56,   76,   58,   78,   91,   61,   62,  112,   64,  105,
 /*   710 */   106,  107,  108,   76,  110,  111,   91,  112,  105,  106,
 /*   720 */   107,  112,   76,  112,   78,   14,  112,  112,   91,  112,
 /*   730 */   105,  106,  107,  108,  112,  110,  111,   91,  112,  112,
 /*   740 */   112,  112,  105,  106,  107,  112,  112,   76,  112,   78,
 /*   750 */   112,  105,  106,  107,  108,  112,  110,  111,  112,   76,
 /*   760 */   112,   78,   91,  112,   14,  112,   55,   56,   76,   58,
 /*   770 */    78,   60,   61,   62,   91,  112,  105,  106,  107,  108,
 /*   780 */   112,  110,  111,   91,  112,  112,  112,  112,  105,  106,
 /*   790 */   107,  108,  112,  110,  111,  112,  112,  105,  106,  107,
 /*   800 */   108,  112,  110,  111,  112,   55,   56,   76,   58,   78,
 /*   810 */   112,   61,   62,  112,   76,  112,   78,   91,   92,  112,
 /*   820 */    94,   50,   91,  112,  112,   99,  100,   56,   57,   91,
 /*   830 */   104,  112,  112,  112,  112,  112,  105,  106,  107,  108,
 /*   840 */   112,  110,  111,  105,  106,  107,  108,  112,  110,  111,
 /*   850 */   112,   76,  112,   78,  112,  112,   91,  112,   76,   94,
 /*   860 */    78,   96,  112,   98,  112,  112,   91,  102,  103,  112,
 /*   870 */   112,  112,  112,   91,  112,  112,   76,  112,   78,  112,
 /*   880 */   105,  106,  107,  108,  112,  110,  111,  105,  106,  107,
 /*   890 */   108,   91,  110,  111,   76,  112,  112,  112,  112,   76,
 /*   900 */   112,   78,  112,  112,  112,  105,  106,  107,  108,   91,
 /*   910 */   110,  111,  112,  112,   91,  112,  112,  112,  112,  112,
 /*   920 */    76,  112,   78,  105,  106,  107,  112,  112,  105,  106,
 /*   930 */   107,  108,  112,  110,  111,   91,  112,  112,  112,  112,
 /*   940 */   112,  112,  112,   76,  112,   78,  112,  112,  112,  105,
 /*   950 */   106,  107,  108,   76,  110,  111,  112,  112,   91,  112,
 /*   960 */   112,  112,  112,  112,   75,   76,   77,  112,   91,   80,
 /*   970 */   112,   76,  105,  106,  107,  108,   76,  110,  111,  112,
 /*   980 */    91,  112,  105,  106,  107,  108,   91,  110,  111,  112,
 /*   990 */   112,   91,  112,  112,  105,  106,  107,   76,  112,  112,
 /*  1000 */   105,  106,  107,  112,  112,  105,  106,  107,   76,  112,
 /*  1010 */   112,  112,   91,  112,  112,  112,  112,  112,  112,  112,
 /*  1020 */    76,  112,  112,   91,  112,  112,  105,  106,  107,   76,
 /*  1030 */   112,  112,   76,  112,  112,   91,  112,  105,  106,  107,
 /*  1040 */   112,   76,  112,  112,   91,  112,  112,   91,  112,  105,
 /*  1050 */   106,  107,   76,  112,  112,  112,   91,  112,  105,  106,
 /*  1060 */   107,  105,  106,  107,   76,  112,  112,   91,  112,  112,
 /*  1070 */   105,  106,  107,   76,  112,  112,  112,   76,  112,   91,
 /*  1080 */   112,  105,  106,  107,   76,  112,  112,  112,   91,  112,
 /*  1090 */   112,  112,   91,  105,  106,  107,  112,   76,  112,   91,
 /*  1100 */   112,  112,  105,  106,  107,   76,  105,  106,  107,   76,
 /*  1110 */   112,  112,   91,  105,  106,  107,   76,  112,  112,  112,
 /*  1120 */    91,  112,  112,  112,   91,  112,  105,  106,  107,   76,
 /*  1130 */   112,   91,  112,   76,  105,  106,  107,   76,  105,  106,
 /*  1140 */   107,  112,  112,  112,   91,  105,  106,  107,   91,  112,
 /*  1150 */   112,  112,   91,  112,  112,  112,  112,  112,  105,  106,
 /*  1160 */   107,  112,  105,  106,  107,  112,  105,  106,  107,
};
#define YY_SHIFT_USE_DFLT (-55)
#define YY_SHIFT_MAX 181
static const short yy_shift_ofst[] = {
 /*     0 */   127,   -9,   72,   72,   72,   72,  375,  394,  394,  394,
 /*    10 */   394,  394,  394,  394,  394,  394,  394,  394,  394,  394,
 /*    20 */   394,  394,  394,  394,  394,  460,  533,  533,  533,  169,
 /*    30 */   169,  750,  750,  473,  533,  533,  533,  533,  533,  533,
 /*    40 */   533,  533,  533,  533,  533,  533,  533,  533,  533,  533,
 /*    50 */   533,  533,  533,  533,  533,  533,  533,  533,  750,  533,
 /*    60 */   533,  533,  533,  533,  533,  750,  -54,  -54,  451,  623,
 /*    70 */   644,  451,  750,  -24,   67,  -54,  711,  644,  750,  750,
 /*    80 */   750,  750,  750,  750,  750,  -32,  180,  -14,  140,  185,
 /*    90 */   204,  127,  771,  407,  -32,  193,  100,  165,  298,  -14,
 /*   100 */   -54,  324,  393,  -54,  381,  429,  429,  -54,  433,  448,
 /*   110 */   433,  448,  448,   59,  249,  268,  287,  320,  320,  320,
 /*   120 */   320,  347,  364,  -11,  152,  152,  152,  152,  152,  152,
 /*   130 */    82,   82,   82,   82,  537,  426,  359,  360,  424,  514,
 /*   140 */   464,  461,  471,  484,  480,  486,  489,  501,  510,  521,
 /*   150 */   523,  511,  525,  535,  527,  538,  540,  541,  600,  543,
 /*   160 */   575,  579,  587,  543,  543,  543,  543,  543,  543,  543,
 /*   170 */   543,  543,  543,  543,  543,  543,  543,  599,  622,  627,
 /*   180 */   601,  605,
};
#define YY_REDUCE_USE_DFLT (-88)
#define YY_REDUCE_MAX 112
static const short yy_reduce_ofst[] = {
 /*     0 */   377,  479,  488,  529,  539,  548,  388,  565,  586,  604,
 /*    10 */   625,  646,  671,  683,  692,  731,  738,  775,  782,  800,
 /*    20 */   823,  844,  867,  877,  877,  889,  323,  329,  443,  142,
 /*    30 */   726,  475,  765,  -68,  505,  613,  637,  818,  895,  900,
 /*    40 */   921,  932,  944,  953,  956,  965,  976,  988,  997, 1001,
 /*    50 */  1008, 1021, 1029, 1033, 1040, 1053, 1057, 1061,  216,   28,
 /*    60 */   311,  316,  430,  526,  560,  144,  251,  301,  -87,   -2,
 /*    70 */   -42,   -6,   -2,  148,  305,  384,  -40,  218,  222,  223,
 /*    80 */   254,  363,  398,  406,  411,   45,   89,   90,  -30,    0,
 /*    90 */     1,   12,   21,   29,   33,   49,   60,  173,  271,  257,
 /*   100 */   295,  294,  361,  346,  330,  386,  391,  376,  401,  418,
 /*   110 */   423,  436,  439,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   435,  435,  319,  321,  421,  435,  435,  435,  435,  435,
 /*    10 */   435,  435,  435,  435,  435,  435,  435,  435,  435,  435,
 /*    20 */   435,  435,  308,  435,  423,  435,  435,  424,  435,  431,
 /*    30 */   431,  433,  433,  435,  435,  435,  435,  435,  435,  435,
 /*    40 */   435,  435,  435,  435,  435,  435,  435,  435,  435,  435,
 /*    50 */   435,  435,  435,  435,  435,  435,  435,  435,  435,  435,
 /*    60 */   435,  435,  435,  435,  435,  421,  435,  435,  338,  435,
 /*    70 */   426,  338,  435,  435,  435,  421,  354,  435,  435,  435,
 /*    80 */   435,  435,  435,  435,  435,  435,  435,  435,  418,  435,
 /*    90 */   435,  421,  435,  435,  421,  435,  420,  420,  420,  421,
 /*   100 */   435,  368,  420,  435,  422,  420,  420,  435,  334,  420,
 /*   110 */   435,  420,  420,  418,  435,  435,  435,  413,  419,  425,
 /*   120 */   418,  409,  408,  435,  402,  403,  404,  405,  406,  407,
 /*   130 */   398,  399,  400,  401,  429,  383,  435,  435,  435,  435,
 /*   140 */   435,  435,  435,  354,  435,  435,  435,  435,  435,  435,
 /*   150 */   435,  358,  435,  435,  410,  435,  435,  435,  348,  349,
 /*   160 */   435,  435,  435,  286,  287,  288,  289,  290,  291,  292,
 /*   170 */   293,  294,  295,  296,  297,  298,  307,  309,  435,  435,
 /*   180 */   435,  435,  325,  326,  327,  340,  341,  342,  339,  328,
 /*   190 */   329,  344,  345,  350,  351,  353,  428,  355,  356,  365,
 /*   200 */   336,  337,  364,  366,  363,  357,  359,  367,  360,  432,
 /*   210 */   361,  362,  369,  430,  370,  434,  371,  372,  373,  374,
 /*   220 */   375,  376,  377,  378,  384,  385,  386,  387,  388,  389,
 /*   230 */   390,  393,  394,  395,  396,  397,  379,  380,  381,  382,
 /*   240 */   411,  412,  414,  415,  417,  416,  392,  391,  352,  343,
 /*   250 */   330,  331,  347,  427,  429,  346,  332,  333,  283,  284,
 /*   260 */   299,  300,  301,  302,  303,  304,  305,  306,  310,  312,
 /*   270 */   311,  313,  314,  315,  317,  318,  320,  316,  322,  323,
 /*   280 */   335,  324,
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
  "SEMICOLON",     "ASSIGN",        "DECLARIZE",     "A_DIVIDE",    
  "A_TIMES",       "A_MOD",         "A_SHIFTL",      "A_SHIFTR",    
  "A_AND",         "A_ANDNOT",      "A_PLUS",        "A_MINUS",     
  "A_OR",          "A_XOR",         "INC",           "DEC",         
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
  "sw_case",       "import_spec",   "import_spec_list",  "type_spec",   
  "type_spec_list",  "value_spec",    "value_spec_list",  "ident",       
  "oargs_comma_list",  "func_results",  "type",          "field_comma_list",
  "ident_list",    "otype",         "ofield_semi_list_and_osemi",  "nametype_list",
  "args_comma_list",  "ofunc_ellipsis",  "field_semi_list",  "field",       
  "nametype",      "basic_lit",     "pexpr",         "uexpr",       
  "compound_lit",  "ocomma",        "type_expr",     "iexpr",       
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *const yyRuleName[] = {
 /*   0 */ "program ::= decl_list osemi",
 /*   1 */ "stmt_list ::= stmt",
 /*   2 */ "stmt_list ::= stmt_list SEMICOLON stmt",
 /*   3 */ "s_stmt ::= expr",
 /*   4 */ "s_stmt ::= expr_list ASSIGN iexpr_list",
 /*   5 */ "s_stmt ::= expr_list DECLARIZE iexpr_list",
 /*   6 */ "s_stmt ::= expr_list A_DIVIDE iexpr_list",
 /*   7 */ "s_stmt ::= expr_list A_TIMES iexpr_list",
 /*   8 */ "s_stmt ::= expr_list A_MOD iexpr_list",
 /*   9 */ "s_stmt ::= expr_list A_SHIFTL iexpr_list",
 /*  10 */ "s_stmt ::= expr_list A_SHIFTR iexpr_list",
 /*  11 */ "s_stmt ::= expr_list A_AND iexpr_list",
 /*  12 */ "s_stmt ::= expr_list A_ANDNOT iexpr_list",
 /*  13 */ "s_stmt ::= expr_list A_PLUS iexpr_list",
 /*  14 */ "s_stmt ::= expr_list A_MINUS iexpr_list",
 /*  15 */ "s_stmt ::= expr_list A_OR iexpr_list",
 /*  16 */ "s_stmt ::= expr_list A_XOR iexpr_list",
 /*  17 */ "s_stmt ::= expr INC",
 /*  18 */ "s_stmt ::= expr DEC",
 /*  19 */ "stmt ::= s_stmt",
 /*  20 */ "stmt ::= decl",
 /*  21 */ "stmt ::= block_stmt",
 /*  22 */ "stmt ::= BREAK",
 /*  23 */ "stmt ::= CONTINUE",
 /*  24 */ "stmt ::= FALLTHROUGH",
 /*  25 */ "stmt ::= RETURN iexpr_list",
 /*  26 */ "stmt ::= RETURN",
 /*  27 */ "stmt ::= IF expr block_stmt",
 /*  28 */ "stmt ::= IF expr block_stmt ELSE stmt",
 /*  29 */ "stmt ::= FOR s_stmt SEMICOLON oexpr SEMICOLON s_stmt block_stmt",
 /*  30 */ "stmt ::= FOR expr block_stmt",
 /*  31 */ "stmt ::= FOR block_stmt",
 /*  32 */ "stmt ::= SWITCH expr sw_block_stmt",
 /*  33 */ "sw_block_stmt ::= LCURLY sw_case_list RCURLY",
 /*  34 */ "sw_case_list ::= sw_case",
 /*  35 */ "sw_case_list ::= sw_case_list sw_case",
 /*  36 */ "sw_case ::= CASE expr_list COLON stmt_list osemi",
 /*  37 */ "sw_case ::= CASE expr_list COLON",
 /*  38 */ "sw_case ::= DEFAULT COLON stmt_list osemi",
 /*  39 */ "sw_case ::= DEFAULT COLON",
 /*  40 */ "block_stmt ::= LCURLY stmt_list osemi RCURLY",
 /*  41 */ "block_stmt ::= LCURLY RCURLY",
 /*  42 */ "decl_list ::= decl",
 /*  43 */ "decl_list ::= decl_list SEMICOLON decl",
 /*  44 */ "decl ::= IMPORT import_spec",
 /*  45 */ "decl ::= IMPORT LPAREN import_spec_list osemi RPAREN",
 /*  46 */ "decl ::= TYPE type_spec",
 /*  47 */ "decl ::= TYPE LPAREN type_spec_list osemi RPAREN",
 /*  48 */ "decl ::= CONST value_spec",
 /*  49 */ "decl ::= CONST LPAREN value_spec_list osemi RPAREN",
 /*  50 */ "decl ::= VAR value_spec",
 /*  51 */ "decl ::= VAR LPAREN value_spec_list osemi RPAREN",
 /*  52 */ "decl ::= FUNC ident LPAREN oargs_comma_list RPAREN func_results",
 /*  53 */ "decl ::= FUNC ident LPAREN oargs_comma_list RPAREN func_results block_stmt",
 /*  54 */ "func_results ::= type",
 /*  55 */ "func_results ::= LPAREN field_comma_list RPAREN",
 /*  56 */ "func_results ::=",
 /*  57 */ "import_spec_list ::= import_spec",
 /*  58 */ "import_spec_list ::= import_spec_list SEMICOLON import_spec",
 /*  59 */ "import_spec ::= IDENT STRING",
 /*  60 */ "import_spec ::= STRING",
 /*  61 */ "type_spec_list ::= type_spec",
 /*  62 */ "type_spec_list ::= type_spec_list SEMICOLON type_spec",
 /*  63 */ "type_spec ::= IDENT type",
 /*  64 */ "value_spec_list ::= value_spec",
 /*  65 */ "value_spec_list ::= value_spec_list SEMICOLON value_spec",
 /*  66 */ "value_spec ::= ident_list otype",
 /*  67 */ "value_spec ::= ident_list otype ASSIGN iexpr_list",
 /*  68 */ "type ::= TIMES type",
 /*  69 */ "type ::= LSB expr RSB type",
 /*  70 */ "type ::= LSB RSB type",
 /*  71 */ "type ::= ident DOT ident",
 /*  72 */ "type ::= ident",
 /*  73 */ "type ::= STRUCT LCURLY ofield_semi_list_and_osemi RCURLY",
 /*  74 */ "type ::= UNION LCURLY ofield_semi_list_and_osemi RCURLY",
 /*  75 */ "type ::= FUNC LPAREN oargs_comma_list RPAREN func_results",
 /*  76 */ "field_comma_list ::= nametype_list",
 /*  77 */ "args_comma_list ::= nametype_list ofunc_ellipsis",
 /*  78 */ "args_comma_list ::= ELLIPSIS",
 /*  79 */ "field_semi_list ::= field",
 /*  80 */ "field_semi_list ::= field_semi_list SEMICOLON field",
 /*  81 */ "nametype_list ::= nametype",
 /*  82 */ "nametype_list ::= nametype_list COMMA nametype",
 /*  83 */ "nametype ::= ident type",
 /*  84 */ "nametype ::= type",
 /*  85 */ "ofunc_ellipsis ::= COMMA ELLIPSIS",
 /*  86 */ "ofunc_ellipsis ::=",
 /*  87 */ "field ::= ident_list type",
 /*  88 */ "field ::= type",
 /*  89 */ "basic_lit ::= INT",
 /*  90 */ "basic_lit ::= FLOAT",
 /*  91 */ "basic_lit ::= STRING",
 /*  92 */ "basic_lit ::= CHAR",
 /*  93 */ "pexpr ::= basic_lit",
 /*  94 */ "pexpr ::= ident",
 /*  95 */ "pexpr ::= LPAREN expr RPAREN",
 /*  96 */ "pexpr ::= pexpr LSB expr RSB",
 /*  97 */ "pexpr ::= pexpr DOT IDENT",
 /*  98 */ "pexpr ::= pexpr DOT LPAREN type RPAREN",
 /*  99 */ "pexpr ::= pexpr LPAREN RPAREN",
 /* 100 */ "pexpr ::= pexpr LPAREN iexpr_list RPAREN",
 /* 101 */ "uexpr ::= pexpr",
 /* 102 */ "uexpr ::= PLUS uexpr",
 /* 103 */ "uexpr ::= MINUS uexpr",
 /* 104 */ "uexpr ::= NOT uexpr",
 /* 105 */ "uexpr ::= AND uexpr",
 /* 106 */ "uexpr ::= TIMES uexpr",
 /* 107 */ "uexpr ::= XOR uexpr",
 /* 108 */ "expr ::= uexpr",
 /* 109 */ "expr ::= expr DIVIDE expr",
 /* 110 */ "expr ::= expr TIMES expr",
 /* 111 */ "expr ::= expr MOD expr",
 /* 112 */ "expr ::= expr SHIFTL expr",
 /* 113 */ "expr ::= expr SHIFTR expr",
 /* 114 */ "expr ::= expr AND expr",
 /* 115 */ "expr ::= expr ANDNOT expr",
 /* 116 */ "expr ::= expr PLUS expr",
 /* 117 */ "expr ::= expr MINUS expr",
 /* 118 */ "expr ::= expr OR expr",
 /* 119 */ "expr ::= expr XOR expr",
 /* 120 */ "expr ::= expr EQ expr",
 /* 121 */ "expr ::= expr NEQ expr",
 /* 122 */ "expr ::= expr LT expr",
 /* 123 */ "expr ::= expr LE expr",
 /* 124 */ "expr ::= expr GT expr",
 /* 125 */ "expr ::= expr GE expr",
 /* 126 */ "expr ::= expr ANDAND expr",
 /* 127 */ "expr ::= expr OROR expr",
 /* 128 */ "compound_lit ::= LCURLY iexpr_list ocomma RCURLY",
 /* 129 */ "compound_lit ::= LCURLY iexpr_list ocomma RCURLY DOT LPAREN type RPAREN",
 /* 130 */ "type_expr ::= TYPE type",
 /* 131 */ "iexpr ::= expr",
 /* 132 */ "iexpr ::= compound_lit",
 /* 133 */ "iexpr ::= type_expr",
 /* 134 */ "iexpr_list ::= iexpr",
 /* 135 */ "iexpr_list ::= iexpr_list COMMA iexpr",
 /* 136 */ "expr_list ::= expr",
 /* 137 */ "expr_list ::= expr_list COMMA expr",
 /* 138 */ "osemi ::=",
 /* 139 */ "osemi ::= SEMICOLON",
 /* 140 */ "ocomma ::=",
 /* 141 */ "ocomma ::= COMMA",
 /* 142 */ "oexpr ::=",
 /* 143 */ "oexpr ::= expr",
 /* 144 */ "otype ::=",
 /* 145 */ "otype ::= type",
 /* 146 */ "ident ::= IDENT",
 /* 147 */ "ident_list ::= ident",
 /* 148 */ "ident_list ::= ident_list COMMA ident",
 /* 149 */ "oargs_comma_list ::=",
 /* 150 */ "oargs_comma_list ::= args_comma_list",
 /* 151 */ "ofield_semi_list_and_osemi ::=",
 /* 152 */ "ofield_semi_list_and_osemi ::= field_semi_list osemi",
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
    case 20: /* SEMICOLON */
    case 21: /* ASSIGN */
    case 22: /* DECLARIZE */
    case 23: /* A_DIVIDE */
    case 24: /* A_TIMES */
    case 25: /* A_MOD */
    case 26: /* A_SHIFTL */
    case 27: /* A_SHIFTR */
    case 28: /* A_AND */
    case 29: /* A_ANDNOT */
    case 30: /* A_PLUS */
    case 31: /* A_MINUS */
    case 32: /* A_OR */
    case 33: /* A_XOR */
    case 34: /* INC */
    case 35: /* DEC */
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
#line 906 "parser.c"
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
  { 73, 3 },
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
  { 74, 1 },
  { 74, 1 },
  { 74, 1 },
  { 74, 1 },
  { 74, 1 },
  { 74, 1 },
  { 74, 2 },
  { 74, 1 },
  { 74, 3 },
  { 74, 5 },
  { 74, 7 },
  { 74, 3 },
  { 74, 2 },
  { 74, 3 },
  { 82, 3 },
  { 83, 1 },
  { 83, 2 },
  { 84, 5 },
  { 84, 3 },
  { 84, 4 },
  { 84, 2 },
  { 80, 4 },
  { 80, 2 },
  { 71, 1 },
  { 71, 3 },
  { 79, 2 },
  { 79, 5 },
  { 79, 2 },
  { 79, 5 },
  { 79, 2 },
  { 79, 5 },
  { 79, 2 },
  { 79, 5 },
  { 79, 6 },
  { 79, 7 },
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
  { 108, 4 },
  { 108, 8 },
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
  { 109, 0 },
  { 109, 1 },
  { 81, 0 },
  { 81, 1 },
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
	yygotominor.yy62 = new program_t(yymsp[-1].minor.yy38);
	ctx->ast = yygotominor.yy62;
}
#line 1348 "parser.c"
        break;
      case 1: /* stmt_list ::= stmt */
#line 44 "parser.y"
{ 
	yygotominor.yy38 = new node_vector_t;
	if (yymsp[0].minor.yy196) yygotominor.yy38->push_back(yymsp[0].minor.yy196);
}
#line 1356 "parser.c"
        break;
      case 2: /* stmt_list ::= stmt_list SEMICOLON stmt */
#line 48 "parser.y"
{
	if (yymsp[0].minor.yy196) yymsp[-2].minor.yy38->push_back(yymsp[0].minor.yy196);
	yygotominor.yy38 = yymsp[-2].minor.yy38;
  yy_destructor(yypParser,20,&yymsp[-1].minor);
}
#line 1365 "parser.c"
        break;
      case 3: /* s_stmt ::= expr */
#line 62 "parser.y"
{ yygotominor.yy196 = new expr_stmt_t(yymsp[0].minor.yy196); }
#line 1370 "parser.c"
        break;
      case 4: /* s_stmt ::= expr_list ASSIGN iexpr_list */
      case 5: /* s_stmt ::= expr_list DECLARIZE iexpr_list */ yytestcase(yyruleno==5);
      case 6: /* s_stmt ::= expr_list A_DIVIDE iexpr_list */ yytestcase(yyruleno==6);
      case 7: /* s_stmt ::= expr_list A_TIMES iexpr_list */ yytestcase(yyruleno==7);
      case 8: /* s_stmt ::= expr_list A_MOD iexpr_list */ yytestcase(yyruleno==8);
      case 9: /* s_stmt ::= expr_list A_SHIFTL iexpr_list */ yytestcase(yyruleno==9);
      case 10: /* s_stmt ::= expr_list A_SHIFTR iexpr_list */ yytestcase(yyruleno==10);
      case 11: /* s_stmt ::= expr_list A_AND iexpr_list */ yytestcase(yyruleno==11);
      case 12: /* s_stmt ::= expr_list A_ANDNOT iexpr_list */ yytestcase(yyruleno==12);
      case 13: /* s_stmt ::= expr_list A_PLUS iexpr_list */ yytestcase(yyruleno==13);
      case 14: /* s_stmt ::= expr_list A_MINUS iexpr_list */ yytestcase(yyruleno==14);
      case 15: /* s_stmt ::= expr_list A_OR iexpr_list */ yytestcase(yyruleno==15);
      case 16: /* s_stmt ::= expr_list A_XOR iexpr_list */ yytestcase(yyruleno==16);
#line 63 "parser.y"
{ yygotominor.yy196 = new assign_stmt_t(yymsp[-2].minor.yy38, yymsp[0].minor.yy38, yymsp[-1].minor.yy0); }
#line 1387 "parser.c"
        break;
      case 17: /* s_stmt ::= expr INC */
      case 18: /* s_stmt ::= expr DEC */ yytestcase(yyruleno==18);
#line 76 "parser.y"
{ yygotominor.yy196 = new incdec_stmt_t(yymsp[-1].minor.yy196, yymsp[0].minor.yy0); }
#line 1393 "parser.c"
        break;
      case 19: /* stmt ::= s_stmt */
      case 101: /* uexpr ::= pexpr */ yytestcase(yyruleno==101);
      case 108: /* expr ::= uexpr */ yytestcase(yyruleno==108);
      case 131: /* iexpr ::= expr */ yytestcase(yyruleno==131);
      case 132: /* iexpr ::= compound_lit */ yytestcase(yyruleno==132);
      case 133: /* iexpr ::= type_expr */ yytestcase(yyruleno==133);
      case 143: /* oexpr ::= expr */ yytestcase(yyruleno==143);
      case 145: /* otype ::= type */ yytestcase(yyruleno==145);
#line 87 "parser.y"
{ yygotominor.yy196 = yymsp[0].minor.yy196; }
#line 1405 "parser.c"
        break;
      case 20: /* stmt ::= decl */
#line 88 "parser.y"
{ yygotominor.yy196 = new decl_stmt_t(yymsp[0].minor.yy196); }
#line 1410 "parser.c"
        break;
      case 21: /* stmt ::= block_stmt */
#line 89 "parser.y"
{ yygotominor.yy196 = yymsp[0].minor.yy200; }
#line 1415 "parser.c"
        break;
      case 22: /* stmt ::= BREAK */
      case 23: /* stmt ::= CONTINUE */ yytestcase(yyruleno==23);
      case 24: /* stmt ::= FALLTHROUGH */ yytestcase(yyruleno==24);
#line 90 "parser.y"
{ yygotominor.yy196 = new flow_stmt_t(yymsp[0].minor.yy0); }
#line 1422 "parser.c"
        break;
      case 25: /* stmt ::= RETURN iexpr_list */
#line 93 "parser.y"
{ yygotominor.yy196 = new return_stmt_t(yymsp[0].minor.yy38, yymsp[-1].minor.yy0); }
#line 1427 "parser.c"
        break;
      case 26: /* stmt ::= RETURN */
#line 94 "parser.y"
{ yygotominor.yy196 = new return_stmt_t(0, yymsp[0].minor.yy0); }
#line 1432 "parser.c"
        break;
      case 27: /* stmt ::= IF expr block_stmt */
#line 96 "parser.y"
{
	yygotominor.yy196 = new ifelse_stmt_t(yymsp[-1].minor.yy196, yymsp[0].minor.yy200, yymsp[-2].minor.yy0);
}
#line 1439 "parser.c"
        break;
      case 28: /* stmt ::= IF expr block_stmt ELSE stmt */
#line 99 "parser.y"
{
	yygotominor.yy196 = new ifelse_stmt_t(yymsp[-3].minor.yy196, yymsp[-2].minor.yy200, yymsp[-4].minor.yy0, yymsp[-1].minor.yy0, yymsp[0].minor.yy196);
}
#line 1446 "parser.c"
        break;
      case 29: /* stmt ::= FOR s_stmt SEMICOLON oexpr SEMICOLON s_stmt block_stmt */
#line 102 "parser.y"
{
	yygotominor.yy196 = new for_stmt_t(yymsp[-5].minor.yy196, yymsp[-3].minor.yy196, yymsp[-1].minor.yy196, yymsp[0].minor.yy200, yymsp[-6].minor.yy0);
  yy_destructor(yypParser,20,&yymsp[-4].minor);
  yy_destructor(yypParser,20,&yymsp[-2].minor);
}
#line 1455 "parser.c"
        break;
      case 30: /* stmt ::= FOR expr block_stmt */
#line 105 "parser.y"
{
	yygotominor.yy196 = new for_stmt_t(0, yymsp[-1].minor.yy196, 0, yymsp[0].minor.yy200, yymsp[-2].minor.yy0);
}
#line 1462 "parser.c"
        break;
      case 31: /* stmt ::= FOR block_stmt */
#line 108 "parser.y"
{
	yygotominor.yy196 = new for_stmt_t(0, 0, 0, yymsp[0].minor.yy200, yymsp[-1].minor.yy0);
}
#line 1469 "parser.c"
        break;
      case 32: /* stmt ::= SWITCH expr sw_block_stmt */
#line 111 "parser.y"
{
	yygotominor.yy196 = new switch_stmt_t(yymsp[-1].minor.yy196, yymsp[0].minor.yy200, yymsp[-2].minor.yy0);
}
#line 1476 "parser.c"
        break;
      case 33: /* sw_block_stmt ::= LCURLY sw_case_list RCURLY */
#line 123 "parser.y"
{
	yygotominor.yy200 = new block_stmt_t(yymsp[-1].minor.yy38, yymsp[-2].minor.yy0, yymsp[0].minor.yy0);
}
#line 1483 "parser.c"
        break;
      case 34: /* sw_case_list ::= sw_case */
#line 128 "parser.y"
{ yygotominor.yy38 = new node_vector_t(1, yymsp[0].minor.yy138); }
#line 1488 "parser.c"
        break;
      case 35: /* sw_case_list ::= sw_case_list sw_case */
#line 129 "parser.y"
{ yymsp[-1].minor.yy38->push_back(yymsp[0].minor.yy138); yygotominor.yy38 = yymsp[-1].minor.yy38; }
#line 1493 "parser.c"
        break;
      case 36: /* sw_case ::= CASE expr_list COLON stmt_list osemi */
#line 132 "parser.y"
{
	yygotominor.yy138 = new switch_stmt_case_t(yymsp[-3].minor.yy38, yymsp[-1].minor.yy38, yymsp[-4].minor.yy0, yymsp[-2].minor.yy0);
}
#line 1500 "parser.c"
        break;
      case 37: /* sw_case ::= CASE expr_list COLON */
#line 135 "parser.y"
{
	yygotominor.yy138 = new switch_stmt_case_t(yymsp[-1].minor.yy38, 0, yymsp[-2].minor.yy0, yymsp[0].minor.yy0);
}
#line 1507 "parser.c"
        break;
      case 38: /* sw_case ::= DEFAULT COLON stmt_list osemi */
#line 138 "parser.y"
{
	yygotominor.yy138 = new switch_stmt_case_t(0, yymsp[-1].minor.yy38, yymsp[-3].minor.yy0, yymsp[-2].minor.yy0);
}
#line 1514 "parser.c"
        break;
      case 39: /* sw_case ::= DEFAULT COLON */
#line 141 "parser.y"
{
	yygotominor.yy138 = new switch_stmt_case_t(0, 0, yymsp[-1].minor.yy0, yymsp[0].minor.yy0);
}
#line 1521 "parser.c"
        break;
      case 40: /* block_stmt ::= LCURLY stmt_list osemi RCURLY */
#line 153 "parser.y"
{
	yygotominor.yy200 = new block_stmt_t(yymsp[-2].minor.yy38, yymsp[-3].minor.yy0, yymsp[0].minor.yy0);
}
#line 1528 "parser.c"
        break;
      case 41: /* block_stmt ::= LCURLY RCURLY */
#line 156 "parser.y"
{
	yygotominor.yy200 = new block_stmt_t(0, yymsp[-1].minor.yy0, yymsp[0].minor.yy0);
}
#line 1535 "parser.c"
        break;
      case 42: /* decl_list ::= decl */
      case 134: /* iexpr_list ::= iexpr */ yytestcase(yyruleno==134);
      case 136: /* expr_list ::= expr */ yytestcase(yyruleno==136);
#line 168 "parser.y"
{ yygotominor.yy38 = new node_vector_t(1, yymsp[0].minor.yy196); }
#line 1542 "parser.c"
        break;
      case 43: /* decl_list ::= decl_list SEMICOLON decl */
#line 169 "parser.y"
{ yymsp[-2].minor.yy38->push_back(yymsp[0].minor.yy196); yygotominor.yy38 = yymsp[-2].minor.yy38;   yy_destructor(yypParser,20,&yymsp[-1].minor);
}
#line 1548 "parser.c"
        break;
      case 44: /* decl ::= IMPORT import_spec */
#line 185 "parser.y"
{
	yygotominor.yy196 = new import_decl_t(yymsp[0].minor.yy79, yymsp[-1].minor.yy0);
}
#line 1555 "parser.c"
        break;
      case 45: /* decl ::= IMPORT LPAREN import_spec_list osemi RPAREN */
#line 188 "parser.y"
{
	yygotominor.yy196 = new import_decl_t(yymsp[-2].minor.yy69, yymsp[-4].minor.yy0, yymsp[-3].minor.yy0, yymsp[0].minor.yy0);
}
#line 1562 "parser.c"
        break;
      case 46: /* decl ::= TYPE type_spec */
#line 191 "parser.y"
{
	yygotominor.yy196 = new type_decl_t(yymsp[0].minor.yy90, yymsp[-1].minor.yy0);
}
#line 1569 "parser.c"
        break;
      case 47: /* decl ::= TYPE LPAREN type_spec_list osemi RPAREN */
#line 194 "parser.y"
{
	yygotominor.yy196 = new type_decl_t(yymsp[-2].minor.yy164, yymsp[-4].minor.yy0, yymsp[-3].minor.yy0, yymsp[0].minor.yy0);
}
#line 1576 "parser.c"
        break;
      case 48: /* decl ::= CONST value_spec */
#line 197 "parser.y"
{
	const_decl_t *d = new const_decl_t(yymsp[0].minor.yy157, yymsp[-1].minor.yy0);
	syntax_check_const_decl(ctx->diag, d);
	yygotominor.yy196 = d;
}
#line 1585 "parser.c"
        break;
      case 49: /* decl ::= CONST LPAREN value_spec_list osemi RPAREN */
#line 202 "parser.y"
{
	const_decl_t *d = new const_decl_t(yymsp[-2].minor.yy187, yymsp[-4].minor.yy0, yymsp[-3].minor.yy0, yymsp[0].minor.yy0);
	syntax_check_const_decl(ctx->diag, d);
	yygotominor.yy196 = d;
}
#line 1594 "parser.c"
        break;
      case 50: /* decl ::= VAR value_spec */
#line 207 "parser.y"
{
	var_decl_t *d = new var_decl_t(yymsp[0].minor.yy157, yymsp[-1].minor.yy0);
	syntax_check_var_decl(ctx->diag, d);
	yygotominor.yy196 = d;
}
#line 1603 "parser.c"
        break;
      case 51: /* decl ::= VAR LPAREN value_spec_list osemi RPAREN */
#line 212 "parser.y"
{
	var_decl_t *d = new var_decl_t(yymsp[-2].minor.yy187, yymsp[-4].minor.yy0, yymsp[-3].minor.yy0, yymsp[0].minor.yy0);
	syntax_check_var_decl(ctx->diag, d);
	yygotominor.yy196 = d;
}
#line 1612 "parser.c"
        break;
      case 52: /* decl ::= FUNC ident LPAREN oargs_comma_list RPAREN func_results */
#line 220 "parser.y"
{
	func_type_t *ftype = new func_type_t(yymsp[-2].minor.yy104, yymsp[0].minor.yy104, yymsp[-5].minor.yy0);
	yygotominor.yy196 = new func_decl_t(yymsp[-4].minor.yy92, ftype);
  yy_destructor(yypParser,50,&yymsp[-3].minor);
  yy_destructor(yypParser,51,&yymsp[-1].minor);
}
#line 1622 "parser.c"
        break;
      case 53: /* decl ::= FUNC ident LPAREN oargs_comma_list RPAREN func_results block_stmt */
#line 227 "parser.y"
{
	func_type_t *ftype = new func_type_t(yymsp[-3].minor.yy104, yymsp[-1].minor.yy104, yymsp[-6].minor.yy0);
	yygotominor.yy196 = new func_decl_t(yymsp[-5].minor.yy92, ftype, yymsp[0].minor.yy200);
  yy_destructor(yypParser,50,&yymsp[-4].minor);
  yy_destructor(yypParser,51,&yymsp[-2].minor);
}
#line 1632 "parser.c"
        break;
      case 54: /* func_results ::= type */
#line 240 "parser.y"
{
	field_t *f = new field_t(0, yymsp[0].minor.yy196);
	yygotominor.yy104 = new field_vector_t(1, f);
}
#line 1640 "parser.c"
        break;
      case 55: /* func_results ::= LPAREN field_comma_list RPAREN */
#line 244 "parser.y"
{ yygotominor.yy104 = yymsp[-1].minor.yy104;   yy_destructor(yypParser,50,&yymsp[-2].minor);
  yy_destructor(yypParser,51,&yymsp[0].minor);
}
#line 1647 "parser.c"
        break;
      case 56: /* func_results ::= */
      case 149: /* oargs_comma_list ::= */ yytestcase(yyruleno==149);
      case 151: /* ofield_semi_list_and_osemi ::= */ yytestcase(yyruleno==151);
#line 245 "parser.y"
{ yygotominor.yy104 = 0; }
#line 1654 "parser.c"
        break;
      case 57: /* import_spec_list ::= import_spec */
#line 258 "parser.y"
{ yygotominor.yy69 = new import_spec_vector_t(1, yymsp[0].minor.yy79); }
#line 1659 "parser.c"
        break;
      case 58: /* import_spec_list ::= import_spec_list SEMICOLON import_spec */
#line 259 "parser.y"
{
	yymsp[-2].minor.yy69->push_back(yymsp[0].minor.yy79); yygotominor.yy69 = yymsp[-2].minor.yy69;
  yy_destructor(yypParser,20,&yymsp[-1].minor);
}
#line 1667 "parser.c"
        break;
      case 59: /* import_spec ::= IDENT STRING */
#line 264 "parser.y"
{ yygotominor.yy79 = new import_spec_t(yymsp[-1].minor.yy0, yymsp[0].minor.yy0); }
#line 1672 "parser.c"
        break;
      case 60: /* import_spec ::= STRING */
#line 265 "parser.y"
{ yygotominor.yy79 = new import_spec_t(0, yymsp[0].minor.yy0); }
#line 1677 "parser.c"
        break;
      case 61: /* type_spec_list ::= type_spec */
#line 278 "parser.y"
{ yygotominor.yy164 = new type_spec_vector_t(1, yymsp[0].minor.yy90); }
#line 1682 "parser.c"
        break;
      case 62: /* type_spec_list ::= type_spec_list SEMICOLON type_spec */
#line 279 "parser.y"
{
	yymsp[-2].minor.yy164->push_back(yymsp[0].minor.yy90); yygotominor.yy164 = yymsp[-2].minor.yy164;
  yy_destructor(yypParser,20,&yymsp[-1].minor);
}
#line 1690 "parser.c"
        break;
      case 63: /* type_spec ::= IDENT type */
#line 284 "parser.y"
{ yygotominor.yy90 = new type_spec_t(yymsp[-1].minor.yy0, yymsp[0].minor.yy196); }
#line 1695 "parser.c"
        break;
      case 64: /* value_spec_list ::= value_spec */
#line 299 "parser.y"
{ yygotominor.yy187 = new value_spec_vector_t(1, yymsp[0].minor.yy157); }
#line 1700 "parser.c"
        break;
      case 65: /* value_spec_list ::= value_spec_list SEMICOLON value_spec */
#line 300 "parser.y"
{
	yymsp[-2].minor.yy187->push_back(yymsp[0].minor.yy157); yygotominor.yy187 = yymsp[-2].minor.yy187;
  yy_destructor(yypParser,20,&yymsp[-1].minor);
}
#line 1708 "parser.c"
        break;
      case 66: /* value_spec ::= ident_list otype */
#line 305 "parser.y"
{
	yygotominor.yy157 = new value_spec_t(yymsp[-1].minor.yy222, yymsp[0].minor.yy196);
}
#line 1715 "parser.c"
        break;
      case 67: /* value_spec ::= ident_list otype ASSIGN iexpr_list */
#line 308 "parser.y"
{
	yygotominor.yy157 = new value_spec_t(yymsp[-3].minor.yy222, yymsp[-2].minor.yy196, yymsp[0].minor.yy38, yymsp[-1].minor.yy0);
}
#line 1722 "parser.c"
        break;
      case 68: /* type ::= TIMES type */
#line 329 "parser.y"
{ yygotominor.yy196 = new pointer_type_t(yymsp[0].minor.yy196, yymsp[-1].minor.yy0); }
#line 1727 "parser.c"
        break;
      case 69: /* type ::= LSB expr RSB type */
#line 330 "parser.y"
{ yygotominor.yy196 = new array_type_t(yymsp[-2].minor.yy196, yymsp[0].minor.yy196, yymsp[-3].minor.yy0, yymsp[-1].minor.yy0); }
#line 1732 "parser.c"
        break;
      case 70: /* type ::= LSB RSB type */
#line 331 "parser.y"
{ yygotominor.yy196 = new array_type_t(0, yymsp[0].minor.yy196, yymsp[-2].minor.yy0, yymsp[-1].minor.yy0); }
#line 1737 "parser.c"
        break;
      case 71: /* type ::= ident DOT ident */
#line 332 "parser.y"
{ yygotominor.yy196 = new selector_expr_t(yymsp[-2].minor.yy92, yymsp[0].minor.yy92);   yy_destructor(yypParser,60,&yymsp[-1].minor);
}
#line 1743 "parser.c"
        break;
      case 72: /* type ::= ident */
      case 94: /* pexpr ::= ident */ yytestcase(yyruleno==94);
#line 333 "parser.y"
{ yygotominor.yy196 = yymsp[0].minor.yy92; }
#line 1749 "parser.c"
        break;
      case 73: /* type ::= STRUCT LCURLY ofield_semi_list_and_osemi RCURLY */
      case 74: /* type ::= UNION LCURLY ofield_semi_list_and_osemi RCURLY */ yytestcase(yyruleno==74);
#line 334 "parser.y"
{
	yygotominor.yy196 = new struct_type_t(yymsp[-1].minor.yy104, yymsp[-3].minor.yy0, yymsp[-2].minor.yy0, yymsp[0].minor.yy0);	
}
#line 1757 "parser.c"
        break;
      case 75: /* type ::= FUNC LPAREN oargs_comma_list RPAREN func_results */
#line 340 "parser.y"
{
	yygotominor.yy196 = new func_type_t(yymsp[-2].minor.yy104, yymsp[0].minor.yy104, yymsp[-4].minor.yy0);
  yy_destructor(yypParser,50,&yymsp[-3].minor);
  yy_destructor(yypParser,51,&yymsp[-1].minor);
}
#line 1766 "parser.c"
        break;
      case 76: /* field_comma_list ::= nametype_list */
#line 355 "parser.y"
{
	yygotominor.yy104 = nametypev_to_fieldv(ctx->diag, yymsp[0].minor.yy87, false);
}
#line 1773 "parser.c"
        break;
      case 77: /* args_comma_list ::= nametype_list ofunc_ellipsis */
#line 360 "parser.y"
{
	yygotominor.yy104 = nametypev_to_fieldv(ctx->diag, yymsp[-1].minor.yy87, yymsp[0].minor.yy89);
}
#line 1780 "parser.c"
        break;
      case 78: /* args_comma_list ::= ELLIPSIS */
#line 363 "parser.y"
{
	yygotominor.yy104 = nametypev_to_fieldv(ctx->diag, new nametype_vector_t, true);
  yy_destructor(yypParser,63,&yymsp[0].minor);
}
#line 1788 "parser.c"
        break;
      case 79: /* field_semi_list ::= field */
#line 368 "parser.y"
{ yygotominor.yy104 = new field_vector_t(1, yymsp[0].minor.yy174); }
#line 1793 "parser.c"
        break;
      case 80: /* field_semi_list ::= field_semi_list SEMICOLON field */
#line 369 "parser.y"
{
	yymsp[-2].minor.yy104->push_back(yymsp[0].minor.yy174); yygotominor.yy104 = yymsp[-2].minor.yy104;
  yy_destructor(yypParser,20,&yymsp[-1].minor);
}
#line 1801 "parser.c"
        break;
      case 81: /* nametype_list ::= nametype */
#line 375 "parser.y"
{ yygotominor.yy87 = new nametype_vector_t(1, yymsp[0].minor.yy103); }
#line 1806 "parser.c"
        break;
      case 82: /* nametype_list ::= nametype_list COMMA nametype */
#line 376 "parser.y"
{
	yymsp[-2].minor.yy87->push_back(yymsp[0].minor.yy103); yygotominor.yy87 = yymsp[-2].minor.yy87;
  yy_destructor(yypParser,64,&yymsp[-1].minor);
}
#line 1814 "parser.c"
        break;
      case 83: /* nametype ::= ident type */
#line 381 "parser.y"
{ yygotominor.yy103.name = yymsp[-1].minor.yy92; yygotominor.yy103.type = yymsp[0].minor.yy196; }
#line 1819 "parser.c"
        break;
      case 84: /* nametype ::= type */
#line 382 "parser.y"
{ yygotominor.yy103.name = 0; yygotominor.yy103.type = yymsp[0].minor.yy196; }
#line 1824 "parser.c"
        break;
      case 85: /* ofunc_ellipsis ::= COMMA ELLIPSIS */
#line 392 "parser.y"
{ yygotominor.yy89 = true;   yy_destructor(yypParser,64,&yymsp[-1].minor);
  yy_destructor(yypParser,63,&yymsp[0].minor);
}
#line 1831 "parser.c"
        break;
      case 86: /* ofunc_ellipsis ::= */
#line 393 "parser.y"
{ yygotominor.yy89 = false; }
#line 1836 "parser.c"
        break;
      case 87: /* field ::= ident_list type */
#line 406 "parser.y"
{ yygotominor.yy174 = new field_t(yymsp[-1].minor.yy222, yymsp[0].minor.yy196); }
#line 1841 "parser.c"
        break;
      case 88: /* field ::= type */
#line 407 "parser.y"
{ yygotominor.yy174 = new field_t(0, yymsp[0].minor.yy196); }
#line 1846 "parser.c"
        break;
      case 89: /* basic_lit ::= INT */
      case 90: /* basic_lit ::= FLOAT */ yytestcase(yyruleno==90);
      case 91: /* basic_lit ::= STRING */ yytestcase(yyruleno==91);
      case 92: /* basic_lit ::= CHAR */ yytestcase(yyruleno==92);
#line 418 "parser.y"
{ yygotominor.yy82 = new basic_lit_expr_t(yymsp[0].minor.yy0); }
#line 1854 "parser.c"
        break;
      case 93: /* pexpr ::= basic_lit */
#line 434 "parser.y"
{ yygotominor.yy196 = yymsp[0].minor.yy82; }
#line 1859 "parser.c"
        break;
      case 95: /* pexpr ::= LPAREN expr RPAREN */
#line 436 "parser.y"
{ yygotominor.yy196 = new paren_expr_t(yymsp[-1].minor.yy196, yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1864 "parser.c"
        break;
      case 96: /* pexpr ::= pexpr LSB expr RSB */
#line 437 "parser.y"
{ yygotominor.yy196 = new index_expr_t(yymsp[-3].minor.yy196, yymsp[-1].minor.yy196, yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1869 "parser.c"
        break;
      case 97: /* pexpr ::= pexpr DOT IDENT */
#line 440 "parser.y"
{
	yygotominor.yy196 = new selector_expr_t(yymsp[-2].minor.yy196, new ident_expr_t(yymsp[0].minor.yy0));
  yy_destructor(yypParser,60,&yymsp[-1].minor);
}
#line 1877 "parser.c"
        break;
      case 98: /* pexpr ::= pexpr DOT LPAREN type RPAREN */
#line 445 "parser.y"
{
	yygotominor.yy196 = new type_cast_expr_t(yymsp[-4].minor.yy196, yymsp[-1].minor.yy196, yymsp[-3].minor.yy0, yymsp[-2].minor.yy0, yymsp[0].minor.yy0);
}
#line 1884 "parser.c"
        break;
      case 99: /* pexpr ::= pexpr LPAREN RPAREN */
#line 451 "parser.y"
{ yygotominor.yy196 = new call_expr_t(yymsp[-2].minor.yy196, 0, yymsp[-1].minor.yy0, yymsp[0].minor.yy0); }
#line 1889 "parser.c"
        break;
      case 100: /* pexpr ::= pexpr LPAREN iexpr_list RPAREN */
#line 454 "parser.y"
{
	yygotominor.yy196 = new call_expr_t(yymsp[-3].minor.yy196, yymsp[-1].minor.yy38, yymsp[-2].minor.yy0, yymsp[0].minor.yy0);
}
#line 1896 "parser.c"
        break;
      case 102: /* uexpr ::= PLUS uexpr */
      case 103: /* uexpr ::= MINUS uexpr */ yytestcase(yyruleno==103);
      case 104: /* uexpr ::= NOT uexpr */ yytestcase(yyruleno==104);
      case 105: /* uexpr ::= AND uexpr */ yytestcase(yyruleno==105);
      case 106: /* uexpr ::= TIMES uexpr */ yytestcase(yyruleno==106);
      case 107: /* uexpr ::= XOR uexpr */ yytestcase(yyruleno==107);
#line 469 "parser.y"
{ yygotominor.yy196 = new unary_expr_t(yymsp[0].minor.yy196, yymsp[-1].minor.yy0); }
#line 1906 "parser.c"
        break;
      case 109: /* expr ::= expr DIVIDE expr */
      case 110: /* expr ::= expr TIMES expr */ yytestcase(yyruleno==110);
      case 111: /* expr ::= expr MOD expr */ yytestcase(yyruleno==111);
      case 112: /* expr ::= expr SHIFTL expr */ yytestcase(yyruleno==112);
      case 113: /* expr ::= expr SHIFTR expr */ yytestcase(yyruleno==113);
      case 114: /* expr ::= expr AND expr */ yytestcase(yyruleno==114);
      case 115: /* expr ::= expr ANDNOT expr */ yytestcase(yyruleno==115);
      case 116: /* expr ::= expr PLUS expr */ yytestcase(yyruleno==116);
      case 117: /* expr ::= expr MINUS expr */ yytestcase(yyruleno==117);
      case 118: /* expr ::= expr OR expr */ yytestcase(yyruleno==118);
      case 119: /* expr ::= expr XOR expr */ yytestcase(yyruleno==119);
      case 120: /* expr ::= expr EQ expr */ yytestcase(yyruleno==120);
      case 121: /* expr ::= expr NEQ expr */ yytestcase(yyruleno==121);
      case 122: /* expr ::= expr LT expr */ yytestcase(yyruleno==122);
      case 123: /* expr ::= expr LE expr */ yytestcase(yyruleno==123);
      case 124: /* expr ::= expr GT expr */ yytestcase(yyruleno==124);
      case 125: /* expr ::= expr GE expr */ yytestcase(yyruleno==125);
      case 126: /* expr ::= expr ANDAND expr */ yytestcase(yyruleno==126);
      case 127: /* expr ::= expr OROR expr */ yytestcase(yyruleno==127);
#line 487 "parser.y"
{ yygotominor.yy196 = new binary_expr_t(yymsp[-2].minor.yy196, yymsp[0].minor.yy196, yymsp[-1].minor.yy0); }
#line 1929 "parser.c"
        break;
      case 128: /* compound_lit ::= LCURLY iexpr_list ocomma RCURLY */
#line 518 "parser.y"
{
	yygotominor.yy196 = new compound_lit_t(yymsp[-2].minor.yy38, 0, yymsp[-3].minor.yy0, yymsp[0].minor.yy0);
}
#line 1936 "parser.c"
        break;
      case 129: /* compound_lit ::= LCURLY iexpr_list ocomma RCURLY DOT LPAREN type RPAREN */
#line 521 "parser.y"
{
	yygotominor.yy196 = new compound_lit_t(yymsp[-6].minor.yy38, yymsp[-1].minor.yy196, yymsp[-7].minor.yy0, yymsp[0].minor.yy0);
  yy_destructor(yypParser,45,&yymsp[-4].minor);
  yy_destructor(yypParser,60,&yymsp[-3].minor);
  yy_destructor(yypParser,50,&yymsp[-2].minor);
}
#line 1946 "parser.c"
        break;
      case 130: /* type_expr ::= TYPE type */
#line 535 "parser.y"
{ yygotominor.yy196 = new type_expr_t(yymsp[0].minor.yy196, yymsp[-1].minor.yy0); }
#line 1951 "parser.c"
        break;
      case 135: /* iexpr_list ::= iexpr_list COMMA iexpr */
      case 137: /* expr_list ::= expr_list COMMA expr */ yytestcase(yyruleno==137);
#line 553 "parser.y"
{ yymsp[-2].minor.yy38->push_back(yymsp[0].minor.yy196); yygotominor.yy38 = yymsp[-2].minor.yy38;   yy_destructor(yypParser,64,&yymsp[-1].minor);
}
#line 1958 "parser.c"
        break;
      case 139: /* osemi ::= SEMICOLON */
#line 568 "parser.y"
{
  yy_destructor(yypParser,20,&yymsp[0].minor);
}
#line 1965 "parser.c"
        break;
      case 141: /* ocomma ::= COMMA */
#line 572 "parser.y"
{
  yy_destructor(yypParser,64,&yymsp[0].minor);
}
#line 1972 "parser.c"
        break;
      case 142: /* oexpr ::= */
      case 144: /* otype ::= */ yytestcase(yyruleno==144);
#line 576 "parser.y"
{ yygotominor.yy196 = 0; }
#line 1978 "parser.c"
        break;
      case 146: /* ident ::= IDENT */
#line 591 "parser.y"
{ yygotominor.yy92 = new ident_expr_t(yymsp[0].minor.yy0); }
#line 1983 "parser.c"
        break;
      case 147: /* ident_list ::= ident */
#line 595 "parser.y"
{ yygotominor.yy222 = new ident_expr_vector_t(1, yymsp[0].minor.yy92); }
#line 1988 "parser.c"
        break;
      case 148: /* ident_list ::= ident_list COMMA ident */
#line 596 "parser.y"
{ yymsp[-2].minor.yy222->push_back(yymsp[0].minor.yy92); yygotominor.yy222 = yymsp[-2].minor.yy222;   yy_destructor(yypParser,64,&yymsp[-1].minor);
}
#line 1994 "parser.c"
        break;
      case 150: /* oargs_comma_list ::= args_comma_list */
#line 601 "parser.y"
{ yygotominor.yy104 = yymsp[0].minor.yy104; }
#line 1999 "parser.c"
        break;
      case 152: /* ofield_semi_list_and_osemi ::= field_semi_list osemi */
#line 606 "parser.y"
{ yygotominor.yy104 = yymsp[-1].minor.yy104; }
#line 2004 "parser.c"
        break;
      default:
      /* (138) osemi ::= */ yytestcase(yyruleno==138);
      /* (140) ocomma ::= */ yytestcase(yyruleno==140);
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
#line 2076 "parser.c"
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
