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
  int * speciesCol;
  int i;

  extern char * yytext;

  int yyerror(const char*c);
  //extern "C" {
    int yylex(void);
  //}
%}

%union{
  char* name;
  double number;
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
    total_names = Specie::getNumSpecie();
  										current_row = 0;
  										current_col = 0;
  										//cout << "YACC names all matched\n";
									}
  names: QUOTE NAME QUOTE ',' {
    int i = Specie::getSpecies($2);
    //    printf("READING: %s i=%d in col %d\n",$2,i,current_col);
    if (i >= 0) { 
      //printf("NAME: %s i=%d in col %d\n",$2,i,current_col);
      speciesCol[i]=current_col;
    }
    current_col++;
    //cout << "\tsuccess\n";
  								} names
  | QUOTE NAME QUOTE 				{
    int i = Specie::getSpecies($2);
    //printf("READING: %s i=%d in col %d\n",$2,i,current_col);
    if (i >= 0) { 
      //printf("Last? NAME: %s i=%d in col %d\n",$2,i,current_col);
      speciesCol[i]=current_col;
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
    for (i=0;i<Specie::getNumSpecie();i++) {
      if (speciesCol[i] == (-1)) {
	globalExp->addTimePoint(current_exp,current_row,i, 0);
      }
    }
		  							current_row++;
  									current_col = 0;
  									}

  list:  NUMBER ',' {
    for (i=0;i<Specie::getNumSpecie();i++)
      if (speciesCol[i]==current_col) 
	break;
    if (i < Specie::getNumSpecie()) { 
      //printf("Matched %i %f\n",i,$1);
      bool f = globalExp->addTimePoint(current_exp,current_row,i, $1);
      if (f == false){
	cout << "YACC PARSER FOUND ERROR 3\n";
	success = false;
      }
    }
    current_col++;
 } list
  | NUMBER 				 {
    for (i=0;i<Specie::getNumSpecie();i++)
      if (speciesCol[i]==current_col) 
	break;
    if (i < Specie::getNumSpecie()) { 
      //printf("Matched %i %f\n",i,$1);
      bool f = globalExp->addTimePoint(current_exp,current_row,i, $1);
      if (f == false){
	cout << "YACC PARSER FOUND ERROR 4\n";
	success = false;
      }
    }
    current_col++;
   }
  ;

%%

  int yyerror(const char*c){
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
  speciesCol = new int[Specie::getNumSpecie()];
  for (int i = 0; i < Specie::getNumSpecie(); i++) {
    speciesCol[i] = (-1);
  }
  success = false;
  yyin = f;
  globalSpecies = S;
  globalExp = E;
  current_exp = experimentNum;
  do{
    yyparse();
  }while(!feof(yyin));
  delete[] speciesCol;
  return success;
}
