%{
#include <stdio.h>
#include <ctype.h>
%}

%token INTEGER

%%

command : exp {
    printf("%d\n", $1);
}
        ; /* allows printing of the result */

exp : exp '+' term {
    $$ = $1+$3 ;
}
    | exp '-' term {
        $$ = $1-$3 ;
}
    | term { $$ = $1; }
    ;

term : term '*' factor {
        $$ = $1*$3 ;
}
     | term '/' factor {
         $$ = $1/$3 ;
}
     | factor { $$ = $1; }
     ;

factor : INTEGER { $$ = $1; }
       | '(' exp ')' { $$ = $2; }
       ;

%%

extern FILE* yyin ;

void main() {
    while ( yyin != EOF ) 
        yyparse() ;
    // return yyparse();
}

/* allows for printing of an error message */
int yyerror(char *s) {
    fprintf(stderr, "%s\n", s);
    return 0;
}