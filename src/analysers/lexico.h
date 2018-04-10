#ifndef __LEXICO_H
#define __LEXICO_H

#include "../util/storage.h"

extern char * const reserved_words[];

typedef enum {
    variavel = 0,
    funcao = 1,
    principal = 2,
    argumento = 3,
    funcao_reservada = 4,
    palavra_reservada = 5
} TokenType;

typedef enum {
    inteiro,
    caractere,
    decimal
} TokenDataType;

typedef struct _Token {
    TokenType type;
    TokenDataType dataType;
    char* tokenValue;
    char *tokenName;
    float tokenDataLenght;
    struct _Token *tokenParent;
    Line *line;
} Token;

typedef struct _SymbolList {
    int tokenCount;
    List tokens;
} SymbolList;

void lexical_analysis(File *file);

#endif