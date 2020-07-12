#include "main.h"

int yyparse();
int yylex();
bool tokens = false;
bool print = false;
PROGRAM *root;


int main(int argc, char* argv[]) {

    if (argc < 2) {
        fprintf(stderr, "Arguments: scan|tokens|parse|pretty|symbol|typecheck|codegen file_name\n");
        exit(1);
    }

    if (strcmp("scan", argv[1]) == 0) {
        while (yylex() != 0);
        printf("OK\n");
	    return 0;
    }
    else if (strcmp("tokens", argv[1]) == 0) {
        tokens = true;
        while (yylex() != 0);
	    return 0;
    }
    else if (strcmp("parse", argv[1]) == 0) {
        if (yyparse() == 0) {
            weedPROGRAM(root);
            printf("OK\n");
	        return 0;
        }
    }
    else if (strcmp("pretty", argv[1]) == 0) {
        if (yyparse() == 0) {
		    weedPROGRAM(root);
	        prettyProgram(root);
	        return 0;
        }
    }
    else if (strcmp("symbol", argv[1]) == 0) {
        if (yyparse() == 0) {
            print = true;
            weedPROGRAM(root);
            symPROGRAM(root);
            return 0;
        }
    }
    else if (strcmp("typecheck", argv[1]) == 0) {
        if (yyparse() == 0) {
            print = false;
            weedPROGRAM(root);
            symPROGRAM(root);
            typePROGRAM(root);
        }
        printf("OK\n");
        return 0;
    }
    else if (strcmp("codegen", argv[1]) == 0) {
        if (yyparse() == 0) {
            print = false;
            weedPROGRAM(root);
            symPROGRAM(root);
            typePROGRAM(root);

            char *dir_path = dirname(argv[2]);
            //printf("dirpath: %s", dir_path);
            char *class_name = "Golite.java";
            char *file_name = malloc((strlen(argv[2])+strlen(class_name)+2)*sizeof(char));
            strcat(file_name, dir_path);
            strcat(file_name, "/");
            strcat(file_name, class_name);
            FILE *fp = fopen(file_name, "w");
            //printf("filename: %s", file_name);
            codePROGRAM(root, "Golite", fp);
            fclose(fp); 
            printf("OK\n");
            return 0;
        }
    }
    else {
	    fprintf(stderr, "Error: mode must be scan|tokens|parse|pretty|symbol|typecheck|codegen\n");
	    exit(1);
    }

}

