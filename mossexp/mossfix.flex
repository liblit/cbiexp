/*

    MOSS:  Measure of Software Similarity
         
*/



%{
/*  C, C++, Perl keywords and other fixed tokens */

#include "lib/regions.h"
#include "reporting.h"
#define ralloc rstralloc

typedef enum {
  STORE,
  QUERY_NO_DUPLICATES,
  QUERY_SHOW_DUPLICATES
} query_kinds;

typedef enum {
  C_INT = 1, 
  C_CHAR, 
  C_STRING, 
  C_NO4,
  C_DOUBLE, 
  C_FLOAT, 
  C_VAR,
  C_NO8,
  C_SHORT, 
  C_EXTERN, 
  C_NUM,
  C_VOID, 
  C_CHARC,
  C_DOT, 
  C_ARROW, 
  C_NO16,
  C_FOR, 
  C_IF, 
  C_ELSE,
  C_WHILE, 
  C_ENUM, 
  C_RETURN, 
  C_STATIC, 
  C_STRUCT, 
  C_TYPEDEF, 
  C_UNION, 
  C_UNSIGNED,
  C_BREAK, 
  C_CONST, 
  C_CONTINUE, 
  C_NO32,
  C_DO, 
  C_ASSIGN, 
  C_CASE, 
  C_SWITCH, 
  C_SEMICOLON, 
  C_STAR, 
  C_PP, 
  C_MM, 
  C_AA, 
  C_AND,
  C_OO, 
  C_OR, 
  C_TILDE, 
  C_COLONCOLON, 
  C_LESS, 
  C_GREATER, 
  C_EQUAL, 
  C_NEQ, 
  C_LBRACK, 
  C_COMMA,
  C_RBRACK,
  C_LPAREN,
  C_QUEST,
  C_MINUS,
  C_PLUS,
  C_RPAREN,
  C_BANG,
  C_SLASH,
  C_COLON,
  C_PERCENT,
  C_HASH,
  C_LONG, 
  C_SIZEOF, 
  C_VOLATILE,
  C_FUNCTION,
  C_OTHER,
  PERL_DOLLARSIGN,
  PERL_SYSTEM,
  PERL_BUILTIN
}
Ckeys;

typedef enum {
  SPICE_TILDE = 1,
  SPICE_NO2,
  SPICE_AC,
  SPICE_DC,
  SPICE_MODEL,
  SPICE_END,
  SPICE_ENDS,
  SPICE_DISTO,
  SPICE_COMMA,
  SPICE_STRING,
  SPICE_CHARC,
  SPICE_NUM,
  SPICE_EQ,
  SPICE_GREAT,
  SPICE_LESS,
  SPICE_COLON,
  SPICE_PLUS,
  SPICE_MINUS,
  SPICE_STAR,
  SPICE_SLASH,
  SPICE_OTHER,
  SPICE_VAR
}

SpiceKeys;
typedef enum {
  F_INT = 1,
  F_CHAR,
  F_STRING,
  F_NO4,
  F_VAR,
  F_EQ,
  F_DOUBLEAST,
  F_AST,
  F_SLASH,
  F_PLUS,
  F_MINUS,
  F_DOUBLESLASH,
  F_RELOP,
  F_DOT,
  F_TRUE,
  F_FALSE,
  F_TYPE,
  F_PRIVATE,
  F_SEQUENCE,
  F_POINTER,
  F_ALLOCATABLE,
  F_DIMENSION,
  F_END,
  F_TYPEALIAS,
  F_BIND,
  F_NAME,
  F_ENUM,
  F_ENUMERATOR,
  F_INTEGER,
  F_REAL,
  F_DOUBLE,
  F_COMPLEX,
  F_PRECISION,
  F_CHARACTER,
  F_LOGICAL,
  F_PARAMETER,
  F_AUTOMATIC,
  F_EXTERNAL,
  F_INTENT,
  F_INTRINSIC,
  F_SAVE,
  F_STATIC,
  F_TARGET,
  F_VALUE,
  F_VOLATILE,
  F_LEN,
  F_KIND,
  F_IN,
  F_OUT,
  F_INOUT,
  F_EQUIVALENCE,
  F_COMMON,
  F_WHERE,
  F_FORALL,
  F_IF,
  F_ELSE,
  F_THEN,
  F_SELECT,
  F_CASE, 
  F_DO,
  F_GO,
  F_TO,
  F_COMMA,
  F_OTHER
} FORTRANkeys;
  

/* Java keywords and other fixed tokens */
typedef enum {
  J_INT = 1, 
  J_CHAR, 
  J_STRING, 
  J_NO4,
  J_DOUBLE, 
  J_FLOAT, 
  J_VAR,
  J_NO8,
  J_SHORT, 
  J_BYTE,
  J_NUM,
  J_VOID, 
  J_DOT, 
  J_CHARC,
  J_THROW,
  J_NO16,
  J_IF, 
  J_FOR, 
  J_ELSE, 
  J_WHILE, 
  J_SUPER, 
  J_RETURN, 
  J_STATIC, 
  J_FINALLY, 
  J_NATIVE, 
  J_TRY, 
  J_THIS,
  J_BREAK, 
  J_CATCH, 
  J_CONTINUE, 
  J_DO, 
  J_NO32,
  J_ASSIGN, 
  J_CASE, 
  J_STAR, 
  J_SWITCH, 
  J_SEMICOLON, 
  J_PP, 
  J_MM, 
  J_AA, 
  J_AND,
  J_OO, 
  J_OR, 
  J_TILDE, 
  J_NEW, 
  J_LESS, 
  J_GREATER, 
  J_EQUAL, 
  J_LBRACK, 
  J_NEQ, 
  J_COMMA,
  J_RBRACK,
  J_LPAREN,
  J_QUEST,
  J_MINUS,
  J_PLUS,
  J_RPAREN,
  J_BANG,
  J_BOOLEAN,
  J_COLON,
  J_OTHER,
  J_PERCENT,
  J_HASH,
  J_LONG, 
  J_TRUE, 
  J_FALSE
}
Jkeys;



/* ML keywords and other fixed tokens */
typedef enum {
  ML_BOOL = 1,
  ML_CHAR,
  ML_ARRAY,
  ML_NO4,
  ML_INT,
  ML_REAL,
  ML_VAR,
  ML_NO8,
  ML_REF,
  ML_PRIME,
  ML_UNIT,
  ML_VECTOR,
  ML_WORD,
  ML_STRINGT,
  ML_EXN,
  ML_NO16,
  ML_STRINGC,
  ML_BANG,
  ML_NIL,
  ML_PLUS,
  ML_MINUS,
  ML_CARAT,
  ML_CONS,
  ML_CAT,
  ML_EQ,
  ML_GREATER,
  ML_LESS,
  ML_ASSIGN,
  ML_CHARC,
  ML_NUM,
  ML_OTHER,
  ML_ABSTRACTION,
  ML_ABSTYPE,
  ML_AND,
  ML_ANDALSO,
  ML_AS,
  ML_CASE,
  ML_DO,
  ML_DATATYPE,
  ML_ELSE,
  ML_END,
  ML_EXCEPTION,
  ML_FN,
  ML_FUN,
  ML_HANDLE,
  ML_IF,
  ML_IN,
  ML_INFIX,
  ML_INFIXR,
  ML_LET,
  ML_LOCAL,
  ML_NONFIX,
  ML_OF,
  ML_OP,
  ML_OPEN,
  ML_ORELSE,
  ML_RAISE,
  ML_REC,
  ML_SIG,
  ML_SIGNATURE,
  ML_STRUCT,
  ML_STRUCTURE,
  ML_THEN,
  ML_TYPE,
  ML_VALS,
  ML_WITH,
  ML_WITHTYPE,
  ML_WHILE,
  ML_LPAREN,
  ML_RPAREN,
  ML_LBRACK,
  ML_RBRACK,
  ML_LCBRACK,
  ML_RCBRACK,
  ML_COMMA,
  ML_COLON,
  ML_COLONGREATER,
  ML_SEMICOLON,
  ML_ELIPSES,
  ML_UNDERBAR,
  ML_VERTICALBAR,
  ML_DOUBLEARROW,
  ML_SINGLEARROW,
  ML_HASH
}
MLkeys;

/* Haskell keywords and other fixed tokens */

typedef enum {
  HASKELL_CHARC,
  HASKELL_STRINGC,
  HASKELL_NUM,
  HASKELL_NO4,
  HASKELL_DOUBLEDOT,
  HASKELL_DOUBLECOLON,
  HASKELL_COLON,
  HASKELL_NO8,
  HASKELL_EQ,
  HASKELL_BACKSLASH,
  HASKELL_AT,
  HASKELL_TILDE,
  HASKELL_MINUS,
  HASKELL_VERTICALBAR,
  HASKELL_DOUBLEARROW,
  HASKELL_NO16,
  HASKELL_RIGHTARROW,
  HASKELL_LEFTARROW,
  HASKELL_CASE,
  HASKELL_CLASS,
  HASKELL_DATA,
  HASKELL_DEFAULT,
  HASKELL_DERIVING,
  HASKELL_DO,
  HASKELL_ELSE,
  HASKELL_IF,
  HASKELL_IMPORT,
  HASKELL_IN,
  HASKELL_INFIX,
  HASKELL_LET,
  HASKELL_MODULE,
  HASKELL_NO32,
  HASKELL_NEWTYPE,
  HASKELL_OF,
  HASKELL_THEN,
  HASKELL_TYPE,
  HASKELL_WHERE,
  HASKELL_AS,
  HASKELL_QUALIFIED,
  HASKELL_HIDING,
  HASKELL_PLUS,
  HASKELL_LBRACK,
  HASKELL_RBRACK,
  HASKELL_COMMA,
  HASKELL_ELLIPSES,
  HASKELL_UNDERBAR,
  HASKELL_VAR,
  HASKELL_OTHER
}
Haskellkeys;

typedef enum {
  LISP_CAR = 1,
  LISP_CDR,
  LISP_CADR,
  LISP_CDDR,
  LISP_OTHER,
  LISP_FIRSTCODE
}
Lkeys;

@include "include/MODULA2.enum"
@include "include/A8086.enum"
@include "include/PASCAL.enum"
@include "include/ADA.enum"
@include "include/VHDL.enum"
@include "include/PYTHON.enum"
@include "include/MIPS.enum"
@include "include/CSHARP.enum"
@include "include/PROLOG.enum"
@include "include/VB.enum"
@include "include/JAVASCRIPT.enum"
@include "include/PLSQL.enum"

/*
The languages we can handle.

LangTest matches all bytes except for newlines; this is an interface for testing.
*/
 typedef enum {LangNone,
@include "include/languages"
	LangC, LangCC, LangJava, LangML, LangLisp, LangScheme, 
	      LangFortran, LangASCII, LangTest, 
	      LangPerl, LangHaskell, LangMatlab, LangHTML, 
	      LangSpice} Lang;

   
/********************************************************************

Most global state is kept in a configuration structure config.
A few extra variables are left global because flex cannot take inputs.

********************************************************************/

/************************************************************************************

CONFIGURATIONS

Parameter settings that affect a session are stored in a configuration struct.

*************************************************************************************/

typedef struct configuration {
    int token_window_size;
    int tile_size;    /* always 1 less than token_window_size */
    int max_matches;  /* maximum matches before a tile is discarded */
    int winnowing_window_size;
    int nomatch_index;
    int nomatch_set;
    int print_stats;  /* non-zero if we should print statistics */
    FILE *outfile;
    FILE *errfile;
    char *dbfile;     /* file to which we will dump the database */
    char *auxfile;    /* list of additional options and files */
    char *manifest;   /* a better kind of auxfile */
    Lang language;
    int gap;          /* permitted gap between two passages so that they can still be merged */
    int user_max;     /* did the user set max matches? */
    int cap;
    int match_comment;	
    int passage_power; /* exponent x in 2^x size of passage array */
    region mem_strings;	
    region mem_database;
    region mem_index;	
    region mem_tokenization;
    region mem_matches;
} configuration;

configuration config;

FILE *fin;	      /* current input file for flex */

/* define YY_INPUT so we read from the FILE fin. */
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
        if ( (result = fread( (char*)buf, sizeof(char), max_size, fin)) < 0) \
                YY_FATAL_ERROR( "read() in flex scanner failed");


static char *filename;        		/* name of currently open file */
static int last = 0;          		/* record last switch to function scope; for debugging. */
static int lispscope, htmlscope;        /* how many scopes are we in? */
static int lineno = 1;                  /* current line number of the program */
static int token_count = 0;             /* count of total number of tokens in the program */


/* for internal use by the rules only */
static int nextstate;

void code(int);
void code_dynamic(char *);
void startfun();

#define CODE_DYNAMIC_AND_RETURN(string)  code_dynamic(string); return(1)
#define CODE_AND_RETURN(string)  if (DEBUG) fprintf(stderr,"string: %s\n",yytext); code(string); return(1)

%}
%option noyywrap
%START C
%START CCOMMENT
%START C_MATCHCOMMENT
%START CFUNCTION
%START JAVA
%START ML
%START MLCOMMENT
%START LISP
%START LISPFUNCTION
%START LISPSTRING
%START FORTRAN
%START FORTRANCOMMENT
%START PCOMMENT1
%START PCOMMENT2
%START IGNORELINE
%START MATCHLINE
%START ASCII
%START TEST
%START PERL
%START HASKELL
%START HASKELLCOMMENT
%START MATLAB
%START HTML
%START HTMLTAG
%START SPICE
@include "include/starts"
%%
 /*
   This rule is used only once for each file to determine what language we are using
   and to reset global variables.

   We must match a character, but we just push it back onto the input.
 */
<INITIAL>.|\n				{  switch (config.language) {
                                             case LangC:
                                             case LangCC: BEGIN(C); break;
					     case LangML: BEGIN(ML); break;
					     case LangJava: BEGIN(JAVA); break;
					     case LangLisp:
					     case LangScheme: BEGIN(LISP); break;
					     case LangFortran: BEGIN(FORTRAN); break;
					     case LangASCII: BEGIN(ASCII); break;
					     case LangTest: BEGIN(TEST); break;
					     case LangPerl: BEGIN(PERL); break;
					     case LangHaskell: BEGIN(HASKELL); break;
					     case LangMatlab: BEGIN(MATLAB); break;
					     case LangHTML: BEGIN(HTML); break;
					     case LangSpice: BEGIN(SPICE); break;
                                             case LangNone: fprintf(stderr,"No language specified."); exit(1); break;			
@include "include/lang.choose"
					  }
                                          unput(*yytext);
                                          lineno = 1;       /* line number */
                                          token_count = 0;  /* reset count of tokens */
                                        }

<IGNORELINE>.				;
<IGNORELINE>\n				{ lineno++; BEGIN(nextstate);}

<MATCHLINE>\n				{ lineno++; BEGIN(nextstate); }
<MATCHLINE>[ \t\r]*			{ }
<MATCHLINE>.				{ CODE_AND_RETURN((int) *yytext); } 


 /****************************************************************************

    Test interface: Files are left uninterpreted.

 *****************************************************************************/

<TEST>\n				{ lineno++; }
<TEST>\>\>\>\>                          { nextstate = TEST; BEGIN(IGNORELINE); }
<TEST>.					{ if (DEBUG) 
						printf("\n\nline: %d  char: %c\n",lineno,*yytext); 
					  CODE_AND_RETURN((int) *yytext); }


 /****************************************************************************

     ASCII interface: Discard whitespace, vowels, and s

 *****************************************************************************/
<ASCII>\>\>\>\>                         { nextstate = ASCII; BEGIN(IGNORELINE); }
<ASCII>\n				{ lineno++; }
<ASCII>[ \t\raeious]*			{ }
<ASCII>.				{ CODE_AND_RETURN((int) *yytext); }		

 /****************************************************************************

     HTML interface: Discard tags and whitespace

 *****************************************************************************/
<HTML>\>\>\>\>                          { nextstate = HTML; BEGIN(IGNORELINE); }
<HTML>\n				{ lineno++; }
<HTML><					{ htmlscope=1; BEGIN(HTMLTAG); }
<HTML>[ \t\r]* 				{ }
<HTML>.					{ CODE_AND_RETURN((int) *yytext); }
						
<HTMLTAG>>				{ if (--htmlscope == 0) { BEGIN(HTML); } }
<HTMLTAG><				{ htmlscope++; }
<HTMLTAG>\"([^"\\\n]|\\.)*\"		{ }
<HTMLTAG>.				{ }
<HTMLTAG>\n				{ lineno++; }

 /****************************************************************************

   The rules for C and C++. 

   Match on lexical tokens.  All variables are considered to be equal, and
   we ignore curly braces, parentheses, and comments.

 *****************************************************************************/

<C>\>\>\>\>                     { nextstate = C; BEGIN(IGNORELINE); }
<C>"{"				{ }
<C>"/*"				{ nextstate = C; if (config.match_comment) { BEGIN(C_MATCHCOMMENT); } else BEGIN(CCOMMENT); }
<C>"//" 			{ nextstate = C; if (config.match_comment) { BEGIN(MATCHLINE); } else BEGIN(IGNORELINE); }
<C>\n				{ lineno++; }

<C>"}"                          { }

 /* whitespace rules */
<C>[ \r\t]                        { }			

 /* C preprocessor directives */
<C>#.*\n 			{ lineno++; }

 /* string constants */
<C>\"([^"\\\n]|\\.)*\"		{ CODE_AND_RETURN(C_STRING);  }

 /* character constants */
<C>'([^\\']|\\.)'		{ CODE_AND_RETURN(C_CHARC); }

 /* integer constants; we don't currently do anything special for floating point */
<C>[0-9]+			{ CODE_AND_RETURN(C_NUM); }

 /* keywords */
<C>"if"             		{ CODE_AND_RETURN(C_IF); }
<C>"else"             		{ CODE_AND_RETURN(C_ELSE); }
<C>"for"             		{ CODE_AND_RETURN(C_FOR); }
<C>"while"             		{ CODE_AND_RETURN(C_WHILE); }
<C>"case"             		{ CODE_AND_RETURN(C_CASE); }
<C>"switch"            		{ CODE_AND_RETURN(C_SWITCH); }
<C>"break"            		{ CODE_AND_RETURN(C_BREAK); }
<C>"do"            		{ CODE_AND_RETURN(C_DO); }
<C>"continue"          		{ CODE_AND_RETURN(C_CONTINUE); }
<C>"int"             		{ CODE_AND_RETURN(C_INT); }
<C>"char"             		{ CODE_AND_RETURN(C_CHAR); }
<C>"double"             	{ CODE_AND_RETURN(C_DOUBLE); }
<C>"float"             		{ CODE_AND_RETURN(C_FLOAT); }
<C>"short"             		{ CODE_AND_RETURN(C_SHORT); }
<C>"long"             		{ CODE_AND_RETURN(C_LONG); }
<C>"void"             		{ CODE_AND_RETURN(C_VOID); }
<C>"volatile"          		{ CODE_AND_RETURN(C_VOLATILE); }
<C>"union"             		{ CODE_AND_RETURN(C_UNION); }
<C>"struct"            		{ CODE_AND_RETURN(C_STRUCT); }
<C>"typedef"           		{ CODE_AND_RETURN(C_TYPEDEF); }
<C>"enum"             		{ CODE_AND_RETURN(C_ENUM); }
<C>"static"            		{ CODE_AND_RETURN(C_STATIC); }
<C>"extern"            		{ CODE_AND_RETURN(C_EXTERN); }
<C>"sizeof"            		{ CODE_AND_RETURN(C_SIZEOF); }


 /* identifiers */
<C>[a-zA-Z_][a-zA-Z0-9_]*	{ CODE_AND_RETURN(C_VAR);  }


 /* common C, C++ operators */
<C>"."             		{ CODE_AND_RETURN(C_DOT); }
<C>","             		{ CODE_AND_RETURN(C_COMMA); }
<C>"["             		{ CODE_AND_RETURN(C_LBRACK); }
<C>"]"             		{ CODE_AND_RETURN(C_RBRACK); }
<C>"("             		{ /* CODE_AND_RETURN(C_LPAREN); */ }
<C>")"             		{ /* CODE_AND_RETURN(C_RPAREN); */ }
 /* identify all relative comparison operations together */
<C>"<"             		{ CODE_AND_RETURN(C_LESS); }
<C>"<="             		{ CODE_AND_RETURN(C_LESS); }
<C>">"             		{ CODE_AND_RETURN(C_LESS); }
<C>">="             		{ CODE_AND_RETURN(C_LESS); }
<C>"*"             		{ CODE_AND_RETURN(C_STAR); }
<C>";"             		{ /* CODE_AND_RETURN(C_SEMICOLON); */ }
<C>"="             		{ CODE_AND_RETURN(C_ASSIGN); }
<C>"->"             		{ CODE_AND_RETURN(C_ARROW); }
<C>"=="             		{ CODE_AND_RETURN(C_EQUAL); }
<C>"!="             		{ CODE_AND_RETURN(C_NEQ); }
<C>"!"             		{ CODE_AND_RETURN(C_BANG); }
<C>"?"             		{ CODE_AND_RETURN(C_QUEST); }
<C>"/"             		{ CODE_AND_RETURN(C_SLASH); }
<C>"++"             		{ CODE_AND_RETURN(C_PP); }
<C>"--"             		{ CODE_AND_RETURN(C_MM); }
<C>"+"             		{ CODE_AND_RETURN(C_PLUS); }
<C>"-"             		{ CODE_AND_RETURN(C_MINUS); }
<C>"&&"             		{ CODE_AND_RETURN(C_AA); }
<C>"&"             		{ CODE_AND_RETURN(C_AND); }
<C>"||"             		{ CODE_AND_RETURN(C_OO); }
<C>"::"             		{ CODE_AND_RETURN(C_COLONCOLON); }
<C>":"             		{ CODE_AND_RETURN(C_COLON); }
<C>"|"             		{ CODE_AND_RETURN(C_OR); }
<C>"~"             		{ CODE_AND_RETURN(C_TILDE); }
<C>"%"             		{ CODE_AND_RETURN(C_PERCENT); }

 /* any other character is considered a one-character operator */
<C>.				{ CODE_AND_RETURN(C_OTHER); }
                                          

<CCOMMENT>\n				{ lineno++;}
<CCOMMENT>.				;
<CCOMMENT>"*/"				BEGIN(nextstate);

<C_MATCHCOMMENT>\n			{ lineno++; }
<C_MATCHCOMMENT>[ \t\r]*		{ }
<C_MATCHCOMMENT>.			{ CODE_AND_RETURN((int) *yytext); } 
<C_MATCHCOMMENT>"*/"			{ BEGIN(nextstate); }


 /****************************************************************************

   The rules for Perl

 *****************************************************************************/

<PERL>\>\>\>\>                  { nextstate = PERL; BEGIN(IGNORELINE); }
<PERL>\n			{ lineno++; }

<PERL>"{"                       { }
<PERL>"}"                       { }
 /* whitespace rules */
<PERL>[ \r\t]			{ }			

 /* string constants */
<PERL>\"([^"\\\n]|\\.)*\"	{ CODE_AND_RETURN(C_STRING);  }

 /* character constants */
<PERL>'([^\\']|\\.)'		{ CODE_AND_RETURN(C_CHARC); }

 /* integer constants; we don't currently do anything special for floating point */
<PERL>[0-9]+			{ CODE_AND_RETURN(C_NUM); }

 /* keywords */
<PERL>"if"             		{ CODE_AND_RETURN(C_IF); }
<PERL>"else"             	{ CODE_AND_RETURN(C_ELSE); }
<PERL>"for"             	{ CODE_AND_RETURN(C_FOR); }
<PERL>"while"             	{ CODE_AND_RETURN(C_WHILE); }
<PERL>"case"             	{ CODE_AND_RETURN(C_CASE); }
<PERL>"switch"            	{ CODE_AND_RETURN(C_SWITCH); }
<PERL>"break"            	{ CODE_AND_RETURN(C_BREAK); }
<PERL>"do"            		{ CODE_AND_RETURN(C_DO); }
<PERL>"continue"          	{ CODE_AND_RETURN(C_CONTINUE); }
<PERL>"system"            	{ CODE_AND_RETURN(PERL_SYSTEM); }
<PERL>"chdir"            	{ CODE_AND_RETURN(PERL_BUILTIN); }
<PERL>"open"            	{ CODE_AND_RETURN(PERL_BUILTIN); }
<PERL>"close"            	{ CODE_AND_RETURN(PERL_BUILTIN); }
<PERL>"sub"            	        { CODE_AND_RETURN(C_FUNCTION); }


 /* identifiers */
<PERL>[a-zA-Z_][a-zA-Z0-9_]*	{ CODE_AND_RETURN(C_VAR);  }

 /* common PERL operators (in common with C, C++) */
<PERL>"."             		{ CODE_AND_RETURN(C_DOT); }
<PERL>","             		{ CODE_AND_RETURN(C_COMMA); }
<PERL>"["             		{ CODE_AND_RETURN(C_LBRACK); }
<PERL>"]"             		{ CODE_AND_RETURN(C_RBRACK); }
<PERL>"("             		{ /* CODE_AND_RETURN(C_LPAREN); */ }
<PERL>")"             		{ /* CODE_AND_RETURN(C_RPAREN); */ }
 /* identify all relative comparison operations together */
<PERL>"<"             		{ CODE_AND_RETURN(C_LESS); }
<PERL>"<="             		{ CODE_AND_RETURN(C_LESS); }
<PERL>">"             		{ CODE_AND_RETURN(C_LESS); }
<PERL>">="             		{ CODE_AND_RETURN(C_LESS); }
<PERL>"*"             		{ CODE_AND_RETURN(C_STAR); }
<PERL>";"             		{ CODE_AND_RETURN(C_SEMICOLON); }
<PERL>"="             		{ CODE_AND_RETURN(C_ASSIGN); }
<PERL>"->"             		{ CODE_AND_RETURN(C_ARROW); }
<PERL>"=="             		{ CODE_AND_RETURN(C_EQUAL); }
<PERL>"!="             		{ CODE_AND_RETURN(C_NEQ); }
<PERL>"!"             		{ CODE_AND_RETURN(C_BANG); }
<PERL>"?"             		{ CODE_AND_RETURN(C_QUEST); }
<PERL>"/"             		{ CODE_AND_RETURN(C_SLASH); }
<PERL>"++"             		{ CODE_AND_RETURN(C_PP); }
<PERL>"--"             		{ CODE_AND_RETURN(C_MM); }
<PERL>"+"             		{ CODE_AND_RETURN(C_PLUS); }
<PERL>"-"             		{ CODE_AND_RETURN(C_MINUS); }
<PERL>"&&"             		{ CODE_AND_RETURN(C_AA); }
<PERL>"&"             		{ CODE_AND_RETURN(C_AND); }
<PERL>"||"             		{ CODE_AND_RETURN(C_OO); }
<PERL>"::"             		{ CODE_AND_RETURN(C_COLONCOLON); }
<PERL>":"             		{ CODE_AND_RETURN(C_COLON); }
<PERL>"|"             		{ CODE_AND_RETURN(C_OR); }
<PERL>"~"             		{ CODE_AND_RETURN(C_TILDE); }
<PERL>"%"             		{ CODE_AND_RETURN(C_PERCENT); }
<PERL>"$"             		{ CODE_AND_RETURN(PERL_DOLLARSIGN); }
<PERL>"#"             		{ nextstate = PERL; BEGIN(IGNORELINE); }



 /* any other character is considered a one-character operator */
<PERL>.				{ CODE_AND_RETURN(C_OTHER); }

 /****************************************************************************

   The rules for PLSQL

 *****************************************************************************/


@include "include/PLSQL.match"

<PLSQL>"/*"                     {nextstate = PLSQL; BEGIN(CCOMMENT); }


 /****************************************************************************

   The rules for Python

 *****************************************************************************/


@include "include/PYTHON.match"


 /****************************************************************************

   The rules for MATLAB

 *****************************************************************************/
<MATLAB>\>\>\>\>                { nextstate = MATLAB; BEGIN(IGNORELINE); }
<MATLAB>\n			{ lineno++;}
<MATLAB>%			{ nextstate = MATLAB; BEGIN(IGNORELINE); }
<MATLAB>[ \t()]			{ }


 /* string constants */
<MATLAB>\'([^'\\\n]|\\.)*\'	{ CODE_AND_RETURN(C_STRING);  }

 /* integer constants; we don't currently do anything special for floating point */
<MATLAB>[0-9]+		        { CODE_AND_RETURN(C_NUM); }

<MATLAB>"="             	{ CODE_AND_RETURN(C_ASSIGN); }
<MATLAB>"=="           		{ CODE_AND_RETURN(C_EQUAL); }
<MATLAB>":"            		{ CODE_AND_RETURN(C_COLON); }
<MATLAB>"+"            		{ CODE_AND_RETURN(C_PLUS); }
<MATLAB>"-"            		{ CODE_AND_RETURN(C_MINUS); }
<MATLAB>"*"            		{ CODE_AND_RETURN(C_STAR); }
<MATLAB>"/"            		{ CODE_AND_RETURN(C_SLASH); }
<MATLAB>"^"            		{ CODE_AND_RETURN(C_COLONCOLON); } /* pick an unused C code for "^" */
<MATLAB>";"            		{ CODE_AND_RETURN(C_SEMICOLON); }
<MATLAB>","             	{ CODE_AND_RETURN(C_COMMA); }
<MATLAB>"if"           		{ CODE_AND_RETURN(C_IF); }
<MATLAB>"else"         		{ CODE_AND_RETURN(C_ELSE); }
<MATLAB>"for"          		{ CODE_AND_RETURN(C_FOR); }
<MATLAB>"while"        		{ CODE_AND_RETURN(C_WHILE); }
<MATLAB>"end"          		{ CODE_AND_RETURN(C_BREAK); }      /* just pick an unused C code for "end" */

 /* variable names */
<MATLAB>[a-zA-Z_][a-zA-Z0-9_]*	{ CODE_AND_RETURN(C_VAR);  }

 /* any other character is considered a one-character operator */
<MATLAB>.			{ CODE_AND_RETURN(C_OTHER); }


 /****************************************************************************

   The rules for FORTRAN

 *****************************************************************************/
<FORTRAN>\>\>\>\>               { nextstate = FORTRAN; BEGIN(IGNORELINE); }
<FORTRAN>\n			{ lineno++;}
<FORTRAN>!			{ nextstate = FORTRAN; BEGIN(IGNORELINE); }
 /* ignore whitespace, parens, and continuations (&) */
<FORTRAN>[ \r\t()&]		{ }

 /* string constants */
<FORTRAN>\"([^"\\\n]|\\.)*\"	{ CODE_AND_RETURN(F_STRING);  }
<FORTRAN>\'([^'\\\n]|\\.)*\'	{ CODE_AND_RETURN(F_STRING);  }

 /* integer constants; we don't currently do anything special for floating point */
<FORTRAN>[0-9]+		        { CODE_AND_RETURN(F_INT); }

<FORTRAN>"="             	{ CODE_AND_RETURN(F_EQ); }
<FORTRAN>"=="          		{ CODE_AND_RETURN(F_RELOP); }
<FORTRAN>"<"           		{ CODE_AND_RETURN(F_RELOP); }
<FORTRAN>">"           		{ CODE_AND_RETURN(F_RELOP); }
<FORTRAN>">="          		{ CODE_AND_RETURN(F_RELOP); }
<FORTRAN>"<="          		{ CODE_AND_RETURN(F_RELOP); }
<FORTRAN>"<>"          		{ CODE_AND_RETURN(F_RELOP); }
<FORTRAN>".EQ."        		{ CODE_AND_RETURN(F_RELOP); }
<FORTRAN>".GT."        		{ CODE_AND_RETURN(F_RELOP); }
<FORTRAN>".GE."        		{ CODE_AND_RETURN(F_RELOP); }
<FORTRAN>".LT."        		{ CODE_AND_RETURN(F_RELOP); }
<FORTRAN>".LE."        		{ CODE_AND_RETURN(F_RELOP); }
<FORTRAN>".NE."        		{ CODE_AND_RETURN(F_RELOP); }
<FORTRAN>"*"           		{ CODE_AND_RETURN(F_AST); }
<FORTRAN>"**"          		{ CODE_AND_RETURN(F_DOUBLEAST); }
<FORTRAN>"-"           		{ CODE_AND_RETURN(F_MINUS); }
<FORTRAN>"/"           		{ CODE_AND_RETURN(F_SLASH); }
<FORTRAN>"//"          		{ CODE_AND_RETURN(F_DOUBLESLASH); }
<FORTRAN>","           		{ CODE_AND_RETURN(F_COMMA); }
<FORTRAN>"."           		{ CODE_AND_RETURN(F_DOT); }
<FORTRAN>".TRUE."      		{ CODE_AND_RETURN(F_TRUE); }
<FORTRAN>".FALSE."     		{ CODE_AND_RETURN(F_FALSE); }
<FORTRAN>"IF"          		{ CODE_AND_RETURN(F_IF); }
<FORTRAN>"ELSE"        		{ CODE_AND_RETURN(F_ELSE); }
<FORTRAN>"DO"     		{ CODE_AND_RETURN(F_DO); }
<FORTRAN>"END"        		{ CODE_AND_RETURN(F_END); }     
<FORTRAN>"TYPE"   		{ CODE_AND_RETURN(F_TYPE); }
<FORTRAN>"PRIVATE"		{ CODE_AND_RETURN(F_PRIVATE); }
<FORTRAN>"SEQUENCE"     	{ CODE_AND_RETURN(F_SEQUENCE); }
<FORTRAN>"POINTER"      	{ CODE_AND_RETURN(F_POINTER); }
<FORTRAN>"ALLOCATABLE"  	{ CODE_AND_RETURN(F_ALLOCATABLE); }
<FORTRAN>"DIMENSION"    	{ CODE_AND_RETURN(F_DIMENSION); }
<FORTRAN>"TYPEALIAS"     	{ CODE_AND_RETURN(F_TYPEALIAS); }
<FORTRAN>"BIND"          	{ CODE_AND_RETURN(F_BIND); }
<FORTRAN>"NAME"                 { CODE_AND_RETURN(F_NAME); }
<FORTRAN>"ENUM"          	{ CODE_AND_RETURN(F_ENUM); }
<FORTRAN>"ENUMERATOR"    	{ CODE_AND_RETURN(F_ENUMERATOR); }
<FORTRAN>"INTEGER"       	{ CODE_AND_RETURN(F_INTEGER); }
<FORTRAN>"REAL"          	{ CODE_AND_RETURN(F_REAL); }
<FORTRAN>"DOUBLE"        	{ CODE_AND_RETURN(F_DOUBLE); }
<FORTRAN>"COMPLEX"       	{ CODE_AND_RETURN(F_COMPLEX); }
<FORTRAN>"PRECISION"     	{ CODE_AND_RETURN(F_PRECISION); }
<FORTRAN>"CHARACTER"     	{ CODE_AND_RETURN(F_CHARACTER); }
<FORTRAN>"LOGICAL"       	{ CODE_AND_RETURN(F_LOGICAL); }
<FORTRAN>"PARAMETER"     	{ CODE_AND_RETURN(F_PARAMETER); }
<FORTRAN>"AUTOMATIC"     	{ CODE_AND_RETURN(F_AUTOMATIC); }
<FORTRAN>"EXTERNAL"      	{ CODE_AND_RETURN(F_EXTERNAL); }
<FORTRAN>"INTENT"        	{ CODE_AND_RETURN(F_INTENT); }
<FORTRAN>"INTRINSIC"            { CODE_AND_RETURN(F_INTRINSIC); }
<FORTRAN>"SAVE"          	{ CODE_AND_RETURN(F_SAVE); }
<FORTRAN>"STATIC"        	{ CODE_AND_RETURN(F_STATIC); }
<FORTRAN>"TARGET"        	{ CODE_AND_RETURN(F_TARGET); }
<FORTRAN>"VALUE"         	{ CODE_AND_RETURN(F_VALUE); }
<FORTRAN>"VOLATILE"      	{ CODE_AND_RETURN(F_VOLATILE); }
<FORTRAN>"LEN"          	{ CODE_AND_RETURN(F_LEN); }
<FORTRAN>"KIND"          	{ CODE_AND_RETURN(F_KIND); }
<FORTRAN>"IN"          		{ CODE_AND_RETURN(F_IN); }
<FORTRAN>"OUT"          	{ CODE_AND_RETURN(F_OUT); }
<FORTRAN>"INOUT"        	{ CODE_AND_RETURN(F_INOUT); }
<FORTRAN>"EQUIVALENCE"  	{ CODE_AND_RETURN(F_EQUIVALENCE); }
<FORTRAN>"COMMON"       	{ CODE_AND_RETURN(F_COMMON); }
<FORTRAN>"WHERE"        	{ CODE_AND_RETURN(F_WHERE); }
<FORTRAN>"FORALL"       	{ CODE_AND_RETURN(F_FORALL); }
<FORTRAN>"THEN"         	{ CODE_AND_RETURN(F_THEN); }
<FORTRAN>"SELECT"       	{ CODE_AND_RETURN(F_SELECT); }
<FORTRAN>"CASE"         	{ CODE_AND_RETURN(F_CASE); }
<FORTRAN>"GO"          		{ CODE_AND_RETURN(F_GO); }
<FORTRAN>"TO"          		{ CODE_AND_RETURN(F_TO); }

 /* variable names */
<FORTRAN>[a-zA-Z_][a-zA-Z0-9_]*	{ CODE_AND_RETURN(F_VAR);  }

 /* any other character is considered a one-character operator */
<FORTRAN>.			{ CODE_AND_RETURN(F_OTHER); }


 /****************************************************************************

   The rules for VHDL

 *****************************************************************************/

@include "include/VHDL.match"


 /****************************************************************************

   The rules for SPICE

 *****************************************************************************/
<SPICE>\>\>\>\>                 { nextstate = SPICE; BEGIN(IGNORELINE); }
<SPICE>\n			{ lineno++;}
<SPICE>"^*"		 	{ nextstate = SPICE; BEGIN(IGNORELINE); }
<SPICE>;		 	{ nextstate = SPICE; BEGIN(IGNORELINE); }
<SPICE>\.COMMENT	 	{ nextstate = SPICE; BEGIN(IGNORELINE); }
<SPICE>[ \t()]			{ }


 /* string constants */
<SPICE>\"([^"\\\n]|\\.)*\"	{ CODE_AND_RETURN(SPICE_STRING);  }

 /* character constants */
<SPICE>'([^\\']|\\.)'		{ CODE_AND_RETURN(SPICE_CHARC); }

 /* integer constants; we don't currently do anything special for floating point */
<SPICE>[0-9]+		        { CODE_AND_RETURN(SPICE_NUM); }

<SPICE>"="	             	{ CODE_AND_RETURN(SPICE_EQ); }
<SPICE>">"	             	{ CODE_AND_RETURN(SPICE_GREAT); }
<SPICE>"<"	             	{ CODE_AND_RETURN(SPICE_LESS); }
<SPICE>":"            		{ CODE_AND_RETURN(SPICE_COLON); }
<SPICE>"+"            		{ CODE_AND_RETURN(SPICE_PLUS); }
<SPICE>"-"            		{ CODE_AND_RETURN(SPICE_MINUS); }
<SPICE>"*"            		{ CODE_AND_RETURN(SPICE_STAR); }
<SPICE>"/"            		{ CODE_AND_RETURN(SPICE_SLASH); }
<SPICE>","             		{ CODE_AND_RETURN(SPICE_COMMA); }
<SPICE>"~"             		{ CODE_AND_RETURN(SPICE_TILDE); }

 /* SPICE keywords */
<SPICE>".AC"		{CODE_AND_RETURN(SPICE_AC); }
<SPICE>".DC"		{CODE_AND_RETURN(SPICE_DC); }
<SPICE>".MODEL"		{CODE_AND_RETURN(SPICE_MODEL); }
<SPICE>".DISTO"		{CODE_AND_RETURN(SPICE_DISTO); }
<SPICE>".END"		{CODE_AND_RETURN(SPICE_END); }
<SPICE>".ENDS"		{CODE_AND_RETURN(SPICE_ENDS); }

 /* variable names */
<SPICE>[a-zA-Z_][a-zA-Z0-9_]*	{ CODE_AND_RETURN(SPICE_VAR);  }

 /* any other character is considered a one-character operator */
<SPICE>.			{ CODE_AND_RETURN(SPICE_OTHER); }


 /****************************************************************************

   The rules for 8086.

 *****************************************************************************/

@include "include/A8086.match"

 /****************************************************************************

   The rules for PASCAL. 

 *****************************************************************************/

 /* comments */
<PASCAL>"{"				{ nextstate = PASCAL; BEGIN(PCOMMENT1); }
<PASCAL>"(*"				{ nextstate = PASCAL; BEGIN(PCOMMENT2); }

<PCOMMENT1>\n				{ lineno++;}
<PCOMMENT1>"}"				BEGIN(nextstate);
<PCOMMENT1>.				;

<PCOMMENT2>\n				{ lineno++;}
<PCOMMENT2>.				;
<PCOMMENT2>"*)"				BEGIN(nextstate);

@include "include/PASCAL.match"

 /****************************************************************************

   The rules for MODULA 2

 *****************************************************************************/

 /* comments */
<MODULA2>"{"				{ nextstate = MODULA2; BEGIN(PCOMMENT1); }
<MODULA2>"(*"				{ nextstate = MODULA2; BEGIN(PCOMMENT2); }

@include "include/MODULA2.match"

 /****************************************************************************

   The rules for MODULA 2

 *****************************************************************************/



<JAVASCRIPT>"/*"			{ nextstate = JAVASCRIPT; BEGIN(CCOMMENT); }
@include "include/JAVASCRIPT.match"

 /****************************************************************************

   The rules for Ada. 

 *****************************************************************************/


<ADA>"--"                       { nextstate = ADA; BEGIN(IGNORELINE); }

@include "include/ADA.match"

 /****************************************************************************

  The rules for C#

 *****************************************************************************/

<CSHARP>"/*"			{ nextstate = CSHARP; BEGIN(CCOMMENT); }
@include "include/CSHARP.match"

 /****************************************************************************

  The rules for Prolog

 *****************************************************************************/

<PROLOG>"/*"			{ nextstate = PROLOG; BEGIN(CCOMMENT); }
@include "include/PROLOG.match"


 /****************************************************************************

  The rules for Visual Basic

 *****************************************************************************/

<VB>' 			{ nextstate = VB; BEGIN(IGNORELINE); }

 /****************************************************************************

  The rules for MIPS

 *****************************************************************************/

@include "include/MIPS.match"

 /****************************************************************************

  The rules for Java  

 *****************************************************************************/

<JAVA>\>\>\>\>                          { nextstate = JAVA; BEGIN(IGNORELINE); }
 /* rules for indentifying the beginning and end of function bodies */
<JAVA>"{"				{  }
<JAVA>"}"                               {  }
<JAVA>"/*"				{ nextstate = JAVA; BEGIN(CCOMMENT); }
<JAVA>"//" 				{ nextstate = JAVA; BEGIN(IGNORELINE); }
<JAVA>\n				{ lineno++; }

 /* whitespace rules */
<JAVA>[ \r\t]			;				

 /* string constants */
<JAVA>\"([^"\\\n]|\\.)*\"	{ CODE_AND_RETURN(J_STRING); }

 /* character constants */
<JAVA>'([^\\']|\\.)'		{ CODE_AND_RETURN(J_CHARC); }

 /* integer constants; we don't currently do anything special for floating point */
<JAVA>[0-9]+			{ CODE_AND_RETURN(J_NUM); }


 /* keywords */
<JAVA>"if"             		        { CODE_AND_RETURN(J_IF); }
<JAVA>"else"             		{ CODE_AND_RETURN(J_ELSE); }
<JAVA>"for"             		{ CODE_AND_RETURN(J_FOR); }
<JAVA>"while"             		{ CODE_AND_RETURN(J_WHILE); }
<JAVA>"case"             		{ CODE_AND_RETURN(J_CASE); }
<JAVA>"switch"            		{ CODE_AND_RETURN(J_SWITCH); }
<JAVA>"break"            		{ CODE_AND_RETURN(J_BREAK); }
<JAVA>"do"            		        { CODE_AND_RETURN(J_DO); }
<JAVA>"continue"          		{ CODE_AND_RETURN(J_CONTINUE); }
<JAVA>"int"             		{ CODE_AND_RETURN(J_INT); }
<JAVA>"char"             		{ CODE_AND_RETURN(J_CHAR); }
<JAVA>"double"             	        { CODE_AND_RETURN(J_DOUBLE); }
<JAVA>"float"             		{ CODE_AND_RETURN(J_FLOAT); }
<JAVA>"short"             		{ CODE_AND_RETURN(J_SHORT); }
<JAVA>"byte"             		{ CODE_AND_RETURN(J_BYTE); }
<JAVA>"long"             		{ CODE_AND_RETURN(J_LONG); }
<JAVA>"void"             		{ CODE_AND_RETURN(J_VOID); }
<JAVA>"static"            		{ CODE_AND_RETURN(J_STATIC); }
<JAVA>"."            		        { CODE_AND_RETURN(J_DOT); }
<JAVA>"throw"          		        { CODE_AND_RETURN(J_THROW); }
<JAVA>"super"          		        { CODE_AND_RETURN(J_SUPER); }
<JAVA>"return"          	        { CODE_AND_RETURN(J_RETURN); }
<JAVA>"native"          	        { CODE_AND_RETURN(J_NATIVE); }
<JAVA>"finally"          	        { CODE_AND_RETURN(J_FINALLY); }
<JAVA>"try"          	                { CODE_AND_RETURN(J_TRY); }
<JAVA>"this"          	                { CODE_AND_RETURN(J_THIS); }
<JAVA>"catch"          	                { CODE_AND_RETURN(J_CATCH); }
<JAVA>"true"          	                { CODE_AND_RETURN(J_TRUE); }
<JAVA>"false"          	                { CODE_AND_RETURN(J_FALSE); }
<JAVA>"boolean"        	                { CODE_AND_RETURN(J_BOOLEAN); }
<JAVA>";"        	                { CODE_AND_RETURN(J_SEMICOLON); }
<JAVA>"*"        	                { CODE_AND_RETURN(J_STAR); }
<JAVA>"++"        	                { CODE_AND_RETURN(J_PP); }
<JAVA>"--"        	                { CODE_AND_RETURN(J_MM); }
<JAVA>"&&"        	                { CODE_AND_RETURN(J_AA); }
<JAVA>"&"        	                { CODE_AND_RETURN(J_AND); }
<JAVA>"||"        	                { CODE_AND_RETURN(J_OO); }
<JAVA>"|"        	                { CODE_AND_RETURN(J_OR); }
<JAVA>"~"        	                { CODE_AND_RETURN(J_TILDE); }
<JAVA>"new"        	                { CODE_AND_RETURN(J_NEW); }
<JAVA>"<"        	                { CODE_AND_RETURN(J_LESS); }
<JAVA>">"        	                { CODE_AND_RETURN(J_GREATER); }
<JAVA>"="        	                { CODE_AND_RETURN(J_EQUAL); }
<JAVA>"!="        	                { CODE_AND_RETURN(J_NEQ); }
<JAVA>"["        	                { CODE_AND_RETURN(J_LBRACK); }
<JAVA>","        	                { CODE_AND_RETURN(J_COMMA); }
<JAVA>"]"        	                { CODE_AND_RETURN(J_RBRACK); }
<JAVA>"("        	                ;
<JAVA>")"        	                ;
<JAVA>"?"        	                { CODE_AND_RETURN(J_QUEST); }
<JAVA>"-"        	                { CODE_AND_RETURN(J_MINUS); }
<JAVA>"+"        	                { CODE_AND_RETURN(J_PLUS); }
<JAVA>"!"        	                { CODE_AND_RETURN(J_BANG); }
<JAVA>":"        	                { CODE_AND_RETURN(J_COLON); }
<JAVA>"%"        	                { CODE_AND_RETURN(J_PERCENT); }
<JAVA>"#"        	                { CODE_AND_RETURN(J_HASH); }
 /* identifiers */
<JAVA>[a-zA-Z_][a-zA-Z0-9_]*	{ CODE_AND_RETURN(J_VAR); }


 /* any other character is considered a one-character operator */
<JAVA>.				{ CODE_AND_RETURN(J_OTHER); }


 /**********************************************************************************

 The rules for ML 

 ***********************************************************************************/

<ML>\>\>\>\>                    { nextstate = ML; BEGIN(IGNORELINE); }
<ML>"(*"		        { nextstate = ML; BEGIN(MLCOMMENT); }

 /* whitespace */
<ML>[ \r\t]			;				
<ML>\n				{ lineno++; }

 /* string constants */
<ML>\"([^"\\\n]|\\.)*\"		{ CODE_AND_RETURN(ML_STRINGC); }

 /* character constants */
<ML>#\".*\"		        { CODE_AND_RETURN(ML_CHARC); }

 /* integers; nothing special for floating point */
<ML>[0-9]+			{ CODE_AND_RETURN(ML_NUM); }

 /* keywords and other special operators */
<ML>"fun"                         { CODE_AND_RETURN(ML_FUN); }
<ML>"bool"                        {CODE_AND_RETURN(ML_BOOL); }
<ML>"char"                        {CODE_AND_RETURN(ML_CHAR); }
<ML>"array"                       {CODE_AND_RETURN(ML_ARRAY); }
<ML>"int"                         {CODE_AND_RETURN(ML_INT); }
<ML>"real"                        {CODE_AND_RETURN(ML_REAL); }
<ML>"ref"                         {CODE_AND_RETURN(ML_REF); }
<ML>"prime"                       {CODE_AND_RETURN(ML_PRIME); }
<ML>"unit"                        {CODE_AND_RETURN(ML_UNIT); }
<ML>"vector"                      {CODE_AND_RETURN(ML_VECTOR); }
<ML>"word"                        {CODE_AND_RETURN(ML_WORD); }
<ML>"string"                      {CODE_AND_RETURN(ML_STRINGT); }
<ML>"exn"                         {CODE_AND_RETURN(ML_EXN); }
<ML>"!"                           {CODE_AND_RETURN(ML_BANG); }
<ML>"nil"                         {CODE_AND_RETURN(ML_NIL); }
<ML>"+"                           {CODE_AND_RETURN(ML_PLUS); }
<ML>"-"                           {CODE_AND_RETURN(ML_MINUS); }
<ML>"^"                           {CODE_AND_RETURN(ML_CARAT); }
<ML>"::"                          {CODE_AND_RETURN(ML_CONS); }
<ML>"@"                           {CODE_AND_RETURN(ML_CAT); }
<ML>"="                           {CODE_AND_RETURN(ML_EQ); }
<ML>">"                           {CODE_AND_RETURN(ML_GREATER); }
<ML>"<"                           {CODE_AND_RETURN(ML_LESS); }
<ML>":="                          {CODE_AND_RETURN(ML_ASSIGN); }
<ML>"abstraction"                 {CODE_AND_RETURN(ML_ABSTRACTION); }
<ML>"abstype"                     {CODE_AND_RETURN(ML_ABSTYPE); }
<ML>"and"                         {CODE_AND_RETURN(ML_AND); }
<ML>"andalso"                     {CODE_AND_RETURN(ML_ANDALSO); }
<ML>"as"                          {CODE_AND_RETURN(ML_AS); }
<ML>"case"                        {CODE_AND_RETURN(ML_CASE); }
<ML>"do"                          {CODE_AND_RETURN(ML_DO); }
<ML>"datatype"                    {CODE_AND_RETURN(ML_DATATYPE); }
<ML>"else"                        {CODE_AND_RETURN(ML_ELSE); }
<ML>"exception"                   {CODE_AND_RETURN(ML_EXCEPTION); }
<ML>"fn"                          {CODE_AND_RETURN(ML_FN); }
<ML>"handle"                      {CODE_AND_RETURN(ML_HANDLE); }
<ML>"if"                          {CODE_AND_RETURN(ML_IF); }
<ML>"infix"                       {CODE_AND_RETURN(ML_INFIX); }
<ML>"infixr"                      {CODE_AND_RETURN(ML_INFIXR); }
<ML>"let"                         {CODE_AND_RETURN(ML_LET); }
<ML>"local"                       {CODE_AND_RETURN(ML_LOCAL); }
<ML>"nonfix"                      {CODE_AND_RETURN(ML_NONFIX); }
<ML>"of"                          {CODE_AND_RETURN(ML_OF); }
<ML>"op"                          {CODE_AND_RETURN(ML_OP); }
<ML>"open"                        {CODE_AND_RETURN(ML_OPEN); }
<ML>"orelse"                      {CODE_AND_RETURN(ML_ORELSE); }
<ML>"raise"                       {CODE_AND_RETURN(ML_RAISE); }
<ML>"rec"                         {CODE_AND_RETURN(ML_REC); }
<ML>"sig"                         {CODE_AND_RETURN(ML_SIGNATURE); }
<ML>"struct"                      {CODE_AND_RETURN(ML_STRUCT); }
<ML>"structure"                   {CODE_AND_RETURN(ML_STRUCTURE); }
<ML>"then"                        {CODE_AND_RETURN(ML_THEN); }
<ML>"type"                        {CODE_AND_RETURN(ML_TYPE); }
<ML>"vals"                        {CODE_AND_RETURN(ML_VALS); }
<ML>"with"                        {CODE_AND_RETURN(ML_WITH); }
<ML>"withtype"                    {CODE_AND_RETURN(ML_WITHTYPE); }
<ML>"while"                       {CODE_AND_RETURN(ML_WHILE); }
<ML>"("                           ;
<ML>")"                           ;
<ML>"{"                           {CODE_AND_RETURN(ML_LCBRACK);}
<ML>"}"                           {CODE_AND_RETURN(ML_RCBRACK);}
<ML>"["                           {CODE_AND_RETURN(ML_LBRACK);}
<ML>"]"                           {CODE_AND_RETURN(ML_RBRACK);}
<ML>","                           {CODE_AND_RETURN(ML_COMMA);}
<ML>":"                           {CODE_AND_RETURN(ML_COLON);}
<ML>":>"                          {CODE_AND_RETURN(ML_COLONGREATER);}
<ML>";"                           {CODE_AND_RETURN(ML_SEMICOLON);}
<ML>"..."                         {CODE_AND_RETURN(ML_ELIPSES);}
<ML>"_"                           {CODE_AND_RETURN(ML_UNDERBAR);}
<ML>"|"                           {CODE_AND_RETURN(ML_VERTICALBAR);}
<ML>"=>"                          {CODE_AND_RETURN(ML_DOUBLEARROW);}
<ML>"->"                          {CODE_AND_RETURN(ML_SINGLEARROW);}
<ML>"#"                           {CODE_AND_RETURN(ML_HASH);}

 /* identifiers; the "fun" rule must be listed before this one. */
<ML>[a-zA-Z_][a-zA-Z0-9_]*	{ CODE_AND_RETURN(ML_VAR); }

 /* anything else is considered a one character operator */
<ML>.				{ CODE_AND_RETURN(ML_OTHER); }

<MLCOMMENT>\n				{ lineno++;}
<MLCOMMENT>.				;
<MLCOMMENT>"*)"				{ BEGIN(nextstate); }


 /**********************************************************************************

 The rules for Haskell

 ***********************************************************************************/

<HASKELL>\>\>\>\>                  { nextstate = HASKELL; BEGIN(IGNORELINE); }
<HASKELL>"{-"		           { BEGIN(HASKELLCOMMENT); }
<HASKELL>"--"		           { nextstate = HASKELL; BEGIN(IGNORELINE); }
<HASKELL>\n			   { lineno++; }

<HASKELLCOMMENT>"-}"		   { BEGIN(HASKELL); }
<HASKELLCOMMENT>\n		   { lineno++; }
<HASKELLCOMMENT>.		   { }

 /* whitespace */
<HASKELL>[ \r\t]		   { } 				
<HASKELL>\"([^"\\\n]|\\.)*\"	   { CODE_AND_RETURN(HASKELL_STRINGC); }

 /* character constants */
<HASKELL>'([^\\']|\\.)'		   { CODE_AND_RETURN(HASKELL_CHARC); }

 /* integers; nothing special for floating point */
<HASKELL>[0-9]+		           { CODE_AND_RETURN(HASKELL_NUM); }

 /* keywords and other special operators */
<HASKELL>".."			   { CODE_AND_RETURN(HASKELL_DOUBLEDOT); }
<HASKELL>"::"                      { CODE_AND_RETURN(HASKELL_DOUBLECOLON); } 
<HASKELL>":"                       { CODE_AND_RETURN(HASKELL_COLON); } 
<HASKELL>"="                       { CODE_AND_RETURN(HASKELL_EQ); }
<HASKELL>"\\"                       { CODE_AND_RETURN(HASKELL_BACKSLASH); } 
<HASKELL>"@"                       { CODE_AND_RETURN(HASKELL_AT); }
<HASKELL>"~"                       { CODE_AND_RETURN(HASKELL_TILDE); }
<HASKELL>"-"                       { CODE_AND_RETURN(HASKELL_MINUS); }
<HASKELL>"|"                       { CODE_AND_RETURN(HASKELL_VERTICALBAR);}
<HASKELL>"=>"                      { CODE_AND_RETURN(HASKELL_DOUBLEARROW);}
<HASKELL>"->"                      { CODE_AND_RETURN(HASKELL_RIGHTARROW);}
<HASKELL>"<-"                      { CODE_AND_RETURN(HASKELL_LEFTARROW);}
<HASKELL>"case"                    { CODE_AND_RETURN(HASKELL_CASE); }
<HASKELL>"class"                   { CODE_AND_RETURN(HASKELL_CLASS); }
<HASKELL>"data"                    { CODE_AND_RETURN(HASKELL_DATA); }
<HASKELL>"default"                 { CODE_AND_RETURN(HASKELL_DEFAULT); }
<HASKELL>"deriving"                { CODE_AND_RETURN(HASKELL_DERIVING); }
<HASKELL>"do"                      { CODE_AND_RETURN(HASKELL_DO); }
<HASKELL>"else"                    { CODE_AND_RETURN(HASKELL_ELSE); }
<HASKELL>"if"                      { CODE_AND_RETURN(HASKELL_IF); }
<HASKELL>"import"                  { CODE_AND_RETURN(HASKELL_IMPORT); }
<HASKELL>"in"                      { CODE_AND_RETURN(HASKELL_IN); }
<HASKELL>"infix"                   { CODE_AND_RETURN(HASKELL_INFIX); }
<HASKELL>"infixl"                  { CODE_AND_RETURN(HASKELL_INFIX); }
<HASKELL>"infixr"                  { CODE_AND_RETURN(HASKELL_INFIX); }
<HASKELL>"let"                     { CODE_AND_RETURN(HASKELL_LET); }
<HASKELL>"module"                  { CODE_AND_RETURN(HASKELL_MODULE); }
<HASKELL>"newtype"                 { CODE_AND_RETURN(HASKELL_NEWTYPE); }
<HASKELL>"of"                      { CODE_AND_RETURN(HASKELL_OF); }
<HASKELL>"then"                    { CODE_AND_RETURN(HASKELL_THEN); }
<HASKELL>"type"                    { CODE_AND_RETURN(HASKELL_TYPE); }
<HASKELL>"where"                   { CODE_AND_RETURN(HASKELL_WHERE); }
<HASKELL>"as"                      { CODE_AND_RETURN(HASKELL_AS); }
<HASKELL>"qualified"               { CODE_AND_RETURN(HASKELL_QUALIFIED); }
<HASKELL>"hiding"                  { CODE_AND_RETURN(HASKELL_HIDING); }
<HASKELL>"+"                       { CODE_AND_RETURN(HASKELL_PLUS); }
<HASKELL>"["                       { CODE_AND_RETURN(HASKELL_LBRACK);}
<HASKELL>"]"                       { CODE_AND_RETURN(HASKELL_RBRACK);}
<HASKELL>","                       { CODE_AND_RETURN(HASKELL_COMMA);}
<HASKELL>"..."                     { CODE_AND_RETURN(HASKELL_ELLIPSES);}
<HASKELL>"_"                       { CODE_AND_RETURN(HASKELL_UNDERBAR);}

 /* identifiers */
<HASKELL>[a-zA-Z_][a-zA-Z0-9_]*	   { CODE_AND_RETURN(HASKELL_VAR); }

 /* lump Haskell operators and all other single characters together */
<HASKELL>[!#$%&*+./<=>?@\\\^|-~\.]+   { CODE_AND_RETURN(HASKELL_OTHER); }
<HASKELL>.			   { CODE_AND_RETURN(HASKELL_OTHER); }


 /***************************************************************************************

 The rules for Lisp and Scheme.

 These depend on a dynamically generated language of strings to match on, which is ugly.
 This should be cleaned up some day.

 ****************************************************************************************/

<LISP>\>\>\>\>                          { nextstate = LISP; BEGIN(IGNORELINE); }
 /* rules dealing with identifying the beginning and end of function bodies */
<LISP>"("				{ BEGIN(LISPFUNCTION); 
					  last = lineno;
                                  	  lispscope=1; 
                                          startfun(); 
                                        }
<LISP>";"			        { nextstate = LISP; BEGIN(IGNORELINE); }
<LISP>.				        ;
<LISP>\n			        { lineno++; }

<LISPFUNCTION>"("                       { lispscope++; }
<LISPFUNCTION>")"                       { if (--lispscope == 0) {
					    BEGIN(LISP); 
                                          }
                                        }
 /* whitespace rules */
<LISPFUNCTION>[ \r\t]			;			
<LISPFUNCTION>\n			{ lineno++; }
<LISPFUNCTION>";"		        { nextstate = LISPFUNCTION; BEGIN(IGNORELINE); }
 /* string constants */
<LISPFUNCTION>\"			{ nextstate=LISP; BEGIN(LISPSTRING); }
<LISPSTRING>\n				{ lineno++; }
<LISPSTRING>[^"]			{ }
<LISPSTRING>\"				{ BEGIN(LISP); CODE_DYNAMIC_AND_RETURN("A string"); }


 /* integer constants; we don't currently do anything special for floating point */
<LISPFUNCTION>[0-9]+			{ CODE_DYNAMIC_AND_RETURN("A num"); }

 /* identifiers (really, lisp symbols) */
<LISPFUNCTION>"car"			{ CODE_AND_RETURN(LISP_CAR); }
<LISPFUNCTION>"first"			{ CODE_AND_RETURN(LISP_CAR); }
<LISPFUNCTION>"cdr"			{ CODE_AND_RETURN(LISP_CDR); }
<LISPFUNCTION>"cadr"			{ CODE_AND_RETURN(LISP_CADR); }
<LISPFUNCTION>"second"			{ CODE_AND_RETURN(LISP_CADR); }
<LISPFUNCTION>"cddr"			{ CODE_AND_RETURN(LISP_CDDR); }

<LISPFUNCTION>[a-zA-Z_\*][a-zA-Z0-9_\?\-\*]* { CODE_DYNAMIC_AND_RETURN(yytext); }

 /* any other character is considered a one-character operator */
<LISPFUNCTION>.				{ CODE_DYNAMIC_AND_RETURN(yytext); }	


 /* catch all EOF rule */
<<EOF>>                                 {  BEGIN(INITIAL); return(0);  }
%%


/************************************************************************************

The Matching Engine.

*************************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>   /* for atoi */
#include <string.h>

#define TOKEN_WINDOW_SIZE_MAX  500           
#define MAXINT  0xFFFFFFFF
#define STREQ(a, b) (strcmp(a, b)==0)
#define MAX(a,b) (a > b ? a : b)
#define MIN(a,b) (a < b ? a : b)


/************************************************************************************

FILES

Metadata about files is stored in a global array.

*************************************************************************************/

typedef struct fileinfo {
  char *localname;
  char *remotename;
  int size;                  /* size is mesaured in the number of tokens lexed */
  int lines;   
  int setid;
  Lang language;
} fileinfo;

#define MAXFILES 75000
int arrsize = MAXFILES;
fileinfo files[MAXFILES];
int filesindex = 0;
int filesbase = 0;


/************************************************************************************

  THE TOKEN_SEQUENCE ARRAY

  The token_sequence array is used to hold a sliding window of tokens from the file currently
  being tokenized.  The array is a circular buffer.

*************************************************************************************/

typedef struct position {
  unsigned int val;
  int tokennum;
  int lineno;
} position;

static int token_index;                                   /* index of next token in token_sequence */
static position token_sequence[TOKEN_WINDOW_SIZE_MAX];    /* array of tokens */

/************************************************************************************

MODULO COUNTERS

  These macros implement an abstract type of modulo counters.  If you want to count
modulo a constant, use only these operations.

*************************************************************************************/

#define MODULO_PP(val,modulus)  ((val + 1) % modulus)
#define MODULO_MM(val,modulus) (((val + modulus) - 1) % modulus)
#define MODULO_SUBTRACT(val,num,modulus) (((val + modulus) - (num)) % modulus) 



/************************************************************************************

   LEGACY CODE FOR LISP/SCHEME

   The following group of functions remains here from an old version of Moss because
   it still appears to be the best way to handle Lisp and Scheme.  Instead of 
   doing string matching over a fixed alphabet, for Lisp and Scheme the alphabet is
   generated dynamically from the unique tokens seen during lexing.  This is implemented
   by hashing tokens and assigning each new token a unique integer.

*************************************************************************************/
#define HASHMAX 293        /* This should be set to a prime. */
int nid = LISP_FIRSTCODE;

typedef struct ident {
    struct ident *next;
    char *name;
    int idno;
} ident;
static ident *bucket[HASHMAX];

void clearhash()
{
    int key;
	
    deleteregion(config.mem_tokenization);
    config.mem_tokenization = newregion();		
    for (key=0; key<HASHMAX; key++)
       bucket[key] = NULL;
}

/* Hash function taken from histok. */
int hash(char *str)
{
    int k;

    for (k=0; *str; str++) {
	k = (k>>1) | (k<<7&0x80);
	k += *str;
    }
    return k%HASHMAX;
}

void startfun()
{

 /* For lisp and scheme we use a hash table to give unique id's
    to identifiers, since there are so few keywords. */
 if ((config.language == LangScheme) || (config.language == LangLisp))
   {
     clearhash();
     nid = LISP_FIRSTCODE;
   }

 if (DEBUG) printf("\nLine number: %d\n",lineno);
}

/*
   Add a token to the token_sequence, recording it's position in the token stream
   and line number as well.
*/


/************************************************************************************

   CODE

   The "code" functions are the interface between the lexer and the matching algorithm.
   The lexer calls one of these functions with each token recognized.  The token
   is either translated into a fixed integer by the lexer (what this integer is depends
   on the language being analyzed) for any language except Lisp/Scheme and this integer
   is passed to code(), or the token itself is passed to code_dynamic for Lisp/Scheme
   and the integer is computed either by looking it up in a hash table of existing
   (token,int) pairs, or by assigning a fresh integer to the token.

*************************************************************************************/

void code(int key)
{
  token_sequence[token_index].val = key;
  token_sequence[token_index].tokennum = token_count;
  token_sequence[token_index].lineno = lineno;
  token_index = MODULO_PP(token_index, config.token_window_size);
  token_count++;
  if (DEBUG) fprintf(stderr,"coded: %d next token_index: %d\n",key,token_index);
}


void code_dynamic(char *str)
{
  int key;
  ident *p;
  
  key = hash(str);
  for (p=bucket[key]; p && !STREQ(str, p->name); p=p->next);
  if (!p) {
	p = (ident *) ralloc(config.mem_tokenization, sizeof(ident));
	p->next = bucket[key];
	bucket[key] = p;
	p->name = (char *) ralloc(config.mem_tokenization, strlen(str)+1);
	strcpy(p->name, str);
	p->idno = nid++;
  }
  code(p->idno);
}


Lang string2lang(char *s) {
  Lang language = LangNone;
  if (STREQ(s,"c"))
    language = LangC;
  if (STREQ(s,"cc"))
    language = LangCC;
  if (STREQ(s,"java"))
    language = LangJava;
  if (STREQ(s,"ml"))
    language = LangML;
  if (STREQ(s,"lisp"))
    language = LangLisp;
  if (STREQ(s,"scheme"))
    language = LangScheme;
  if (STREQ(s, "ascii")) 
    language = LangASCII;
  if (STREQ(s, "test"))
    language = LangTest;
  if (STREQ(s, "perl"))
    language = LangPerl;
  if (STREQ(s, "haskell"))
    language = LangHaskell;
  if (STREQ(s, "matlab"))
    language = LangMatlab;
  if (STREQ(s, "html"))
    language = LangHTML;
  if (STREQ(s,"fortran"))
    language = LangFortran; 
  if (STREQ(s,"spice"))
    language = LangSpice; 
@include "include/lang.set";	
  return language;
}

/************************************************************************************

   TOKENIZATION: STARTING IT, STOPPING IT, DOING IT

   Initialize the program state for lexing.
   All that is needed is to reset the counter and index into the token_sequence arrray.

*************************************************************************************/

#define WINNOWING_WINDOW_SIZE_MAX 100
#define MAX_MATCHES 10           /* maximum number of matches per passage */
#define GAP 10                   /* # of tokens gap allowed between merged passages */

#define DEFAULT_THRESHOLD 30
int threshold = DEFAULT_THRESHOLD;              /* minimum match size */

#define DEFAULT_WINNOWING_WINDOW_SIZE 5
#define DEFAULT_TOKEN_WINDOW_SIZE 25


/************************************************************************************

  PASSAGES

  A "passage" identifies a file and a range of tokens in that file.  Associated
line numbers are also kept and eventually used for reporting the position of matches
to the user.  

  Passages are ordered lexicographically, first by fileid and then by position
in the file.  It's also useful to know the size of a passage, and whether
one passage contains another.

*************************************************************************************/
typedef struct passage {
  unsigned long long tile;
  int fileid;
  int first_token;
  int last_token;
  int first_line;
  int last_line;
} passage;

/*
This cannot be called after any function that sets p->fileid to an invalid
index for the files array (i.e., dont_match_common_tiles).

This order guarantees:
  - identical tiles are all contiguous
  - for a given tile, all copies from the same file are contiguous
  - if the tile is in a base file, then a copy from set 0 is listed first
  
*/
int passage_leq(const void *vp1, const void *vp2) {
    passage *p1 = (passage *) vp1;
    passage *p2 = (passage *) vp2;
    if (p1->tile < p2->tile)
        return -1;
    if (p1->tile > p2->tile)
	return 1;
    if (files[p1->fileid].setid > files[p2->fileid].setid)
	return 1;
    if (files[p1->fileid].setid < files[p2->fileid].setid)
	return -1;
    if (p1->fileid < p2->fileid)
        return -1;
    if (p1->fileid > p2->fileid)
        return 1;
    return p1->first_token - p2->first_token;
}

#define PASSAGE_ADJACENT(p1,p2) ((p1.fileid == p2.fileid) && (p1.last_token <= p2.last_token) && (p1.first_token <= p2.first_token) && ((p2.first_token - p1.last_token) <= config.gap))


#define PASSAGE_OVERLAP(p1,p2) ((p1.fileid == p2.fileid) && (p1.first_token < p2.last_token) && (p2.first_token < p1.last_token))

#define PASSAGE_SUBSET(p1,p2) ((p1.fileid == p2.fileid) && (p1.first_token >= p2.first_token) && (p1.last_token <= p2.last_token))

#define PASSAGE_SIZE(p) (p.last_token - p.first_token)

void write_string_to_file(FILE *f, char *s) {
  int bytes = strlen(s);
  fwrite((void *) &bytes, sizeof(int),1,f);
  fwrite((void *) s,bytes,1,f);
}

void write_int_to_file(FILE *f,int i) {
  fwrite((void *) &i, sizeof(int),1,f);
}

void write_Lang_to_file(FILE *f, Lang l) {
  fwrite((void *) &l, sizeof(Lang),1,f);
}



int write_database(char *name, fileinfo *files, int fsize, passage *passages, int psize) {
  FILE *f = fopen(name,"w");
  int i;

  if (f == NULL) {
    fprintf(stdout,"Could not open database file %s for writing.\n",name);
    return(-1);
  }

  write_int_to_file(f,fsize);
  for(i = 0; i < fsize; i++) {
    write_string_to_file(f,files->localname);
    write_string_to_file(f,files->remotename);
    write_int_to_file(f,files->size);
    write_int_to_file(f,files->lines);
    write_int_to_file(f,files->setid);
    write_Lang_to_file(f,files->language);
    files++;
  }

  write_int_to_file(f, psize);
  fwrite((void *) passages, sizeof(passage), psize, f);
  fprintf(stdout,"Wrote %d files (%d bytes) and %d passages (%d bytes) to %s\n",fsize,fsize * sizeof(fileinfo), psize, psize*sizeof(passage),name);
  return(0);

}


char *read_string_from_file(FILE *f) {
  int bytes;
  char *s;
  fread((void *) &bytes, sizeof(int),1,f);
  s = (char *) ralloc(config.mem_strings, bytes+1);
  fread((void *) s,bytes,1,f);
  *(s+bytes) = '\0';
  return s;
}

int read_int_from_file(FILE *f) {
  int i;
  fread((void *) &i, sizeof(int),1,f);
  return i;
}

Lang read_Lang_from_file(FILE *f) {
  Lang l;
  fread((void *) &l, sizeof(Lang),1,f);
  return l;
}

int read_database(char *name, fileinfo *files, int *fstart, int fmax, passage *passages, int *pstart, int pmax) {
  FILE *f = fopen(name,"r");
  int i,numfiles, numpassages;

  if (f == NULL) {
    fprintf(stdout,"Could not open database file %s for reading.\n",name);
    return(-1);
  }

  numfiles = read_int_from_file(f);
  if (*fstart + numfiles > fmax - 1) {
    fprintf(stdout,"Maximum number of files %d would be exceeded by loading database %s.\n",fmax,name);
    return(-1);
  }

  for(i = 0; i < numfiles; i++) {
    files->localname = read_string_from_file(f);
    files->remotename = read_string_from_file(f);
    files->size = read_int_from_file(f);
    files->lines = read_int_from_file(f);
    files->setid = read_int_from_file(f);
    files->language = read_int_from_file(f);
    files++;
  }

  numpassages = read_int_from_file(f);
  if (*pstart + numpassages > pmax - 1) {
    fprintf(stdout,"Maximum number of passages %d would be exceeded by loading database %s.\n",fmax,name);
    return(-1);
  }

  fread((void *) passages, sizeof(passage), numpassages, f);
  for(i =0; i < numpassages; i++) { 
    (passages + i)->fileid += *fstart;
  }

  *fstart += numfiles;
  *pstart += numpassages; 
		
  if (DEBUG) fprintf(stderr,"Read %d files and %d passages from %s\n",numfiles,numpassages,name);
  return(0);
}
	

/************************************************************************************

   MATCHES

   Matches are stored as lists of pairs of matching passages of a program.
   Two structs, match_info (the matches) and resultinfo (the list) hold this
   information.

   MATCH_INFO

   match_info records all relevant information about a match between
   two passages.  The first fields are the information about "this" program,
   the last fields are information about the "other" program.  Note
   match_info is really a list of such information.


   RESULTINFO

   resultinfo holds a list of passages that match the current program in program 
   "programid" in field  "matches".  The "next" field holds lists of matches
   from other programs.

*************************************************************************************/

typedef struct match_info {
  passage this;
  passage other;
  struct match_info *next;
} match_info;

match_info *new_match_info(passage *this, passage *other, match_info *next) 
{
  match_info *f = (match_info *) ralloc(config.mem_matches, sizeof(match_info));
  f->this = *this;
  f->other = *other;
  f->next = next;
  return f;
}

#define INFO_SIZE(i) (MAX(PASSAGE_SIZE(i->this),PASSAGE_SIZE(i->other)))

typedef struct resultinfo {
  int fileid;
  struct match_info *matches;
  struct resultinfo *next;
} resultinfo;


resultinfo *new_resultinfo(int fileid, match_info *matches, resultinfo *next) {
  resultinfo *r = (resultinfo *) ralloc(config.mem_matches, sizeof(resultinfo));
  r->fileid = fileid;	
  r->matches = matches;
  r->next = next;
  return r;
}

/* similar, but sort a match by position of the first passage */
int match_info_leq(const void *mi1, const void *mi2) {
    match_info **m1 = (match_info **) mi1;
    match_info **m2 = (match_info **) mi2;
    passage *p1 = &((*m1)->this);
    passage *p2 = &((*m2)->this);
    if (p1->fileid < p2->fileid)
        return -1;
    if (p1->fileid > p2->fileid)
        return 1;
    if (p1->first_token > p2->first_token)
	return 1;
    if (p1->first_token < p2->first_token)
	return -1;
    if (p1->tile < p2->tile)
        return -1;
    if (p1->tile > p2->tile)
	return 1;
    return 0;	
}

  

/*******************************************************************************************

   DEBUGGING

   A few routines for printing out data structures.

********************************************************************************************/


void dump_passage(char *s, passage *p)
{
  fprintf(stderr,"%s\tfile: %s\ttokens: %5d-%5d\tlines: %5d-%5d\ttile: %llx\tset:%d\t file id:%d \n", 
	  s, files[p->fileid].remotename, p->first_token, p->last_token, p->first_line, p->last_line, p->tile, files[p->fileid].setid,  p->fileid);
}

void dump_2passage(char *s, passage *p1, passage *p2)
{
  fprintf(stderr,"%s\n", s);
  fprintf(stderr,"\tfile: %s\ttokens: %d-%d\tlines: %d-%d\n", 
	  files[p1->fileid].remotename, p1->first_token, p1->last_token, p1->first_line, p1->last_line);
  fprintf(stderr,"\tfile: %s\ttokens: %d-%d\tlines: %d-%d\n", 
	  files[p2->fileid].remotename, p2->first_token, p2->last_token, p2->first_line, p2->last_line);
}


void dump_4passage(char *s, passage p1, passage p2, passage p3, passage p4)
{
  fprintf(stderr,"%s\n", s);
  fprintf(stderr,"\tfile: %s\ttokens: %d-%d\tlines: %d-%d\n", 
	  files[p1.fileid].remotename, p1.first_token, p1.last_token, p1.first_line, p1.last_line);
  fprintf(stderr,"\tfile: %s\ttokens: %d-%d\tlines: %d-%d\n", 
	  files[p2.fileid].remotename, p2.first_token, p2.last_token, p2.first_line, p2.last_line);
  fprintf(stderr,"\tfile: %s\ttokens: %d-%d\tlines: %d-%d\n", 
	  files[p3.fileid].remotename, p3.first_token, p3.last_token, p3.first_line, p3.last_line);
  fprintf(stderr,"\tfile: %s\ttokens: %d-%d\tlines: %d-%d\n", 
	  files[p4.fileid].remotename, p4.first_token, p4.last_token, p4.first_line, p4.last_line);
}

void dump_match_list(match_info *f)
{
  for(; f != NULL; f = f->next)
    {
      dump_2passage("list elt: ", &(f->this), &(f->other));
      fprintf(stderr,"\n");
    }
}


void dump_circular_buffer(position v[], int n, int num, int size)
{
  int i, start;
  start = MODULO_SUBTRACT(n, num, size);
  fprintf(stderr,"[ ");	
  for(i = 0; i < num; i++)
    fprintf(stderr,"%d ",v[(start+i)%size].val);
  fprintf(stderr,"]\n");
}




/***********************************************************************************

  STRING MATCHING

  Routines associated with computing the hash of a substring.

  Karp-Rabin string matching is based on the following idea.
  Assume we want to find all the substrings matching a given pattern string p
  in a string s.  If B is the cardinality of the alphabet, then every string
  of length |p| can be regarded as a number base B^|p|.  Obviously two substrings
  are the same if they represent the same number.

  We can make this idea more realistic by hashing the numbers.  Let {p} stand
  for string p represented as a number.  Then a heuristic for determining if two substrings 
  p and p' are equal is to check {p} mod k = {p'} mod k.  If they are not equal,
  the strings are certainly not equal.  If they are equal, we still need to compare
  p and p'.  If matches are rare then this comparison will be of negligible cost.

  We now need to compute {s'} mod k for every length |p| substring s' of s.
  Consider a substring "c_1 c_2 ... c_p+1" and let

      x = c_1 ... c_p
      y = c_2 ... c_p+1

  Now we can observer that

     {y} = ({x} - (c_1 * B^p)) * B + c_p+1 

  The idea is that we can compute {y} by discarding the leftmost digit of {x},
  shifting left by the base B, and adding in the rightmost digit of y.  To compute
  the modulus:
  
     {y} mod k = (({x} mod k) - (c_1 * (B^p mod k)) mod k) * B + c_p+1 

  assuming B << k.  This now gives us an efficient procedure.  For the
  purposes of this program the values are:

  B = 269        (a small prime)
  k = 2^64       (i.e., the natural modulus of long long unsigned ints)
  p = max_match  (a user chosen parameter)
  
************************************************************************************/

#define B 269

unsigned long long M;

/* M = B^substringlen */
void base_calc(int substringlen)
{
  int i;

  M = 1;
  for (i = 0; i < substringlen; i++)
    M = M * B;
  if (DEBUG) fprintf(stderr,"B = %u, M = %llu\n", B, M);
}

/*  Compute the hash of the first tile of size array_size. The numbers in the
circular buffer are treated as a base B number, with the most recently entered
index as the lowest order digits. */

unsigned long long first_hash(position vec[], int position, int tile_size, int array_size)
{
  unsigned long long hash = 0;
  int i, start;
  start = MODULO_SUBTRACT(position, tile_size, array_size);
  if (DEBUG) fprintf(stderr,"position %d array_size %d \n", position, array_size);
  assert(0 <= position && position <= array_size); 	
  for(i = 0; i < tile_size; i++)
    {
      hash = (hash  + (unsigned int) vec[start].val) * (unsigned int) B;
      start = MODULO_PP(start, array_size);
    }
  return hash;
}

/* compute the next hash incrementally from the previous one

unsigned long long next_hash(position vec[], int position, int array_size, int string_size, unsigned long long old_hash)
{
	if (DEBUG) 
	  fprintf(stderr,"position: %d\nold_hash: %llx\ndropping %u at position %d\nadding %u at position %d\n",position,old_hash,vec[MODULO_SUBTRACT(position,string_size+1,array_size)].val, MODULO_SUBTRACT(position, string_size+1, array_size), vec[MODULO_MM(position, array_size)].val, MODULO_MM(position, array_size));
	   return (old_hash - (((unsigned int) vec[MODULO_SUBTRACT(position,string_size+1,array_size)].val)*M) + (unsigned int) vec[MODULO_MM(position, array_size)].val) * B;
}
 */

#define NEXT_HASH(vec,position,array_size,string_size,old_hash)      	((old_hash - (((unsigned int) vec[MODULO_SUBTRACT(position,string_size+1,array_size)].val)*M) + (unsigned int) vec[MODULO_MM(position, array_size)].val) * B)



/*******************************************************************************************

   INPUT

   Opening files and driving the lexical analyzer.

*******************************************************************************************/

FILE *open_file(char *name) {

  fin = fopen(name, "r");
  if (fin == NULL) 
    {
      fprintf(config.errfile,"Could not open file \"%s\"\n",name);
      return NULL;
    }
  filename = name; /* record filename in global variable for use in flex diagnostics */
  return fin;
}

/*
   read_token is the interface to the lexical analyzer.  yylex()
reads one token into the global token array.

    Returns 1 on end of file and 0 otherwise.
*/
int read_token(FILE *fin, int fileid) 
{
  if (yylex() != 0)
    {
      files[fileid].size++;
      return 0;
    }
  else
    {
      files[fileid].lines += lineno;
      lineno=0;	
      return 1;
    }
}

void initialize_token_sequence(FILE *fin, int fileid) 
{
  int result,i;	

  for(i = 0; i < config.token_window_size; i++)
    token_sequence[i].val = 0;

  token_index = 0;

  for(i = 0; i < config.tile_size; i++) {
   result = read_token(fin,fileid);	

   if (DEBUG) {
     fprintf(stderr,"processing startup tokens with result %d: ",result);
     dump_circular_buffer(token_sequence, token_index, config.tile_size, config.token_window_size);
   }
  }
}

/*******************************************************************************************

   PASS 1

   The main algorithm makes two passes.  The first pass processes
   each file and produces lists of matching passages.  The second pass reorganizes
   the matches into a report.  Two passes are needed to properly exclude
   matches that appear in many different files; the idea is that such matches
   are very likely code that is legitimately common to many programs.  Until
   all the programs are processed it cannot be known how many programs have
   a particular passage.

   Pass1 builds the global hash table of passages.  Passages that match base
   files are excluded as are passages that appear in more than a given number
   of other programs.

   Pass1 computes the hash of every substring of length max_match in a function body.
   However, to save space only a few of these hashes are stored in the database.
   Assuming no matches, about every Kth substring hash is saved; this guarantees that
   if another program has a match of length (max_match + K) that it will be detected,
   but it will only consume 1/Kth the storage.

*******************************************************************************************/

#define PASSAGE_ARRAY_SIZE (2 << config.passage_power)
#define PASSAGE_INDEX_ARRAY_SIZE (PASSAGE_ARRAY_SIZE >> 3)
#define PASSAGE_INDEX_HASH(h) ((int) (h >> (64 - (config.passage_power - 3))))
 
passage *passages, *ps = NULL;

int *passage_index = NULL;
int pindex = 0;

int first_tile(int i) {
    return (i == 0) || (passages[i-1].tile != passages[i].tile);
}

int first_tile_in_file(int i) {
    return (i == 0) || (passages[i-1].tile != passages[i].tile) || (passages[i-1].fileid != passages[i].fileid);
}

void init_passage_array() {
    passages = (passage *) ralloc(config.mem_database, sizeof(passage) * PASSAGE_ARRAY_SIZE);
    if (passages == NULL) {
	fprintf(stdout,"Cannot allocate passage array.\n");
	exit(1);
    }
    ps = passages;
    pindex = 0;
    if (DEBUG) fprintf(stderr,"passage array initialized\n");
}
	
void store_passage(passage *p) {
    if (DEBUG) dump_passage("storing passage ",p);	
    *ps++ = *p;
    if (++pindex >= PASSAGE_ARRAY_SIZE) {
	fprintf(stdout,"Passage array too small\n");
	exit(1);
    }
}

void dump_passage_array() {
    int i = 0;
    for(; i < pindex; i++) 	
	dump_passage("",&passages[i]);
}
 	

void dump_fileinfo(fileinfo *fi) {
  fprintf(stderr,"local: %s\tremote: %s\tsize: %d\tlines: %d\tset: %d\tlanguage: %d\n",
	  fi->localname, fi->remotename, fi->size, fi->lines, fi->setid, (int) fi->language);
}

void dump_file_array() {
  int i = 0;
  for(; i < filesindex; i++)
    dump_fileinfo(&files[i]);
}

void dump_database_info() {
  dump_file_array();
  dump_passage_array();
}

void sort_passage_array() {
	if (DEBUG) { fprintf(stderr,"unsorted passage array\n"); dump_passage_array(); }
	qsort((void *) passages, pindex, sizeof(passage), passage_leq);
	if (DEBUG) { fprintf(stderr,"\nsorted passage array\n"); dump_passage_array(); }
}


void  build_passage_array_index() {
    int i = 0, j = 0;
    passage_index = (int *) ralloc(config.mem_index, sizeof(int) * PASSAGE_INDEX_ARRAY_SIZE);
    for (; i < PASSAGE_INDEX_ARRAY_SIZE; i++) {
      for(; (PASSAGE_INDEX_HASH(passages[j].tile) < i) && (j < pindex); j++);
      passage_index[i] = j;
    }
}		

/*

The following routines help to navigate through multiple copies of one tile.
There are routines for counting the number of copies of a tile in one file, which 
of several duplicates (ordered by file and then position in a file) a tile is,
finding the first tile in the next, and so on.

This code assumes the array of passages has been sorted lexicographically by
tile, fileid, and position.

*/

/* returns the total number of copies of a tile in the database */
int num_copies_of_tile(int i) {
    int count = 0; 
    unsigned long long tile = passages[i].tile;
    assert(first_tile(i));	
    for(; (i < pindex) && (passages[i].tile == tile); i++)
	count++;
    return count;	
}

int num_copies_of_tile_in_file(int i) {
    int count = 0; 
    unsigned long long tile = passages[i].tile;
    int fileid = passages[i].fileid;
    assert(first_tile_in_file(i));
    for(; (i < pindex) && (passages[i].tile == tile) && (passages[i].fileid == fileid); i++)
	count++;
    return count;	
}

/* find the first occurrence of the tile in the next file in the database */
int next_file_for_tile(int i) {
    int count = num_copies_of_tile_in_file(i);
    assert(first_tile_in_file(i));
    assert(count > 0);
    if ((i + count < pindex) && (passages[i].tile == passages[i+count].tile))
	{
	  assert(passages[i].fileid != passages[i+count].fileid);
	  return i+count;
 	}
    else
        return -1;
}

int which_duplicate_in_file(int i, passage *p) {
    unsigned long long tile = passages[i].tile;
    int fileid = passages[i].fileid;
    int j = i;

    assert(first_tile_in_file(i));
    assert((p->fileid == fileid) && (p->tile == tile));
    check_bug_8(j, pindex);
    for (; /* (j < pindex) && */ (passages[j].tile == tile) && (passages[j].fileid == fileid) && (passages[j].first_token != p->first_token); j++)
      check_bug_8(j, pindex);

    if ((j < pindex) && (passages[j].tile == tile) && (passages[j].fileid == fileid))
	return j-i;
    else
	return -1;
}

int nth_duplicate_in_file(int i, int n) {
    assert(first_tile_in_file(i));

    check_bug_7(i, n, pindex);

    if ( /* (i+n < pindex) && */ (passages[i].tile == passages[i+n].tile) && (passages[i].fileid == passages[i+n].fileid))
	return i+n;
    else
        return -1;
}

/*
Scan the entire database and mark those tiles that are too common (according to a paramater
set in the current configuration) as coming from a special "no match" file.  Note that
only the first occurrence of each "too common" tile is marked in this way. 
*/
void dont_match_common_tiles() {
    int i = 0, count = 0; 
    for(; i < pindex; i += count) {
	count = num_copies_of_tile(i);
	if (count > config.max_matches)
	    passages[i].fileid = config.nomatch_index;
     }	
}

void dump_passage_array_index() {
    int i = 0;
    fprintf(stderr, "[");
    for (; i < PASSAGE_INDEX_ARRAY_SIZE; i++) {
	fprintf(stderr, " %d",passage_index[i]);
    }
    fprintf(stderr, " ]\n");
}

/*
Find the first occurrence of a particular tile in the databse.
This function uses the index for fast access.
*/
int lookup_tile(passage *p) {
    int i = passage_index[PASSAGE_INDEX_HASH(p->tile)];
    for(; (passages[i].tile < p->tile) && (i < pindex); i++);
    if ((i == pindex) || (passages[i].tile != p->tile))
	return -1;
    else
	return i;
}

/*
Find the first occurrence of a particular tile in a particular file.
The input i must be the index of the first occurrence of the tile in the database.
*/
int lookup_tile_in_file(int i, int fileid) {
   unsigned long long tile = passages[i].tile;

   assert(first_tile(i));
   for (; (i < pindex) && (passages[i].tile == tile) && (passages[i].fileid != fileid); i++);
   if ((i < pindex) && (passages[i].tile == tile) && (passages[i].fileid == fileid))
     return i;
   else
     return -1;
}

/*  

Process a passage selected by winnowing to see if it matches any passages in the database.
This code relies on the sort of the tile array having the property that passages are sorted by tile, 
then file, then token offset.  There are five ways a tile in file X may fail to match any tile in file Y:

  1. File X has a higher index (in the files array) than Y.  X can only match files with higher indices;
     in this way we avoid reporting matches for X vs. Y and again for Y vs. X.
  2. The tile is too common.
  3. The tile is in a base file.
  4. The tile is the nth copy in X, and there are less than n copies of the tile in Y.
     This also handles the case where the tile does not occur in Y.
  5. X and Y are in the same set.


*/
match_info *process(passage *p, match_info *f) 
{
  int match_index = lookup_tile(p);
  int duplicate, next, next_duplicate;

  assert(match_index != -1);   /* certainly we added this passage before! */
  assert(first_tile(match_index)); /* and it needs to be the very first tile of all copies in all files */

  if (passages[match_index].fileid == config.nomatch_index)
    {
      if (DEBUG) fprintf(stderr,"NO MATCH: Match is too common.\n");
      return f;
    }


  if (files[passages[match_index].fileid].setid == config.nomatch_set)
    {
      if (DEBUG) fprintf(stderr,"NO MATCH:Matches base file.\n");
      return f;
    }

  match_index = lookup_tile_in_file(match_index,p->fileid);
  assert(match_index != -1);

  duplicate = which_duplicate_in_file(match_index, p);
  if (DEBUG) fprintf(stderr,"This tile is the %d duplicate in file %s\n",duplicate,files[p->fileid].remotename);
  assert(duplicate >= 0);

  for(next = next_file_for_tile(match_index); next != -1; next = next_file_for_tile(next)) {
    /* We use a heuristic to deal with multiple copies of the same tile in the same file.
       The nth copy of a tile (in file order) can only match the nth copy of the same tile in another file. */
	next_duplicate = nth_duplicate_in_file(next, duplicate);
 	if (next_duplicate == -1) {
	  if (DEBUG) fprintf(stderr,"NO MATCH: file %s does not have a matching duplicate.\n",
		  files[passages[next].fileid].remotename);
	  continue;
	}

	/* Skip any matches within the same set. */
	if (files[passages[match_index + duplicate].fileid].setid ==
	    files[passages[next_duplicate].fileid].setid) {
	  if (DEBUG) fprintf(stderr,"NO MATCH: File %s has a match in the same set %d",
			     files[passages[next_duplicate].fileid].remotename,
			     files[passages[next_duplicate].fileid].setid);
	  continue; 
	}

	/* If we get this far we have found a legitimate match, and we add it to the list of matches. */
	f = new_match_info(&passages[match_index + duplicate], &passages[next_duplicate], f);
	if (DEBUG) dump_2passage("MATCH: ", &passages[match_index + duplicate], &passages[next_duplicate]);
  }
  return f;
}

/*
  For CAP, we show all matches, including duplicates.  To keep things from getting
out of hand we currently do not show more than 200 matches per tile.
*/
match_info *process_cap(passage *p, match_info *f) 
{
  int limit = 200;
  int match_index = lookup_tile(p);
  passage *q;

  if (match_index == -1) return f;
  assert(first_tile(match_index)); 

/*
  We aren't using base files in CAP at the moment.

  if (files[passages[match_index].fileid].setid == config.nomatch_set)
    {
      if (DEBUG) fprintf(stderr,"NO MATCH:Matches base file.\n");
      return f;
    }
*/

  for(q = &passages[match_index]; q->tile == p->tile; q++) {
    if (limit-- == 0) {
      if (DEBUG) fprintf(stderr,"Match limit exceeded in process_cap; dropping matches.\n");
      break;
    }
    f = new_match_info(p, q, f);
    if (DEBUG) dump_2passage("MATCH: ", p, q);
  }
  return f;
}


match_info *handle_query(query_kinds query, passage *p, match_info *f) {
  switch (query) {
  case STORE: 
    store_passage(p);  
    return NULL;
  case QUERY_NO_DUPLICATES:
    return process(p,f);
  case QUERY_SHOW_DUPLICATES:
    return process_cap(p,f);
  default:
    fprintf(stderr,"Unrecognized query mode %d!\n",query);
    exit(-1);
  }
  return(0);  /* to make the compiler happy */
}
	   

/*
  Process each function in a file, building the database and recording
  the list of matched functions from other programs for later use.
*/
match_info *process_file_pass1(int fileid, query_kinds query)
{
  passage p[config.winnowing_window_size];
  int i, first, last, passage_index = 0, min_index=0;
  char *filename;
  FILE *fin;
  match_info *f = NULL;

  assert(fileid >= 0);
  filename = files[fileid].localname;
  config.language = files[fileid].language;
  files[fileid].size = 0;
  files[fileid].lines = 0;
  if (DEBUG) fprintf(stderr, "Pass 1: Processing file %s   fileid %d   query %d\n", filename,fileid,query);
  fin = open_file(filename);
  if (fin == NULL) return NULL;

  initialize_token_sequence(fin, fileid);
  if (DEBUG) {
    fprintf(stderr,"processing tokens ");
    dump_circular_buffer(token_sequence, token_index, config.tile_size, config.token_window_size);
  }

  for(i = 0; i < config.winnowing_window_size; i++)
	p[i].tile = MAXINT; 

  p[passage_index].tile = first_hash(token_sequence,token_index, config.tile_size, config.token_window_size);
  if (DEBUG) fprintf(stderr, "first hash: %llx\n", p[passage_index].tile);

  /* set up passage info */
  first = 0;
  last = MODULO_MM(token_index, config.token_window_size);

  p[passage_index].first_token = token_sequence[0].tokennum;
  p[passage_index].first_line  = token_sequence[0].lineno;
  p[passage_index].last_token  = token_sequence[last].tokennum;
  p[passage_index].last_line   = token_sequence[last].lineno;
  p[passage_index].fileid      = fileid;

  f = handle_query(query,&p[passage_index],f);

  passage_index = MODULO_PP(passage_index, config.winnowing_window_size);

  while(read_token(fin, fileid) == 0)
    {
	if (DEBUG) {
	  fprintf(stderr,"processing tokens ");
	  dump_circular_buffer(token_sequence, token_index, config.tile_size, config.token_window_size);
	}

      p[passage_index].tile = 
	NEXT_HASH(token_sequence, token_index, config.token_window_size, config.tile_size, p[MODULO_MM(passage_index,config.winnowing_window_size)].tile);

      assert(p[passage_index].tile == first_hash(token_sequence, token_index, config.tile_size, config.token_window_size));

      first = MODULO_SUBTRACT(token_index,config.tile_size,config.token_window_size);
      last = MODULO_MM(token_index, config.token_window_size); 
     
      p[passage_index].first_token = token_sequence[first].tokennum;
      p[passage_index].first_line  = token_sequence[first].lineno;
      p[passage_index].last_token  = token_sequence[last].tokennum;
      p[passage_index].last_line   = token_sequence[last].lineno;
      p[passage_index].fileid      = fileid;
      if (DEBUG) {
	dump_passage("next passage ",&p[passage_index]);
	fprintf(stderr, "min_index: %d\npassage_index: %d\n\n", min_index, passage_index);
      }

      /* winnowing: local min */
      if (min_index == passage_index) {
	int i;
 	for(i = passage_index + config.winnowing_window_size; i > passage_index; i--)
 	   if (p[i % config.winnowing_window_size].tile <= p[min_index].tile)
 		min_index = i % config.winnowing_window_size;

	f = handle_query(query,&p[min_index],f);
      }
      else 
        if (p[passage_index].tile <= p[min_index].tile) {
		min_index = passage_index;
		f = handle_query(query,&p[min_index],f);
	}

	else { if (DEBUG) fprintf(stderr,"Tile not selected by winnowing.\n"); }
      passage_index = MODULO_PP(passage_index, config.winnowing_window_size);

      if (DEBUG) {
	fprintf(stderr,"winnowing information: \n");
	fprintf(stderr,"   min index, value: %d %llx", min_index, p[min_index].tile);
	fprintf(stderr,"   winnowing buffer: ");
	for(i = 0; i < config.winnowing_window_size; i++)
		fprintf(stderr,"%llx ",p[i].tile);
	fprintf(stderr,"\n");
      }	
    }
  fclose(fin);
  return f;
}




/*******************************************************************************************

   PASS 2

   Pass 2 organizes the matches into a report.  Adjacent matches are first merged,
   then isolated small matches are discarded, and finally a stylized report is generated.


*******************************************************************************************/


/*
   Compute the total size of the matching passages in a resultinfo
(size is number of tokens matched).
*/
int size_of_match(resultinfo *r)
{
  match_info *f;
  int size = 0;

  if (r == NULL) return size;
  for(f = r->matches; f; f = f->next)
    size += INFO_SIZE(f);
  return size;
}

/*

  On entry we have, for each program, a list of all functions in all other programs that
are matched (the array program_matches).  Pass2 reorganizes this information into a
list of pairs of programs together with the associated list of common functions for each pair.
The matches are sorted and adjacent matches are merged.  Passages that are too small
(and therefore likely false positives) are filtered out after merging.  Finally,
the results are printed in a form suitable for perl script processing (moss_report.pl).

*/
void process_file_pass2(match_info *f)
{
  resultinfo *result = NULL;
  resultinfo *r;
  int fileid;
  match_info *next;

  if (f == NULL) return;
  fileid = f->this.fileid;

  /* A document will generally match many other documents.
     Take all the matches for a document and divide them into separate lists,
     where each list contains all of the matches for one other document.
  */
  for (; f != NULL;)
     {
        for (r = result; r != NULL; r = r->next)
	  if (f->other.fileid == r->fileid)
	    {
              next = f->next;
	      f->next = r->matches;
	      r->matches = f;
	      f = next;
	      break;
	    }
	if (r == NULL) {
	  result = new_resultinfo(f->other.fileid, f, result);
	  next = f->next;
	  f->next = NULL;
	  f = next;
	}
     }

  /* Now result has the list of matches on a per-document basis.
     The next step is to merge adjacent matches. */

  for (r = result; r != NULL; r = r->next)
    {
      /* 
	 For each document, put all the matches in an array.
	 This is just for convenience in implementing the following algorithms.
      */
      int len = 0;
      match_info *f;
      for(f = r->matches; f != NULL; f = f->next)
	len++;
      {
	match_info *fi[len], *final[len];
	int i = 0;
	int j, max;

	for(f = r->matches; f != NULL; f = f->next)
	  fi[i++] = f;

	qsort((void *) fi, len, sizeof(match_info *), match_info_leq);

	if (DEBUG) fprintf(stderr,"Merging adjacent passages.\n");
	/*
	  Compare all pairs of matches to see if any can be merged.  Repeat
	  entire process so long as one merge occurs in scanning all pairs.
	*/
	for(i = 0; i < len; i ++) {
	  if (fi[i] == NULL) continue;
	  for(j = i+1; j < len; j++) {
	    if (fi[j] == NULL) continue;
	    if ((PASSAGE_ADJACENT(fi[i]->this,fi[j]->this)) &&
		(PASSAGE_ADJACENT(fi[i]->other,fi[j]->other)) && 
		/* ensure the two adjacent passages overlap by the same number of tokens
		   in this and other */
		((fi[j]->this.last_token - fi[i]->this.last_token) ==
		 (fi[j]->other.last_token - fi[i]->other.last_token))
		)
	      {
		if (DEBUG) dump_4passage("adjacent: ", fi[i]->this, fi[j]->this, fi[i]->other, fi[j]->other);
		fi[i]->this.last_token  = fi[j]->this.last_token;
		fi[i]->this.last_line   = fi[j]->this.last_line;
		fi[i]->other.last_token = fi[j]->other.last_token;
		fi[i]->other.last_line  = fi[j]->other.last_line;
		if (DEBUG) dump_2passage("result: ", &(fi[i]->this), &(fi[i]->other));
		fi[j] = NULL;
	      }
	    else {
	      if (!(PASSAGE_ADJACENT(fi[i]->this,fi[j]->this)))
		break;
	    }
	  }
	}

	/* make a sweep through the fi array and eliminate null entries */
	j = 0;
	for (i = 0; i < len; i++) {
	  if (fi[i] == NULL) continue;
	  fi[j++] = fi[i];
	}
	len = j;

	if (DEBUG) fprintf(stderr,"Eliminating overlapping passages.\n");
	/*
	  At this point one passage may be matched against two different passages.
	  Eliminate any such double-counting as follows:
		the fi array is the "candidate" list
		select the largest remaining match M and add it to the "final" list
		scan all remaining elements of the candidate list and shorten
			them so that they do not overlap with M
		repeat
	*/

	j = -1;
	max = 0;
	while (max != -1) {
	  j++;
	  max = -1;
	  for(i = 0; i < len; i++) {
	    if (fi[i] == NULL) continue;
	    if ((max == -1) || (INFO_SIZE(fi[i]) > INFO_SIZE(fi[max])))
	      max = i;
	  }
	  if (max != -1) {
	    final[j] = fi[max];
	    fi[max] = NULL;
	    for(i = 0; i < len; i++)  
	      if (fi[i] != NULL) {
		if (PASSAGE_OVERLAP(fi[i]->this, final[j]->this)) {
		  if (DEBUG) dump_4passage("overlap: ",fi[i]->this, fi[i]->other, final[j]->this, final[j]->other);
		  /* 
		     There are four possible cases here:
		     final[j] is a proper subset of fi[i]
		     This can't happen, as we chose max to be the largest possible match.
		     fi[i] is a subset of final[j]
		     In this case fi[i] is just deleted from the array.   
		     fi[i] overlaps final[j]
		     Either shorten the beginning or end of fi[i], so it no longer overlaps final[j]
		  */
		  assert(!(PASSAGE_SUBSET(final[j]->this,fi[i]->this)) || (PASSAGE_SIZE(final[j]->this) == PASSAGE_SIZE(fi[i]->this)));
		  if (PASSAGE_SUBSET(fi[i]->this,final[j]->this)) {
		    fi[i] = NULL;
	          }
		  else
		    {
		      int diff;
		      if (fi[i]->this.first_token < final[j]->this.first_token) {
			diff = fi[i]->this.last_token - (final[j]->this.first_token - 1);
			fi[i]->this.last_token -= diff;
			fi[i]->other.last_token -= diff;
			diff = fi[i]->this.last_line - (final[j]->this.first_line - 1);
			fi[i]->this.last_line -= diff;
		  	fi[i]->other.last_line -= diff;
		      }
		      if (fi[i]->this.last_token > final[j]->this.last_token) {
			diff = (final[j]->this.last_token + 1) - fi[i]->this.first_token;
			fi[i]->this.first_token += diff;
			fi[i]->other.first_token += diff;
			diff = (final[j]->this.last_line + 1) - fi[i]->this.last_line;
			fi[i]->this.first_line += diff;
			fi[i]->other.first_line += diff;
		      }
		      if ((PASSAGE_SIZE(fi[i]->this) <= threshold) ||
			  (fi[i]->other.last_line < fi[i]->other.first_line) ||
			  (fi[i]->this.last_line < fi[i]->this.first_line))
			fi[i] = NULL;
		    }
		};
		
		if (!(fi[i] == NULL) && (PASSAGE_OVERLAP(fi[i]->other, final[j]->other))) {
		  /* Now we must check the same four cases for the "other" passage. */ 
		  assert(!(PASSAGE_SUBSET(final[j]->other,fi[i]->other)) || (PASSAGE_SIZE(final[j]->other) == PASSAGE_SIZE(fi[i]->other)));
		  if (DEBUG) dump_4passage("overlap 2: ",fi[i]->this, fi[i]->other, final[j]->this, final[j]->other);
		  if (PASSAGE_SUBSET(fi[i]->other,final[j]->other)) {
		    
		    fi[i] = NULL;}
		  else
		    {
		      int diff;
		      if (fi[i]->other.first_token < final[j]->other.first_token) {
			diff = fi[i]->other.last_token - (final[j]->other.first_token - 1);
			fi[i]->other.last_token -= diff;
			fi[i]->this.last_token -= diff;
			diff = fi[i]->other.last_line - (final[j]->other.first_line - 1);
			fi[i]->this.last_line -= diff;
			fi[i]->other.last_line -= diff;
		      }
		      if (fi[i]->other.last_token > final[j]->other.last_token) {
			diff = (final[j]->other.last_token + 1) - fi[i]->other.first_token;
			fi[i]->other.first_token += diff;
			fi[i]->this.first_token += diff;
			diff = (final[j]->other.last_line + 1) - fi[i]->other.first_line;
			fi[i]->other.first_line += diff;
			fi[i]->this.first_line += diff;
		      }
			  if ((PASSAGE_SIZE(fi[i]->other) <= threshold) ||
			      (fi[i]->other.last_line < fi[i]->other.first_line) ||
			      (fi[i]->this.last_line < fi[i]->this.first_line))
			    fi[i] = NULL;
		    }
		}
	      }
	  }
	}

	if (DEBUG) {
	  int x;
	  for(x = 0; x < j; x++) {
		dump_2passage("match: ",&(final[x]->this),&(final[x]->other));
	  }
	}

	r->matches = NULL;
	for(j--; j >= 0; j--) {
	  final[j]->next = r->matches;
	  r->matches = final[j];
	}
      }
    }
       
  /*
    Once a program has been processed, each comparison with another program
    is processed.
    */	
  for(; result != NULL; result = result->next)
    {
      int size = size_of_match(result);

      /* We report the number of lines matched to the user; this is 
	 probably the most intuitive measure. */
      int numlines_this = 0;
      int numlines_other = 0;
      match_info *m = result->matches;
      
      for(; m != NULL; m = m->next)
	{
	  numlines_this += (m->this.last_line - m->this.first_line) + 1;
	  numlines_other += (m->other.last_line - m->other.first_line) + 1;
	}
      fprintf(config.outfile,"\n%s + %s: tokens %d   lines %d", files[fileid].remotename, files[result->fileid].remotename, size, MAX(numlines_this,numlines_other));
      fprintf(config.outfile,"# total tokens %d + %d, total lines %d + %d, percentage matched %d%% + %d%%",
	      files[fileid].size,files[result->fileid].size,
	      files[fileid].lines, files[result->fileid].lines,
	      (int) (((float) size / (float) files[fileid].size) * 100.0),
	      (int) (((float) size / (float) files[result->fileid].size) * 100.0));
      for(m = result->matches; m != NULL; m = m->next)
	  fprintf(config.outfile,"# %d-%d, %d-%d: %d", 
		  m->this.first_line, m->this.last_line, 
		  m->other.first_line, m->other.last_line, 
		  INFO_SIZE(m));
    }
}

/*******************************************************************************************

  PRINT STATISTICS

This hook has been left in just in case we want to print statistics in the future.

*******************************************************************************************/

void statistics()
{

}


/*******************************************************************************************

   OPTION HANDLING


moss options

  -a file        Read commands from a new-style manifest file "file"

  -c		 Match comments (default is to ignore comments). 	

  -e filename    Error file.

  -g gap         The interval between saved hashes.

  -m #           Minimum match size; defaults to tile size + winnowing window size.
                 Smaller matches may arise from the subdivision of matches in pass2.

  -n maxmatches  If a particular passage is matched more than
                 maxmatches time, it is changed to a "no match"---not counted.

  -o outfile     write results to "outfile"

  -p             set the size of the passage array (-p sets the exponent x in for an array of size 2^x)
                 This option, if used, should always be first!

  -q             CAP-style queries instead of regular moss

  -r dbfile      read a database file (done before reading any files on the command line)
  -R dbfilelist  read a list of database files from the file dbfilelist (one db filename per line)

  -s dbfile      after pass 1, save the database to dbfile and exit

  -t #           Tile size.

  -v             Print statistics about database size and shape on termination.

  -w #           Winnowing window size.


*******************************************************************************************/

void init_options(configuration *c)
{
  /* set defaults */

  c->errfile = stderr;
  c->outfile = stdout;	
  c->max_matches = MAX_MATCHES;
  c->print_stats = 0;
  c->language = LangC;
  c->outfile = stdout;
  c->errfile = stderr;
  c->auxfile = NULL;
  c->manifest = NULL;
  c->token_window_size = DEFAULT_TOKEN_WINDOW_SIZE;
  c->tile_size = c->token_window_size - 1;
  c->user_max = 0;
  c->nomatch_index = -1;
  c->nomatch_set = 0;
  c->gap = GAP;
  c->cap = 0;
  c->winnowing_window_size = DEFAULT_WINNOWING_WINDOW_SIZE; 
  c->match_comment = 0;	
  c->passage_power = 20;
}

void print_statistics(int min_times) {
  int i,j, fb, pb, first, same_file;
  int distribution[50000];
  long long tile;

  for (i = 0; i < 50000; i++)
    distribution[i] = 0;

  fb = filesindex * sizeof(fileinfo);
  pb = pindex * sizeof(passage);
  printf("Number of files: %d\n",filesindex);
  printf("Number of passages: %d\n",pindex);
  printf("Size of files + passages in bytes: %d + %d = %d\n", fb, pb, fb+pb);
  sort_passage_array();
  for(i = 0; i < pindex; i++) {
    tile = passages[i].tile;
    j = 0;
    first = i;
    same_file = 0;
    for(; (i < pindex)  && (passages[i].tile == tile); i++) {
      if ((i > 0) && (passages[i].tile == passages[i-1].tile) && (passages[i].fileid == passages[i-1].fileid)) 
	same_file = 1;
      j++;
    }
    distribution[(j>=50000)?49999:j]++;
    if (j > min_times) {
	printf("%s: %d-%d  occurs %d times %s\n", files[passages[first].fileid].localname, passages[first].first_line, passages[first].last_line, j, same_file?"(same file)":"");
    }
  }
  printf("\n\nDistribution of tiles:\n");
  for(i = 0; i < 50000; i++) {
    if (distribution[i] != 0) {
      printf("\t%d: %d\n", i, distribution[i]);
    }
  }
}


void handle_options(int argc, char *argv[])
{
    int i;

    for(i = 0; i < argc; i++) {
	if (DEBUG) fprintf(stderr,"Command line: %s index: %d\n",argv[i],i); 
        if (STREQ(argv[i],"-a")) {
           config.manifest = argv[++i];	
           continue; }
        if (STREQ(argv[i],"-c")) {
           config.match_comment = 1;
           continue; }
        if (STREQ(argv[i],"-v")) {
           config.print_stats = 1;  /* This really isn't necessary, as we currently print statistics and exit. */  
	   print_statistics(atoi(argv[++i]));
           exit(0); }
        if (STREQ(argv[i],"-e")) {
           config.errfile = fopen(argv[++i], "a");
           if (config.errfile == NULL) {
	      fprintf(stdout,"Could not open error file %s.\n", optarg);
	      config.errfile = stderr; }
           continue; }
        if (STREQ(argv[i],"-g")) {
           config.gap = atoi(argv[++i]); /* optarg should be >= 0 */
           if (config.gap < 1)
	       {
	          fprintf(config.errfile,"Error: gap size of %d is out of range.  Setting gap to default %d\n", config.gap, GAP);
	          config.gap = GAP;
	       }
           continue; }
        if (STREQ(argv[i],"-m")) {
            threshold = atoi(argv[++i]); 
            if (threshold < 0) {
                fprintf(config.errfile,"Error: minimum match threshold %d is out of range.  Setting minimum match to default %d\n", threshold, DEFAULT_THRESHOLD);
                threshold = DEFAULT_THRESHOLD;
            }
            continue; 
        }
	if (STREQ(argv[i],"-n")) {
            config.user_max = 1;
            config.max_matches = atoi(argv[++i]); /* optarg should be >= 0 */
            if (config.max_matches <= 1)
	        {
	           fprintf(config.errfile,"Error: max matches %d is out of range.  Setting maximum match size to default %d\n", config.max_matches, MAX_MATCHES);
	           config.max_matches = MAX_MATCHES;
	        }
            continue; 
        }
        if (STREQ(argv[i],"-o")) {
            config.outfile = fopen(argv[++i], "w");
            if (config.outfile == NULL) {
	        fprintf(config.errfile,"Could not open output file \"%s\"; using standard output\n", optarg);
	        config.outfile = stdout;
            }
            continue; 
        }
	if (STREQ(argv[i],"-p")) {
	  config.passage_power = atoi(argv[++i]); /* optarg should be >= 0 */
	  if (config.passage_power <= 0)
	    {
	      fprintf(config.errfile,"Error: exponent %d must be positive\n", config.passage_power);
	      exit(1);
	    }
	  deleteregion(config.mem_database);
	  config.mem_database = newregion();
	  init_passage_array();
	  continue; 
	}
	
        if (STREQ(argv[i],"-q")) {
	  config.cap = 1;
	  continue; 
        }
        if (STREQ(argv[i],"-r")) {
	  read_database(argv[++i], &files[filesindex], &filesindex, MAXFILES, ps, &pindex, PASSAGE_ARRAY_SIZE);
	  filesbase = filesindex;
	  ps = passages + pindex; 
	  continue; 
        }
        if (STREQ(argv[i],"-R")) {
	  FILE *dblist;
	  int result;
	  char dbfilename[1000];

	  dblist = open_file(argv[++i]);
	  if (dblist == NULL) exit(1);
	  while(1) {
	    result = fscanf(dblist,"%s",dbfilename);
	    if (result == EOF) break;
	    read_database(dbfilename, &files[filesindex], &filesindex, MAXFILES, ps, &pindex, PASSAGE_ARRAY_SIZE);
	    filesbase = filesindex;
	    ps = passages + pindex; 
	  }
	  continue;
        }
	if (STREQ(argv[i],"-s")) {
	  config.dbfile = argv[++i];
	  continue; 
        }
	if (STREQ(argv[i],"-t")) {
            config.tile_size = atoi(argv[++i]);
            config.token_window_size =  config.tile_size+1;
	    check_bug_4(config.token_window_size, TOKEN_WINDOW_SIZE_MAX);
            continue; 
        }
	if (STREQ(argv[i],"-w")) {
           config.winnowing_window_size=  atoi(argv[++i]); 
	   check_bug_9(config.winnowing_window_size, WINNOWING_WINDOW_SIZE_MAX);
           continue; 
        }
	fprintf(stderr,"Unrecognized option: %s\n",argv[i]);
	exit(1);
    }
}

void read_manifest(char *manifest_filename) {
  FILE *manifest;
  char localname[2000], *localfilename=NULL, language[50], *remotefilename, remotename[2000];
  int set, i;

  if (manifest_filename == NULL) return;
  manifest = open_file(manifest_filename);

  if (manifest == NULL) {
    fprintf(stderr,"Could not open manifest file %s\n",manifest_filename);
    exit(1);
  }
  for(i = 0; fscanf(manifest,"%s %d %s %s",localname, &set, language, remotename) != EOF; i++) {
    if (DEBUG) fprintf(stderr,"index: %d, local name: %s, set: %d, langauge: %s, file name: %s\n",filesindex, localname, set, language, remotename);
    localfilename = (char *) ralloc(config.mem_strings, strlen(localname)+1);
    strcpy(localfilename,localname);

    remotefilename = (char *) ralloc(config.mem_strings, strlen(remotename)+1);
    strcpy(remotefilename,remotename);

    files[filesindex].setid = set;
    files[filesindex].language = string2lang(language);
    files[filesindex].lines = 0;
    files[filesindex].localname = localfilename;
    files[filesindex].remotename = remotefilename;
    filesindex++;
  }
}
    
/*******************************************************************************************


MAIN



*******************************************************************************************/

int stc(const void *s1, const void*s2) { 
     const char *st1 = *((char **) s1);
     const char *st2 = *((char **) s2);
     return strcmp(st1, st2); 
}

int count_array_size(char *s[]) {
    int i;
    for (i = 0; s[i] != NULL; i++);
    return i;
}

void files_init() {
  int i;
  for(i = 0; i < arrsize; i++)
    {
      files[i].size  = 0;            /* keep track of size of each file; initially 0 */
      files[i].lines = 0;            /* keep track of number of lines in each file; initially 0 */
      files[i].setid = 0;            /* we only keep matches between files in different sets */
      files[i].language = LangNone;
      files[i].localname = NULL;
      files[i].remotename = NULL;
    }
}

int region_main(int argc, char *argv[]) 
{
  int i;
  char filein[1000];


  /* allocate regions for data structures */
  config.mem_strings = newregion();	
  config.mem_database = newregion();
  config.mem_index = newregion();	
  config.mem_tokenization = newregion();	
  config.mem_matches = newregion();	


  /* Initialization */
  files_init();
  init_options(&config);
  init_passage_array();
  handle_options(argc-1, argv+1);                  /* command line options; skip over argv[0] */
  base_calc(config.tile_size);                     /* initialize substring hashing parameters */

  /*
    We have two modes: cap and moss.  Moss processes batch jobs and builds databases;
    cap handles on-line queries.
  */
  if (!(config.cap)) {
    read_manifest(config.manifest);

    /*
      If the user didn't give us a setting for the max number of matches
      for a passage, then set max_matches to about 1/3rd of the number of programs.
    */
    if (! config.user_max) config.max_matches = (filesindex / 3) + 2;
    /* process all source files */
    for(i = filesbase; i < filesindex; i++)
      process_file_pass1(i,STORE);
    
    if (config.dbfile != NULL) {
      write_database(config.dbfile, files, filesindex, passages, pindex);
      exit(0);
    }

    sort_passage_array();
    if (DEBUG) {fprintf(stderr,"Sorted passages\n"); dump_passage_array(); };
    dont_match_common_tiles();	
    build_passage_array_index();
    if (DEBUG) dump_passage_array_index(); 

    for(i = filesbase; i < filesindex; i++)
      process_file_pass2(process_file_pass1(i,QUERY_NO_DUPLICATES));

    statistics();
    return 0;	
  }
  else {
    int res;
    sort_passage_array();
    build_passage_array_index();
    while (1) {
      config.mem_strings = newregion();
      config.mem_tokenization=config.mem_strings;
      config.mem_matches=config.mem_strings;
      fflush(stdout);
      if (DEBUG) {
	fprintf(stderr,"waiting\n");
	fflush(stderr);
      }
      res = fscanf(stdin,"%s", filein);
      if (res == EOF) exit(1);
      if (DEBUG) {
	fprintf(stderr,"read %s with return code %d \n",filein,res);
      }
      if (strcmp(filein,"end") == 0) return(0);
      files[filesindex].remotename = "submission";
      files[filesindex].localname = filein;
      files[filesindex].setid = 1;
      files[filesindex].language = LangC;
      config.outfile = stdout;
      process_file_pass2(process_file_pass1(filesindex,QUERY_SHOW_DUPLICATES));
      fprintf(stdout,"\n==\n");
      fflush(stdout);	
      if (DEBUG) {
	fprintf(stderr,"finished %s\n",filein);
	fflush(stderr);
      }
      deleteregion(config.mem_strings);
    }
  }
  return(0);
}

