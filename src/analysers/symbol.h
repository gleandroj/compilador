#ifndef __SYMBOL_H
#define __SYMBOL_H

#include "../util/list.h"

typedef enum {
    nao_identificado = -1,
    variavel = 0,
    funcao = 1,
    principal = 2,
    argumento = 3,
    funcao_reservada = 4,
    palavra_reservada = 5
} TokenType;

typedef enum {
    vazio,
    inteiro,
    caractere,
    decimal
} TokenDataType;

typedef struct _Token
{
    TokenType type;
    TokenDataType dataType;
    char *value;
    char *name;
    char *dataLenght;
    struct _Token *parent;
    int startLineIndex;
    int startCharIndex;
    int endCharIndex;
    int endLineIndex;
} Token;

typedef struct _SymbolList
{
    int tokenCount;
    List tokens;
} SymbolList;

/**
 * Inicializa a tabela de simbolos
 */
void newSymbolList();

/**
 * Insere token na tabela de simbolos
 */
Token *pushToken(char *name,
                 TokenType type,
                 TokenDataType dataType,
                 char *value,
                 char *dataLenght,
                 int lineIndex,
                 int startTokenIndex,
                 Token* scopeToken);

/**
 * Busca tokens por nome na tabela de simbolos
 */
Token *getTokenByName(char *name);

/**
 * Imprime Tabela de Simbolos
 */
void printSymbolList();

extern SymbolList *symbolList;

#endif