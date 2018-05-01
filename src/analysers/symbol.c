#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#include "symbol.h"
#include "../util/storage.h"
#include "../util/memory.h"
#include "../util/list.h"
#include "../util/helpers.h"
#include "../util/log.h"

SymbolList *symbolList;

void freeToken(void *data)
{
    Token *token = (Token *)data;
    free_memory(token->name);
    if(token->value != NULL) free_memory(token->value);
    if(token->dataLenght != NULL) free_memory(token->dataLenght);
    free_memory(token);
}

void newSymbolList()
{
    symbolList = (SymbolList *)allocate_memory(sizeof(SymbolList));
    symbolList->tokenCount = 0;
    list_new(&symbolList->tokens, sizeof(Token), freeToken);
}

Token *pushToken(char *name,
                 TokenType type,
                 TokenDataType dataType,
                 char *value,
                 char *dataLenght,
                 int lineIndex,
                 int startTokenIndex,
                 Token *scopeToken)
{
    Token *token = allocate_memory(sizeof(Token));

    token->name = (char *)allocate_memory(sizeof(char) * (strlen(name) + 1));
    memcpy(token->name, name, strlen(name) + 1);

    if (value != NULL)
    {
        token->value = (char *)allocate_memory(sizeof(char) * (strlen(value) + 1));
        memcpy(token->value, value, strlen(value) + 1);
    }
    else
    {
        token->value = NULL;
    }

    if (dataLenght != NULL)
    {
        token->dataLenght = (char *)allocate_memory(sizeof(char) * (strlen(dataLenght) + 1));
        memcpy(token->dataLenght, dataLenght, strlen(dataLenght) + 1);
    }
    else
    {
        token->dataLenght = NULL;
    }

    token->type = type;
    token->dataType = dataType;
    token->startCharIndex = startTokenIndex;
    token->startLineIndex = lineIndex;
    token->parent = scopeToken;

    list_append(&symbolList->tokens, token);
    symbolList->tokenCount++;

    const char *_ttype = type == argumento ? "(argumento)" : (type == variavel ? "(variavel)" : type == funcao ? "(funcao)" : type == principal ? "(princial)" : type == funcao_reservada ? "funcao_reservada" : "palavra_reservada");
    const char *_tdatatype = dataType == inteiro ? "inteiro" : (dataType == caractere ? "caractere" : "vazio");
    log_debug("Novo token: %s %s %s %s\n", _ttype, _tdatatype, token->name, dataLenght != NULL ? dataLenght : "");

    return token;
}

Token *getTokenByName(char *name)
{
    List *list = &symbolList->tokens;
    if (list->logicalLength == 0)
        return NULL;
    ListNode *node = list->head;
    while (node != NULL)
    {
        Token *token = (Token *)node->data;

        if (strcmp(name, token->name) == 0)
        {
            return token;
        }

        node = node->next;
    }

    return NULL;
}

Booleano _printToken(void *data)
{
    Token *token = (Token *)data;
    const char *_tdatatype = token->dataType == inteiro ? "inteiro" : (token->dataType == caractere ? "caractere" : "vazio");
    printf("|%10s|%10s|%16s|%10s|\n", _tdatatype, token->name, token->value, token->parent ? token->parent->name : "");
    return TRUE;
}

void destroySymbolList()
{
    list_destroy(&symbolList->tokens);
    free_memory(symbolList);
}

void printSymbolList()
{
    printf("---------------------------------------------------\n");
    printf("|   Tipo   |   Nome   | Possível Valor |  Escopo  |\n");
    list_for_each(&symbolList->tokens, _printToken);
    printf("---------------------------------------------------\n");
}