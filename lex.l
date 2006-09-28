%{
#include "yacc.tab.h"

#include <string.h>
#include <stdio.h>

extern YYSTYPE yylval;

%}

%%
[\t \n]+ ;
["]     return QUOTE;
<<EOF>> return 0;
        [A-Za-z\_\'\.][A-Za-z\_\'\.0-9]*   { yylval.name = strdup(yytext);
                              return NAME;}
        [0-9]+([.][0-9]+)*  {yylval.number = atof(yytext);
                             return NUMBER;}
. {return yytext[0];}
%%

//to avoid a complaint by yacc
int yywrap(){
  return 1;
}

