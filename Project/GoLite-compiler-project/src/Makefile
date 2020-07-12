program: lex.yy.c y.tab.c tree.o weeder.o pretty.o symbol.o type.o codegen.o main.o
	gcc -o golitec lex.yy.c y.tab.h y.tab.c tree.o weeder.o pretty.o symbol.o type.o codegen.o main.o -lfl

lex.yy.c:
	flex scanner.l

y.tab.c:
	bison --yacc --verbose -d parser.y
	
tree.o:
	gcc -c tree.h tree.c

pretty.o:
	gcc -c pretty.h pretty.c
	
weeder.o:
	gcc -c weeder.h weeder.c

symbol.o:
	gcc -c symbol.h symbol.c

type.o:
	gcc -c type.h type.c

codegen.o:
	gcc -c codegen.h codegen.c
	
main.o:
	gcc -c main.h main.c

clean:
	rm -f lex.yy.c y.tab.c y.output y.tab.h *.o *.h.gch
	rm golitec
