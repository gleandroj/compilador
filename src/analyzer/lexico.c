#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../util/typings.h"
#include "../util/log.h"
#include "../util/list.h"
#include "../util/memory.h"
#include "../util/helpers.h"
#include "lexico.h"

SymbolList* symbolList = NULL;

void free_token(void *data){
    Token *token = (Token *)data;
    free_memory(token->tokenName);
    free_memory(token->tokenValue);
    free_memory(token);
}

char const *reserverd_words[] = {
    "principal",
    "funcao"
    "leitura",
    "escrita",
    "se",
    "senao",
    "para",
    "inteiro",
    "caractere",
    "decimal"
};

// Booleano check_principal(char *input){
//     return strcmp(substr(input, 0, 1), "p") && strcmp(input, reserverd_words[0]);
// }

void analise_lexica(File *file){
    int i, j;

    symbolList = (SymbolList*)allocate_memory(sizeof(SymbolList));
    symbolList->tokenCount = 0;
    list_new(&symbolList->tokens, sizeof(Token), free_token);

    for (i = 0; i < file->linesCount; i++)
    {
        Line *line = file_get_line(file, i);
        for (j = 1; j <= line->charCount; j++)
        {
            char *test = (char*)allocate_memory(sizeof(char) * (j + 2));
            memcpy(test, line->lineText, j);
            test[j + 1] = '\0';
            printf("%s\n", test);
            free_memory(test);
        }
    }
}