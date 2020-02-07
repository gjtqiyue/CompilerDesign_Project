program: lex.yy.c y.tab.c miniTree.o pretty.o symbol.o
	gcc -o minic lex.yy.c y.tab.h y.tab.c symbol.o pretty.o miniTree.o main.h main.c -lfl

lex.yy.c:
	flex mini.l

y.tab.c:
	bison --yacc --verbose -d mini.y

miniTree.o:
	gcc -c miniTree.h miniTree.c 

pretty.o:
	gcc -c pretty.h main.h pretty.c

symbol.o:
	gcc -c symbol.h miniTree.h symbol.c

clean:
	rm -f lex.yy.c y.tab.c y.output y.tab.h *.o *.h.gch
