#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include <string.h>

void yyparse();
int yylex();
int token;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Enter tokens|scan|parse as options\n");
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
        yylex();    //for parsing purpose, yylex() only has to be called once
        printf("OK\n");
        return 0;
    }
    else if (strcmp(argv[1], "parse") == 0)
    {
        token = 0;
        yyparse();  //need to check if error occurs
        printf("OK\n");
        return 0;
    }
    return 0;
}
