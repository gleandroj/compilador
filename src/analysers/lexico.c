#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "lexico.h"
#include "../util/storage.h"
#include "../util/memory.h"
#include "../util/helpers.h"
#include "../util/log.h"

#define BREAK_LINE '\n'

char const *reserverd_words[] = {
    "principal",
    "funcao",
    "leitura",
    "escrita",
    "se",
    "senao",
    "para",
    "inteiro",
    "caractere",
    "decimal"
};

File *_file = NULL;

void verifyPossibleTokenType(char *c, char *possibleWord, TokenType *possibleType)
{
    if (isspace(*c) || ((int)*c) == BREAK_LINE)
    {
        //printf("Ignored char %c\n", isspace(*c) ? '_' : (*c == BREAK_LINE ? '^' : *c));
        c++;
        return verifyPossibleTokenType(c, possibleWord, possibleType);
    }
    else if (*c == reserverd_words[0][0])
    {
        possibleWord = (char *)&reserverd_words[0][0];
        *possibleType = principal;
        return;
    }
    else if (*c == reserverd_words[1][0])
    {
        possibleWord = (char *)&reserverd_words[1][0];
        *possibleType = funcao;
        return;
    }
    else if (*c == reserverd_words[2][0])
    {
        possibleWord = (char *)&reserverd_words[2][0];
        *possibleType = funcao_reservada;
        return;
    }
    else if (*c == reserverd_words[3][0])
    {
        possibleWord = (char *)&reserverd_words[3][0];
        *possibleType = funcao_reservada;
        return;
    }
    else if (*c == reserverd_words[4][0])
    {
        possibleWord = (char *)&reserverd_words[4][0];
        *possibleType = palavra_reservada;
        return;
    }
    else if (*c == reserverd_words[5][0])
    {
        possibleWord = (char *)&reserverd_words[5][0];
        *possibleType = palavra_reservada;
        return;
    }
    else if (*c == reserverd_words[6][0])
    {
        possibleWord = (char *)&reserverd_words[6][0];
        *possibleType = palavra_reservada;
        return;
    }
    else if (*c == reserverd_words[7][0])
    {
        possibleWord = (char *)&reserverd_words[7][0];
        *possibleType = variavel | argumento;
        return;
    }
    else if (*c == reserverd_words[8][0])
    {
        possibleWord = (char *)&reserverd_words[8][0];
        *possibleType = variavel | argumento;
        return;
    }
    else if (*c == reserverd_words[9][0])
    {
        possibleWord = (char *)&reserverd_words[9][0];
        *possibleType = variavel | argumento;
        return;
    }
}

Token *nextToken()
{
    if (_file == NULL)
    {
        return NULL;
    }

    int lineIndex = 0, charPosition = 0;
    Line *line;
    char *c, *possibleWord = NULL;
    TokenType possibleType = nao_identificado;
    while ((line = file_get_line(_file, lineIndex)) != NULL)
    {
        c = line->lineText;
        verifyPossibleTokenType(c, possibleWord, &possibleType);


        lineIndex++;
        possibleType = nao_identificado;
    }

    return NULL;
}

void lexical_analysis(File *file)
{
    _file = file;
    Token *token = NULL;
    while ((token = nextToken()) != NULL)
    {
    }

    log_info("Finalizado analise lexica\n");
}

// void lexical_analysis(File *file)
// {
//     int lineIndex = 0, charPosition = 0;
//     char *c, *possible_word = NULL;
//     TokenType possible_token_type;
//     for (lineIndex = 0; lineIndex < file->linesCount; lineIndex++)
//     {
//         Line *line = file_get_line(file, lineIndex);
//         for (c = line->lineText; *c; c++, charPosition++)
//         {
//             if (possible_word != NULL)
//             {
//                 int j, k, read_len = 2, len = strlen(possible_word);
//                 for (j = 1; j < len; j++, c++, charPosition++, read_len++)
//                 {

//                     printf("Read len %d, Possible word len %d, type %d, char: %c, \n", read_len, len, possible_token_type, isspace(*c) ? 's' : *c);

//                     if ((possible_token_type == funcao ||
//                          possible_token_type == principal ||
//                          possible_token_type == palavra_reservada) &&
//                         (*c == '(' || *c == ')'))
//                     {
//                         continue;
//                     }
//                     else if ((possible_token_type == funcao ||
//                          possible_token_type == principal ||
//                          possible_token_type == palavra_reservada) &&
//                         (*c == '{' || *c == '}'))
//                     {
//                         printf("{} fim \n");
//                         break;
//                     }
//                     else if ((possible_token_type == variavel ||
//                               possible_token_type == funcao_reservada) &&
//                              *c == ';')
//                     {
//                         printf("; fim \n");
//                         break;
//                     }

//                     if ((possible_token_type == variavel | argumento) && (char)possible_word[0] == 'c' && (char)possible_word[0] == 'd' && (*c == '['))
//                     {
//                         printf("[ fim \n");
//                         Booleano done = FALSE;
//                         do
//                         {
//                             c++;
//                             if (!isalnum(*c))
//                             {
//                                 log_error("Unexpected char at line %d char %c\n", lineIndex + 1, *c);
//                             }
//                             if (*c == ']')
//                             {
//                                 done = TRUE;
//                             }
//                         } while (!done);
//                     }

//                     if (len == read_len && isspace(*c) && (possible_word[0] != 'c' || possible_word[0]) != 'd')
//                     {
//                         printf("espaço fim \n");
//                         continue;
//                     }

//                     // || *c == '}' || *c == ';'

//                     if (*c != (char)possible_word[j])
//                     {
//                         log_error("Unexpected char at line %d char %c\n", lineIndex + 1, *c);
//                     }
//                 }
//                 printf("Nenhuma condição\n");
//                 possible_word = NULL;
//                 printf("Reiniciando..\n");
//             }

//             if (isspace(*c) || ((int)*c) == BREAK_LINE)
//             {
//                 printf("Ignored char %c\n", isspace(*c) ? '_' : (*c == BREAK_LINE ? '^' : *c));
//                 continue;
//             }
//             else if (*c == reserverd_words[0][0])
//             {
//                 possible_word = (char *)&reserverd_words[0][0];
//                 possible_token_type = principal;
//             }
//             else if (*c == reserverd_words[1][0])
//             {
//                 possible_word = (char *)&reserverd_words[1][0];
//                 possible_token_type = funcao;
//             }
//             else if (*c == reserverd_words[2][0])
//             {
//                 possible_word = (char *)&reserverd_words[2][0];
//                 possible_token_type = funcao_reservada;
//             }
//             else if (*c == reserverd_words[3][0])
//             {
//                 possible_word = (char *)&reserverd_words[3][0];
//                 possible_token_type = funcao_reservada;
//             }
//             else if (*c == reserverd_words[4][0])
//             {
//                 possible_word = (char *)&reserverd_words[4][0];
//                 possible_token_type = palavra_reservada;
//             }
//             else if (*c == reserverd_words[5][0])
//             {
//                 possible_word = (char *)&reserverd_words[5][0];
//                 possible_token_type = palavra_reservada;
//             }
//             else if (*c == reserverd_words[6][0])
//             {
//                 possible_word = (char *)&reserverd_words[6][0];
//                 possible_token_type = palavra_reservada;
//             }
//             else if (*c == reserverd_words[7][0])
//             {
//                 possible_word = (char *)&reserverd_words[7][0];
//                 possible_token_type = variavel | argumento;
//             }
//             else if (*c == reserverd_words[8][0])
//             {
//                 possible_word = (char *)&reserverd_words[8][0];
//                 possible_token_type = variavel | argumento;
//             }
//             else if (*c == reserverd_words[9][0])
//             {
//                 possible_word = (char *)&reserverd_words[9][0];
//                 possible_token_type = variavel | argumento;
//             }

//             //printf("Possible word: %s\n", possible_word);
//         }
//     }
// }

// void analise_lexica(File *file){
//     int i, j;

//     symbolList = (SymbolList*)allocate_memory(sizeof(SymbolList));
//     symbolList->tokenCount = 0;
//     list_new(&symbolList->tokens, sizeof(Token), free_token);

//     for (i = 0; i < file->linesCount; i++)
//     {
//         Line *line = file_get_line(file, i);
//         for (j = 1; j <= line->charCount; j++)
//         {
//             char *test = (char*)allocate_memory(sizeof(char) * (j + 2));
//             memcpy(test, line->lineText, j);
//             test[j + 1] = '\0';
//             printf("%s\n", test);
//             free_memory(test);
//         }
//     }
// }