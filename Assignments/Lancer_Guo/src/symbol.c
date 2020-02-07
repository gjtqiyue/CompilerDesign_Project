#include "symbol.h"
#include "miniTree.h"
#include "string.h"
#include <stdlib.h>
#include <stdio.h>

SymbolTable* initSymbolTable() {
    SymbolTable *t = malloc(sizeof(SymbolTable));

    for (int i=0; i<HashSize; i++){
        t->table[i] = NULL;
    }

    t->parent = NULL;
    return t;
}

SymbolTable* scopeSymbolTable(SymbolTable *s) {
    SymbolTable *t = initSymbolTable();
    t->parent = s;
    return t;
}

int Hash(char *str) {
    unsigned int hash = 0;
    while (*str) hash = (hash << 1) + *str++;
    return hash % HashSize;
}

SYMBOL* putSymbol(SymbolTable *t, char *name, TYPE *kind) {
    int  i = Hash(name);

    for (SYMBOL *s = t->table[i]; s; s = s->next) {
        if (strcmp(s->name, name) == 0) //throw an error because all identifiers are unique 
        {
            fprintf(stderr, "Error: symbol %s is already defined\n", name);
            exit(1);
        }
    }

    SYMBOL *s = malloc(sizeof(SYMBOL));
    s->name = name;
    s->type = kind;
    s->next = t->table[i];
    t->table[i] = s;
    return s; 
}

// when using a variable, we check all the scopes
SYMBOL* getSymbol (SymbolTable *t, char *name) {
    int i = Hash(name);
    
    // Check the current scope
    for (SYMBOL *s = t->table[i]; s; s = s->next) {
        if (strcmp(s->name, name) == 0) return s;        
    }

    // Check for existence of a parent scope
    if (t->parent == NULL){
        return NULL;
    }

    // Check the parent scopes
    return getSymbol(t->parent, name);
} 

// when declaring a variable, we only check current scope
SYMBOL* getSymbolScope (SymbolTable *t, EXP *identifier) {
    char* name = identifier->val.identifier.identifier;
    int i = Hash(name);

    // Check the current scope
    for (SYMBOL *s = t->table[i]; s; s = s->next) {
        if (strcmp(s->name, name) == 0) return s;        
    }
} 

SYMBOL* symVar(char *name, SymbolTable *symbolTable, int lineno){
    SYMBOL *s = getSymbol(symbolTable, name);
    if (s == NULL){
        // throw an error: undefined
        fprintf(stderr, "Error: line(%d) symbol %s is not defined\n", lineno, name);
        exit(1);
    }
    return s;
}

void symEXP(EXP *e, SymbolTable *symbolTable){
    switch(e->kind){
        case k_TermKindIdentifier:
            e->val.identifier.sym = symVar(e->val.identifier.identifier, symbolTable, e->lineno);
            break;
        case k_ExpressionKindAdd:
            symEXP(e->val.binary.lhs, symbolTable);
            symEXP(e->val.binary.rhs, symbolTable);
            break;
        case k_ExpressionKindMinus:
            symEXP(e->val.binary.lhs, symbolTable);
            symEXP(e->val.binary.rhs, symbolTable);
            break;
        case k_ExpressionKindTimes:
            symEXP(e->val.binary.lhs, symbolTable);
            symEXP(e->val.binary.rhs, symbolTable);
            break;
        case k_ExpressionKindDivide:
            symEXP(e->val.binary.lhs, symbolTable);
            symEXP(e->val.binary.rhs, symbolTable);
            break;
        case k_ExpressionKindAnd:
            symEXP(e->val.binary.lhs, symbolTable);
            symEXP(e->val.binary.rhs, symbolTable);
            break;
        case k_ExpressionKindOr:
            symEXP(e->val.binary.lhs, symbolTable);
            symEXP(e->val.binary.rhs, symbolTable);
            break;
        case k_ExpressionKindNot:
            symEXP(e->val.unary.exp, symbolTable);
            break;
        case k_ExpressionKindParentheses:
            symEXP(e->val.paren.exp, symbolTable);
            break;
        case k_ExpressionKindGEQ:
            symEXP(e->val.binary.lhs, symbolTable);
            symEXP(e->val.binary.rhs, symbolTable);
            break;
        case k_ExpressionKindLEQ:
            symEXP(e->val.binary.lhs, symbolTable);
            symEXP(e->val.binary.rhs, symbolTable);
            break;
        case k_ExpressionKindGreater:
            symEXP(e->val.binary.lhs, symbolTable);
            symEXP(e->val.binary.rhs, symbolTable);
            break;
        case k_ExpressionKindSmaller:
            symEXP(e->val.binary.lhs, symbolTable);
            symEXP(e->val.binary.rhs, symbolTable);
            break;
        case k_ExpressionKindEqual:
            symEXP(e->val.binary.lhs, symbolTable);
            symEXP(e->val.binary.rhs, symbolTable);
            break;
        case k_ExpressionKindNotEqual:
            symEXP(e->val.binary.lhs, symbolTable);
            symEXP(e->val.binary.rhs, symbolTable);
            break;
        case k_ExpressionKindUMinus:
            symEXP(e->val.unary.exp, symbolTable);
            break;
    }
}

void symPROGRAM(PROG *s, SymbolTable* symbolTable) {
    if (s == NULL){
        return;
    }

    if (s->next != NULL){
        symPROGRAM(s->next, symbolTable);
    }

    switch (s->kind) {
        case k_StatementKindWhile:
            //check condition
            symEXP(s->val.while_stmt.cond, symbolTable);
            //check body
            SymbolTable *next_table = scopeSymbolTable(symbolTable);
            symPROGRAM(s->val.while_stmt.body, next_table);
            break;
        case k_StatementKindIf:
            //check condition
            symEXP(s->val.if_stmt.cond, symbolTable);
            //check body
            SymbolTable *body_table = scopeSymbolTable(symbolTable);
            symPROGRAM(s->val.if_stmt.body, body_table);
            //check else part
            if (s->val.if_stmt.elseif_stmt != NULL){
                SymbolTable *else_table = scopeSymbolTable(symbolTable);
                symPROGRAM(s->val.if_stmt.elseif_stmt, else_table);
            }
            break;
        case k_StatementKindElse:
            symPROGRAM(s->val.else_stmt.body, scopeSymbolTable(symbolTable));
            break;
        case k_StatementKindRead:
            //check expression
            symEXP(s->val.read.input, symbolTable);
            break;
        case k_StatementKindPrint:
            symEXP(s->val.print.output, symbolTable);
            break;
        case k_StatementKindAssign:
            //check expression
            symEXP(s->val.assign.exp, symbolTable);
            //check the identifier
            symEXP(s->val.assign.name, symbolTable);
            break;
        case k_StatementKindDeclaration:
            symEXP(s->val.decl.exp, symbolTable);
            EXP *name1 = s->val.decl.name;
            // if (getSymbolScope(symbolTable, name1) != NULL){
            //     fprintf(stderr, "Error: line(%d) redeclaration of symbol %s\n", s->lineno, name1->val.identifier.identifier);
            //     exit(1);
            // }
            SYMBOL *sym1 = putSymbol(symbolTable, name1->val.identifier.identifier, NULL);
            name1->val.identifier.sym = sym1;
            //printf("%s: <Infer>", name1->val.identifier.sym->name);
            break;
        case k_StatementKindDeclarationType:
            symEXP(s->val.decl_type.exp, symbolTable);
            EXP *name2 = s->val.decl_type.name;
            // if (getSymbolScope(symbolTable, name2) != NULL){
            //     fprintf(stderr, "Error: line(%d) redeclaration of symbol %s\n", s->lineno, name2->val.identifier.identifier);
            //     exit(1);
            // }
            SYMBOL *sym2 = putSymbol(symbolTable, name2->val.identifier.identifier, s->val.decl_type.type);
            name2->val.identifier.sym = sym2;
            //printf("%s: %s", name2->val.identifier.sym->name, name2->val.identifier.sym->kind);
            break;
    }
}