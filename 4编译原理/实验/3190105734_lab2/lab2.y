%{
#include <stdio.h>
#include <math.h>

int mypow(int x, int y);
int yylex(void);
int yyparse(void);
void yyerror(const char *);
%}

%token NUMBER '(' ')'
%left '+' '-'
%left '*' '/'
%right '^'

%%
program:
program expr '\n' { printf("%d\n", $2); }
|program '\n'
|
;

expr:
NUMBER { $$ = $1; }
| '(' expr ')' { $$ = $2; }
| expr '^' expr { $$ = mypow($1, $3); }
| expr '*' expr { $$ = $1 * $3; }
| expr '/' expr { $$ = $1 / $3; }
| expr '+' expr { $$ = $1 + $3; }
| expr '-' expr { $$ = $1 - $3; }
| '-' expr { $$ = -$2; }
;
%%

void yyerror(const char *s) {
	printf("%s\n", s);
}

int mypow(int x, int y)
{
	int ret = 1;	
	for(int i = 0; i < y; i++){
		ret *= x;
	}
	return ret;
}

int main(void) {
	yyparse();
	return 0;
}
