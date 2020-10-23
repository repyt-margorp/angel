%{
#include <stdlib.h>
#include "y.tab.h"
%}

white		[ \t]
integer		[1-9][0-9]*
symbol		[_a-zA-Z]+
colon		:
arrow		->
comma		,
paren_begin	\(
paren_end	\)

%%

{white}+
{symbol} {
	//strcpy(yylval.str, yytext);
	return SYMBOL;
}
{integer} {
	return SYMBOL;
}
{colon} {
	return COLON;
}
{arrow} {
	return ARROW;
}
{comma} {
	return COMMA;
}
{paren_begin}	{
	return PAREN_BEGIN;
}
{paren_end}	{
	return PAREN_END;
}

%%
