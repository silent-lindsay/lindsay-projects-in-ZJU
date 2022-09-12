#include "print.h"
#include "sbi.h"

void puts(char *s) {
    // unimplemented
    for (int i = 0; s[i] ; i++)
    {
        sbi_ecall(0x1,0x0, s[i], 0, 0,0,0,0) ;
        /* code */
    }
    
}

void recur(int x) {
    if (x>=10)
    {
        recur ( x/10 ) ;
        /* code */
    }
    x = x % 10 ;
    sbi_ecall(0x1,0x0,x+'0',0,0,0,0, 0) ;    
}

void puti(int x) {
    // unimplemented
    recur(x) ;
    
}
