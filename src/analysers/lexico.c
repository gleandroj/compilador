#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include "lexico.h"
#include "../util/storage.h"
#include "../util/memory.h"
#include "../util/list.h"
#include "../util/helpers.h"
#include "../util/log.h"

#define BREAK_LINE 10

char const *reserverd_words[] = {
    "principal", //0
    "funcao",    //1
    "leitura",   //2
    "escrita",   //3
    "se",        //4
    "senao",     //5
    "para",      //6
    "inteiro",   //7
    "caractere", //8
    "decimal"    //9
};

File *_file = NULL;
Token *lastToken = NULL;
Token *scopeToken = NULL;
List chavesPilha;

int charIndex = 0;
int lineIndex = 0;

Booleano leuVirgula = FALSE;
TokenType possibleType = nao_identificado;
ReservedWordsIndex pwi = -1;

void free_pilha(void *data)
{
    free_memory(data);
}

Booleano printPilha(void *data)
{
    PilhaItem *item = (PilhaItem *)data;
    printf("%c ", (char)item->ascii);
    return TRUE;
}

void abriuChaves()
{
    PilhaItem *item = (PilhaItem *)allocate_memory(sizeof(PilhaItem));
    item->ascii = 123;
    item->charIndex = charIndex;
    list_append(&chavesPilha, item);
}

void fechouChaves()
{
    list_pop(&chavesPilha);
}

void verifyPossibleTokenType(char *c, ReservedWordsIndex *possibleWordIndex, TokenType *possibleType)
{
    if (((int)*c) == (int)reserverd_words[0][0])
    {
        *possibleWordIndex = principal_index;
        *possibleType = principal;
        return;
    }
    else if (((int)*c) == (int)reserverd_words[1][0])
    {
        *possibleWordIndex = funcao_index;
        *possibleType = funcao;
        return;
    }
    else if (((int)*c) == (int)reserverd_words[2][0])
    {
        *possibleWordIndex = leitura_index;
        *possibleType = funcao_reservada;
        return;
    }
    else if (((int)*c) == (int)reserverd_words[3][0])
    {
        *possibleWordIndex = escrita_index;
        *possibleType = funcao_reservada;
        return;
    }
    else if (((int)*c) == (int)reserverd_words[4][0])
    {
        *possibleWordIndex = se_index;
        *possibleType = palavra_reservada;
        return;
    }
    else if (((int)*c) == (int)reserverd_words[5][0])
    {
        *possibleWordIndex = senao_index;
        *possibleType = palavra_reservada;
        return;
    }
    else if (((int)*c) == (int)reserverd_words[6][0])
    {
        *possibleWordIndex = para_index;
        *possibleType = palavra_reservada;
        return;
    }
    else if (((int)*c) == (int)reserverd_words[7][0])
    {
        *possibleWordIndex = inteiro_index;
        *possibleType = variavel;
        return;
    }
    else if (((int)*c) == (int)reserverd_words[8][0])
    {
        *possibleWordIndex = caractere_index;
        *possibleType = variavel;
        return;
    }
    else if (((int)*c) == (int)reserverd_words[9][0])
    {
        *possibleWordIndex = decimal_index;
        *possibleType = variavel;
        return;
    }
}

char nextChar()
{
    assert(charIndex < _file->charactersCount);
    char c = file_get_char_at(_file, charIndex++);
    if ((int)c == 10)
        lineIndex++;
    if ((int)c == 0)
        return nextChar();
    return c;
}

char nextCharIgnoreSpaceAndBreakLine()
{
    char c = nextChar();
    if ((int)c == 32 || (int)c == 10)
        return nextCharIgnoreSpaceAndBreakLine();
    return c;
}

char nextCharIgnoreSpace()
{
    char c = nextChar();
    if ((int)c == 32)
        return nextCharIgnoreSpace();
    return c;
}

void setToken(Token *token,
              char *name,
              TokenType type,
              TokenDataType dataType,
              Token *parent,
              char *tokenValue,
              char *dataLenght,
              int lineIndex,
              int startTokenIndex)
{
    token->name = name;
    token->parent = parent;
    token->value = tokenValue;
    token->dataLenght = dataLenght;
    token->startCharIndex = startTokenIndex;
    token->lineIndex = lineIndex;
}

void imprimePilhaChaves()
{
    printf("Pilha de chaves:[");
    list_for_each(&chavesPilha, printPilha);
    printf("]\n");
}

char *validaVariavel(char *c, int *ascii)
{
    int varlen = 2;
    char buffer[MAX_VARIABLE_NAME_LEN];
    Booleano read = TRUE;
    if ((*ascii = (int)(*c = nextCharIgnoreSpace())) != 38)
    { //&
        log_error("Declaração de variável incorreta na linha: %d, caracter esperado: \'&\' .\n", lineIndex + 1);
    }
    buffer[0] = *c;
    *ascii = (int)(*c = nextChar());
    buffer[1] = *c;
    if (*ascii < 97 || *ascii > 122)
    { //a..z
        log_error("Declaração de variável incorreta na linha: %d, caracteres esperados: a..z .\n", lineIndex + 1);
    }
    //a..z A..Z 0..9
    while (read)
    {
        *ascii = (int)(*c = nextChar());
        if ((*ascii >= 97 && *ascii <= 122) || (*ascii >= 65 && *ascii <= 90) || (*ascii >= 48 && *ascii <= 57) || (*ascii == -1))
        {
            buffer[varlen] = *c;
            varlen++;
        }
        else
        {
            read = FALSE;
        }
    }

    char *varname = allocate_memory(sizeof(char) * varlen);
    memcpy(varname, buffer, varlen);
    varname[varlen] = '\0';
    return varname;
}

void validaPalavraReservada(char *c, int tokenLineIndex)
{
    int ci = 1, pwilen = strlen(reserverd_words[pwi]) - 1, ascii;
    //valida palavra reservada

    do
    {
        if ((int)(*c = nextChar()) != ((int)reserverd_words[pwi][ci]))
        {
            log_error("Caracter não identificado na linha: %d caracter: %c.\n", tokenLineIndex + 1, *c);
        }
        ci++;
    } while (ci <= pwilen);
}

Token *nextToken()
{
    if (_file == NULL)
    {
        return NULL;
    }

    Token *token = (Token *)allocate_memory(sizeof(Token));

    while ((charIndex + 1) < _file->charactersCount)
    {
        char c;
        int ascii;

        if (!leuVirgula)
        {
            ascii = (int)(c = nextCharIgnoreSpaceAndBreakLine());
            possibleType = nao_identificado;
            pwi = -1;
            verifyPossibleTokenType(&c, &pwi, &possibleType);
        }

        int pwilen = strlen(reserverd_words[pwi]) - 1, startTokenIndex = charIndex, tokenLineIndex = lineIndex;

        if (pwi != -1 && possibleType != nao_identificado)
        {
            //valida palavra reservada
            if (!leuVirgula)
                validaPalavraReservada(&c, tokenLineIndex);

            //Valida modulo/funcao principal
            if (possibleType == principal)
            {
                if ((int)(c = nextCharIgnoreSpaceAndBreakLine()) != 40 || ((int)(c = nextCharIgnoreSpace())) != 41)
                { //()
                    log_error("Erro na declaração do módulo/função princial na linha: %d caracter: %c.\n", tokenLineIndex + 1, c);
                }

                if ((int)(c = nextCharIgnoreSpaceAndBreakLine()) != 123)
                { //{
                    log_error("Erro na declaração do módulo/função princial na linha: %d caracter: %c.\n", tokenLineIndex + 1, c);
                }
                abriuChaves();

                setToken(token, (char *)reserverd_words[pwi], principal, vazio, NULL, NULL, NULL, tokenLineIndex, startTokenIndex);
                scopeToken = token;
                return token;
            }

            //valida declaração de variável
            if (possibleType == variavel)
            {
                if (!leuVirgula && (int)(c = nextChar()) != 32)
                { //space
                    log_error("Declaração de variável incorreta na linha: %d.\n", tokenLineIndex + 1, c);
                }

                char *varname;
                char *dataLenght = NULL;

                varname = validaVariavel(&c, &ascii);

                //printf("Variável encontrada: %s\n", varname);
                if (pwi == caractere_index || pwi == decimal_index)
                {
                    //Ignore space ex: caractere &teste [];
                    ascii = ascii == 10 ? (int)(c = nextCharIgnoreSpace()) : ascii;
                    if (ascii != 91) //[
                    {
                        log_error("Declaração de variável incorreta na linha: %d, caracter esperado: \'[\' .\n", tokenLineIndex + 1);
                    }
                    Booleano leuPonto = FALSE;
                    char buffer[MAX_VARIABLE_NAME_LEN];
                    int len = 0;

                    while ((ascii = (int)(c = nextChar())) && (ascii >= 48 && ascii <= 57) || (pwi == decimal_index && ascii == 46)){
                        if(pwi == caractere_index && (ascii < 48 || ascii > 57)){
                            log_error("Declaração de variável incorreta na linha: %d, caracter: \'%c\' .\n", tokenLineIndex + 1, c);
                        }else if(pwi == decimal_index && ((ascii < 48 || ascii > 57) || ascii != 46)){
                            log_error("Declaração de variável incorreta na linha: %d, caracter: \'%c\' .\n", tokenLineIndex + 1, c);
                        }
                        buffer[len] = c;
                        len++;
                    }

                    if(ascii != 93){
                        log_error("Declaração de variável incorreta na linha: %d, caracter esperado: \']\' .\n", tokenLineIndex + 1);
                    }

                    dataLenght = (char*)allocate_memory(sizeof(char) * len);
                    memcpy(dataLenght, buffer, len);
                    dataLenght[len] = '\0';

                    ascii = (int)(c = nextCharIgnoreSpace());
                }

                if (ascii == 44) //,
                {
                    leuVirgula = TRUE;
                    setToken(token, varname, variavel, pwi == inteiro_index ? inteiro : (pwi == caractere_index ? caractere : decimal), scopeToken, NULL, dataLenght, tokenLineIndex, startTokenIndex);
                    return token;
                }

                leuVirgula = FALSE;

                if (ascii == 59)
                { //; fim da declaração
                    setToken(token, varname, variavel, pwi == inteiro_index ? inteiro : (pwi == caractere_index ? caractere : decimal), scopeToken, NULL, dataLenght, tokenLineIndex, startTokenIndex);
                    return token;
                }
            }
        }
    }

    return NULL;
}

void lexical_analysis(File *file)
{
    _file = file;
    list_new(&chavesPilha, sizeof(PilhaItem), free_pilha);
    Token *token = NULL;
    while ((token = nextToken()) != NULL)
    {
        log_info("Novo token: %s\n", token->name);
    }
    list_destroy(&chavesPilha);
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