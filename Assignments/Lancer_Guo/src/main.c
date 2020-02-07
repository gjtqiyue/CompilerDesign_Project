#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "miniTree.h"
#include "pretty.h"
#include "symbol.h"
#include "main.h"

int yyparse();
int yylex();

PROG *root;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Enter tokens|scan|parse|pretty|symbol|typecheck|codegen as options\n");
        return 1;
    }

    if (strcmp(argv[1], "tokens") == 0)
    {
        token = 1;
        while (yylex() != 0) {}
        return 0;
    }
    else if (strcmp(argv[1], "scan") == 0)
    {
        token = 0;
        while (yylex() != 0) {}    //for parsing purpose, yylex() only has to be called once
        printf("OK\n");
        return 0;
    }
    else if (strcmp(argv[1], "parse") == 0)
    {
        token = 0;
        if (yyparse() == 0) { 
		//need to check if error occurs
        	printf("OK\n");
	    }
        return 0;
    }
    else if (strcmp(argv[1], "pretty") == 0)
    {
        //printf("hello");
        prettySymbol = 0;
        printf("%d", prettySymbol);
        if (yyparse() == 0){
            prettyPROG(root, 0);
        }
        return 0;
    }
    else if (strcmp(argv[1], "symbol") == 0)
    {
        prettySymbol = 1;
        if (yyparse() == 0){
            SymbolTable *t = initSymbolTable();
            symPROGRAM(root, t);
            prettyPROG(root, 0);
        }

        return 0;
    }
    return 0;
}
