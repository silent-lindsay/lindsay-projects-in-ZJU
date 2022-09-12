all:
	bison --defines='task3.y.h' -o 'task3.y.c' 'task3.y'
	lex -t 'task3.l' >'task3.l.c'
	gcc 'task3.y.c' 'task3.l.c' -o 'task3'

clean:
	rm -f *.h *.c task3