%{
#include <stdlib.h>
#include "y.tab.h"
%}

white		[ \t]
number		[1-9][0-9]*
symbol		[_a-zA-Z]+
colon		:
assign		:=
semicolon	;
back_slash	\\
arrow		->
dollar		\$
comma		,
period		\.
paren_begin	\(
paren_end	\)
block_begin	\{
block_end	\}

%%

{white}+
{number} {
	strcpy(yylval.str, yytext);
	return NUMBER;
}
{symbol} {
	strcpy(yylval.str, yytext);
	return SYMBOL;
}
{assign} {
	return ASSIGN;
}
{colon} {
	return COLON;
}
{period} {
	return PERIOD;
}
{dollar} {
	return DOLLAR;
}
{semicolon} {
	return SEMICOLON;
}
{back_slash} {
	return BACK_SLASH;
}
{arrow} {
	return ARROW;
}
{comma} {
	return COMMA;
}
{paren_begin} {
	return PAREN_BEGIN;
}
{paren_end} {
	return PAREN_END;
}
{block_begin} {
	return BLOCK_BEGIN;
}
{block_end} {
	return BLOCK_END;
}

%%
