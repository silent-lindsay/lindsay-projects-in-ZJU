%{
#include "lab2.tab.h"
#include <math.h>
void yyerror(const char *);

%}

%%
[0-9]+       	{ yylval = atoi(yytext); return NUMBER; }
[-+*/^()\n]     return *yytext;
[ \t]			;
[q]				return 0;
.            	yyerror("无效字符");
%%

int yywrap(void) {
	return 1;
}
