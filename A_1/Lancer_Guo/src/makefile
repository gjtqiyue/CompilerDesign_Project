program: lex.yy.c y.tab.c
	gcc -o minic lex.yy.c y.tab.h y.tab.c main.h main.c -lfl

lex.yy.c:
	flex mini.l

y.tab.c:
	bison --yacc --verbose -d mini.y

clean:
	rm -f lex.yy.c y.tab.c y.output y.tab.h
