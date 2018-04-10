#ifndef __LEXICO_H
#define __LEXICO_H

#include "../util/storage.h"

extern char * const reserved_words[];

typedef enum {
    variavel,
    funcao,
    argumento
} TokenType;

typedef enum {
    inteiro,
    caractere,
    decimal
} TokenDataType;

typedef struct _Token {
    TokenType type;
    TokenDataType dataType;
    char *tokenValue;
    char *tokenName;
    float tokenDataLenght;
    struct _Token *tokenParent;
    Line *line;
} Token;

typedef struct _SymbolList {
    int tokenCount;
    List tokens;
} SymbolList;

extern SymbolList *symbolList;

void analise_lexica(File *file);

#endif