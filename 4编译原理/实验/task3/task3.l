%{
#include <stdio.h>
#include "task3.y.h"
%}

%option noyywrap

%%

[0-9]+ {
    yylval = atoi(yytext);
    return INTEGER;
}

[^ \t\n] {
    return yytext[0];
}

\n {
    return 0;
}

. {}

%%