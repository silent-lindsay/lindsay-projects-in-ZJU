#!/bin/sh
bison -d lab2.y
lex lab2.l
gcc -o parser *.c
./parser < input.txt
