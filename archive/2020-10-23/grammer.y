/********************************
 *
 ********************************/
%{
#include <stdio.h>
void yyerror(char *s) {
	printf("%s\n", s);
}
int yywrap()
{
	return 1;
}
%}


%token SYMBOL
%token COLON
%token COMMA
%token ARROW
%token ASSIGN

%token PAREN_BEGIN
%token PAREN_END

%right ARROW
%right COMMA

%type definition

%%

program
	: definition

definition
	: SYMBOL COLON type {
		printf("%s(symbol) is typed as %s(type)!!\n", $1, $3);
	}
;
type
	: SYMBOL { $$ = $1; }
/*
	| PAREN_BEGIN type PAREN_END { $$ = $2; }
	| pi_type
	| sigma_type
;
pi_type
	: type ARROW type
;
sigma_type
	: type COMMA type
;
*/

%%

int main()
{
	yyparse();
}
