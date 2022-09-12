%{
#include <stdio.h>
int char_count = 0, word_count = 0, line_count = 1;
%}

%option noyywrap

%%

\n {
    ++char_count ;
    ++line_count ;
}

[^ \t\n\,\?\!\\\.\;\'\"\[\]\-\+\^\*\$\(\)\|]+ {
    char_count += yyleng ;
    ++word_count ;
    // ,?!\.;'"[]-+^%$()|
}

. {
    ++char_count ;
}

%%

int main() {
    // \,\?\!\\\.\;\'\"\[\]\-\+\^\*\$\(\)\|
    if ( (yyin = fopen("txt","r")) == NULL ) {
        printf("Can not open file txt!") ;
        return 0 ;
    }
    yylex() ;
    if ( char_count == 0 ) line_count = 0 ;
    printf("char:%d word:%d line:%d \n", char_count, word_count, line_count) ;
    fclose(yyin) ;
    return 0 ;
}