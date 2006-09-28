%{
  #include <stdio.h>
  #include <string.h>
  #include "Specie.h"
  #include "Species.h"
  #include "Experiments.h"
  
  using namespace std;

  int current_row = 0;
  int total_names = 0;
  int current_exp = 0;
  int current_col = 0;
  bool success = false;
  Experiments * globalExp;
  Species * globalSpecies;

  extern char * yytext;

  int yyerror(char*c);
  //extern "C" {
    int yylex(void);
  //}
%}

%union{
  char* name;
  float number;
}

%token <name> NAME
%token <number> NUMBER
%token QUOTE
%%

start: '(' '(' the_names ')' ',' lines ')'	{
											//current_exp++;
											success = true;
										}
  the_names: names					{
  										total_names = current_col;
  										current_row = 0;
  										current_col = 0;
  										//cout << "YACC names all matched\n";
									}
  names: QUOTE NAME QUOTE ',' {
  										//printf("NAME: %s\n",$2);
  										Specie * s = Specie::getInstance($2,current_col);
  										bool f = globalSpecies->addSpecie(s);
  										if (f == false){
  											cout << "YACC PARSER FOUND ERROR 1\n";
  											success = false;
  										}
  										current_col++;
  										//cout << "\tsuccess\n";
  								} names
  | QUOTE NAME QUOTE 				{
  										//printf("Last? NAME: %s\n",$2);
  										Specie * s = Specie::getInstance($2,current_col);
  										bool f = globalSpecies->addSpecie(s);
  										if (f == false){
  											cout << "YACC PARSER FOUND ERROR 2\n";
  											success = false;
  										}
  										current_col++;
  									}
  ;

  lines: '(' the_list ')' ',' lines	{
  										current_col = 0;
  									}
  | '(' the_list ')'				{
  										current_col = 0;
  									}
  the_list: list					{
		  							current_row++;
  									current_col = 0;
  									}

  list:  NUMBER ',' {
  							//printf("Matched %f\n",$1);
  							bool f = globalExp->addTimePoint(current_exp,current_row,current_col, $1);
  							if (f == false){
  								cout << "YACC PARSER FOUND ERROR 3\n";
  								success = false;
  							}
  							current_col++;
  						 } list
  | NUMBER 				 {
  							//printf("Matched %f\n",$1);
  							bool f = globalExp->addTimePoint(current_exp,current_row,current_col, $1);
							if (f == false){
  								cout << "YACC PARSER FOUND ERROR 4\n";
								success = false;
							}
  							current_col++;
  						 }
  ;

%%

  int yyerror(char*c){
    fprintf(stderr, "ERROR: '%s' with '%s'\n", c, yytext);
    return 1;
  }


extern FILE *yyin;

/*
main(){
  do{
    yyparse();
  }while(!feof(yyin));
}
*/



bool yaccParse(FILE * f, Species * S, Experiments * E, int experimentNum){
  success = false;
  yyin = f;
  globalSpecies = S;
  globalExp = E;
  current_exp = experimentNum;
  do{
    yyparse();
  }while(!feof(yyin));
  return success;
}
