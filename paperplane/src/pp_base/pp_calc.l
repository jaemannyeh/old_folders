
%{
/* A COMPACT GUIDE TO LEX & YACC by Tom Niemann */

#include "pp_calc_inc.h"
#include "pp_calc.tab.h"
%}

%%
[a-z]           { yylval.index = *yytext - 'a'; return VARIABLE; }
[0-9]+          { yylval.value = atoi(yytext); return INTEGER; }
[-()<>=+*/;{}.] { return *yytext; }
">="            return GE;
"<="            return LE;
"=="            return EQ;
"!="            return NE;
"while"         return WHILE;
"if"            return IF;
"else"          return ELSE;
"print"         return PRINT;

"delay"         return DELAY;
"exit"          return EXIT;

[ \t\n]+        ;
.               yyerror("Unknown character");
%%

int yywrap()
{
    return 1;
}
