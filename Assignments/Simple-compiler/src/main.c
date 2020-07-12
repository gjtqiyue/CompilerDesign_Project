#include <stdlib.h>
#include <string.h>
#include "miniTree.h"
#include "pretty.h"
#include "symbol.h"
#include "typeCheck.h"
#include "code.h"
#include "main.h"

int yyparse();
int yylex();
PROG *root;
FILE *f;
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
    else if (strcmp(argv[1], "typecheck") == 0)
    {
        if (yyparse() == 0){
            SymbolTable *t = initSymbolTable();
            symPROGRAM(root, t);
            typePROG(root);
            printf("OK\n");
        }

        return 0;
    }
    else if (strcmp(argv[1], "codegen") == 0)
    {
        if (argv[2] == NULL) {
            printf("File name is not provided for codegen\n");
        }

        char* filename = strcat(argv[2], ".c");
        f = fopen(filename, "w");
        if (f == NULL) {
            printf("Unable to create file %s\n", filename);
            exit(1);
        }

        //code to include before and after the generated code
        char* include = "#include <stdlib.h>\n#include <stdio.h>\n#include <string.h>\n#include <stdbool.h>\n\nint main() { \n";
        char* global = "\tchar* string_cat_array;\n\tchar LancerNiubi[1024];\n";
        char* end = "\treturn 0;\n}\n";

        fputs(include, f);
        fputs(global, f);

        if (yyparse() == 0){
            SymbolTable *t = initSymbolTable();
            symPROGRAM(root, t);
            typePROG(root);
            codePROG(root, 1);
        }

        fputs(end, f);
        fclose(f);

        return 0;
    }
    return 0;
}
