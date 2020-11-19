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

%union {
	char str[256];
}


%token <str> SYMBOL
%token <str> NUMBER
%token COMMA
%token PERIOD
%token COLON
%token SEMICOLON
%token ARROW
%token ASSIGN

%token DOLLAR
%token BACK_SLASH

%token PAREN_BEGIN
%token PAREN_END
%token BLOCK_BEGIN
%token BLOCK_END

%left COMMA
%left ARROW

%type <str> judge
%type <str> typing
%type <str> assign

%type <str> any
%type <str> base_any

%type <str> term
%type <str> type
%type <str> term_and_type

%type <str> base_term
%type <str> application
//%type <str> abstraction

%type <str> base_type
%type <str> pre_type
%type <str> pi_type
%type <str> sigma_type

//%type <str> tuple

%%

program
	: /*none*/
	| judge SEMICOLON program
;
judge
	: typing
	| assign
;
typing
	: SYMBOL COLON type {
		printf("typing: %s(symbol) : %s(type)\n", $1, $3);
		sprintf($$, "(%s : %s)", $1, $3);
	}
;
assign
	: SYMBOL ASSIGN term {
		printf("assign: %s(symbol) := %s(term)\n", $1, $3);
		sprintf($$, "(%s := %s)", $1, $3);
	}
;

any
	: term
	| type
;
base_term
	: SYMBOL
	| PAREN_BEGIN term PAREN_END {
		printf("paren term: ( %s(term) )\n", $2);
		sprintf($$, "(%s)", $2);
	}
;
base_any
	: base_term
	| NUMBER
;
application
	: base_term
	| application base_any {
		printf("application: %s(term) %s(term)\n", $1, $2);
		sprintf($$, "(%s %s)", $1, $2);
	}
;

/*
base_type
	: SYMBOL
	| PAREN_BEGIN type PAREN_END {
		printf("paren type: ( %s(type) )\n", $2);
		sprintf($$, "(%s)", $2);
	}
;
*/
pre_type
	: application
	| PAREN_BEGIN typing PAREN_END {
		printf("pre type: %s(typing)\n", $2);
		sprintf($$, "(%s)", $2);
	}
;
pi_type
	: application
	| pre_type ARROW pi_type {
		printf("pi type: %s(type) -> %s(type)\n", $1, $3);
		sprintf($$, "(%s -> $s)", $1, $3);
	}
;
sigma_type
	: pi_type
	| pre_type COMMA sigma_type {
		printf("sigma type: %s(type) , %s(type)\n", $1, $3);
		sprintf($$, "(%s , $s)", $1, $3);
	}
;

tuple
	: application
	| pi_type
	|  COMMA sigma_type {
		printf("sigma type: %s(type) , %s(type)\n", $1, $3);
		sprintf($$, "(%s , $s)", $1, $3);
	}
;

term
	: tuple
	//: application
;
type
	: sigma_type
	//: pi_type
;

/*
*/

%%

int main()
{
	yyparse();
}
