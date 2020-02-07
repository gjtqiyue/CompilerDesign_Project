#ifndef SYMBOL_H
#define SYMBOL_H

//#include "miniTree.h"

typedef struct TYPE TYPE;
typedef struct PROG PROG;

#define HashSize 317

typedef struct SYMBOL {
    char *name;
    TYPE *type;
    struct SYMBOL *next;
} SYMBOL;

typedef struct SymbolTable {
    SYMBOL *table[HashSize];
    struct SymbolTable *parent;
} SymbolTable;

SymbolTable* initSymbolTable();
void symPROGRAM(PROG *s, SymbolTable* symbolTable);
#endif
