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
Token *lastToken = NULL,
      *scopeToken = NULL;
//List chavesPilha;

int charIndex = 0,
    lineIndex = 0,
    chavesCount = 0;

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
    // PilhaItem *item = (PilhaItem *)allocate_memory(sizeof(PilhaItem));
    // item->ascii = 123;
    // item->charIndex = charIndex;
    // list_append(&chavesPilha, item);
    chavesCount++;
}

void fechouChaves()
{
    chavesCount--;
}

void verifyPossibleTokenType(char *c, ReservedWordsIndex *possibleWordIndex, TokenType *possibleType)
{
    *possibleWordIndex = -1;
    *possibleType = nao_identificado;

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
    // printf("Pilha de chaves:[");
    // list_for_each(&chavesPilha, printPilha);
    // printf("]\n");
}

char *validaVariavel(char *c, int *ascii)
{
    int varlen = 2;
    char buffer[MAX_VARIABLE_NAME_LEN];

    if ((*ascii = (int)(*c = nextCharIgnoreSpace())) != 38)
    { //&
        log_error("Declaração de variável incorreta na linha: %d, caracter esperado: \'&\' .\n", lineIndex + 1);
    }
    buffer[0] = *c;
    *ascii = (int)(buffer[1] = (*c = nextChar()));
    if (*ascii < 97 || *ascii > 122)
    { //a..z
        log_error("Declaração de variável incorreta na linha: %d, caracteres esperados: a..z .\n", lineIndex + 1);
    }
    //a..z A..Z 0..9
    while ((*ascii = (int)(*c = nextChar())) && ((*ascii >= 97 && *ascii <= 122) || (*ascii >= 65 && *ascii <= 90) || (*ascii >= 48 && *ascii <= 57)))
    {
        buffer[varlen] = *c;
        varlen++;
    }

    char *varname = allocate_memory(sizeof(char) * varlen);
    memcpy(varname, buffer, varlen);
    varname[varlen] = '\0';
    return varname;
}

char *validaFuncao(char *c, int *ascii)
{
    int varlen = 1;
    char buffer[MAX_VARIABLE_NAME_LEN];

    if ((*ascii = (int)(*c = nextChar())) != 102)
    { //f
        log_error("Declaração/chamada de função incorreta na linha: %d, caracter esperado: \'f\' .\n", lineIndex + 1);
    }
    buffer[0] = *c;
    //a..z A..Z 0..9
    while ((*ascii = (int)(*c = nextChar())) && ((*ascii >= 97 && *ascii <= 122) || (*ascii >= 65 && *ascii <= 90) || (*ascii >= 48 && *ascii <= 57)))
    {
        buffer[varlen] = *c;
        varlen++;
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

char *validaDataLenght(char *c, int *ascii)
{
    if (pwi == caractere_index || pwi == decimal_index)
    {
        //TODO: Validar com a professora
        //Ignore space ex: caractere &teste [];
        *ascii = *ascii == 10 ? (int)(*c = nextCharIgnoreSpace()) : *ascii;
        if (*ascii != 91) //[
            log_error("Declaração de variável incorreta na linha: %d, caracter esperado: \'[\' .\n", lineIndex + 1);

        char buffer[MAX_VARIABLE_NAME_LEN];
        int len = 0;

        //[(0..9)(0..9)*(.)?(0..9)*]
        while ((*ascii = (int)(*c = nextChar())) && (*ascii >= 48 && *ascii <= 57) || (pwi == decimal_index && *ascii == 46))
        {
            if (pwi == caractere_index && (*ascii < 48 || *ascii > 57))
                log_error("Declaração de variável incorreta na linha: %d, caracter: \'%c\' .\n", lineIndex + 1, *c);
            else if (pwi == decimal_index && ((*ascii < 48 || *ascii > 57) || *ascii != 46))
                log_error("Declaração de variável incorreta na linha: %d, caracter: \'%c\' .\n", lineIndex + 1, *c);

            buffer[len] = *c;
            len++;
        }

        if (*ascii != 93) //]
            log_error("Declaração de variável incorreta na linha: %d, caracter esperado: \']\' .\n", lineIndex + 1);

        char *dataLenght = (char *)allocate_memory(sizeof(char) * len);
        memcpy(dataLenght, buffer, len);
        dataLenght[len] = '\0';
        *ascii = (int)(*c = nextCharIgnoreSpace());
        return dataLenght;
    }

    return NULL;
}

Token *nextToken()
{
    if (_file == NULL)
        return NULL;
    Token *token = (Token *)allocate_memory(sizeof(Token));
    while ((charIndex + 1) < _file->charactersCount)
    {
        char c;
        int ascii;

        if (!leuVirgula)
        {
            ascii = (int)(c = nextCharIgnoreSpaceAndBreakLine());
            verifyPossibleTokenType(&c, &pwi, &possibleType);
        }

        int startTokenIndex = charIndex, tokenLineIndex = lineIndex;

        if (possibleType != nao_identificado)
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
            else if (possibleType == funcao) //valida declaração/chamada de funcao
            {
                if ((ascii = (int)(c = nextChar())) != 32)
                { //space
                    log_error("Declaração/chamada de função incorreta na linha: %d.\n", lineIndex + 1, c);
                }
                char *funname;
                funname = validaFuncao(&c, &ascii);
                //TODO: Validar com a professora
                //Ignore space ex: funcao fsoma ();
                ascii = ascii == 10 ? (int)(c = nextCharIgnoreSpace()) : ascii;
                if (ascii != 40) //(
                    log_error("Declaração/chamada de função incorreta na linha: %d, caracter esperado \'(\'.\n", tokenLineIndex + 1);

                if ((ascii = (int)(c = nextCharIgnoreSpace())) != 41) //)
                {
                    if (ascii == 38) //&, chamada de função com argumentos
                    {//TODO: Verificar se já foi declarada, se não foi varrer o código até achar e voltar para esse ponto
                        charIndex--;
                        do
                        {
                            char *varname = validaVariavel(&c, &ascii);
                            char *dataLenght = validaDataLenght(&c, &ascii);
                            //Variáveis da chamada da função
                            //TODO: verificar se foram declaras
                        } while (ascii == 44); //,

                        ascii = ascii == 10 ? (int)(c = nextCharIgnoreSpace()) : ascii;

                        if (ascii != 41) //)
                            log_error("Chamada de função incorreta na linha: %d, caracter esperado \')\'.\n", tokenLineIndex + 1);
                        if ((ascii = (int)(c = nextChar())) != 59) //;
                            log_error("Chamada de função incorreta na linha: %d, caracter esperado \';\'.\n", tokenLineIndex + 1);
                    }
                    else
                    {//Declaração de função com argumentos
                        do
                        {
                            verifyPossibleTokenType(&c, &pwi, &possibleType);
                            validaPalavraReservada(&c, tokenLineIndex);

                            if (ascii = (int)(c = nextChar()) != 32) //space
                                log_error("Declaração de argumento incorreto na linha: %d.\n", lineIndex + 1, c);

                            char *argname = validaVariavel(&c, &ascii);
                            char *dataLenght = validaDataLenght(&c, &ascii);
                            
                            printf("Arg: %s\n", argname);

                        } while (ascii == 44 && (ascii = (int)(c = nextCharIgnoreSpace()))); //,
                    }
                }
            }
            else if (possibleType == variavel) //valida declaração de variável
            {
                if (!leuVirgula && (ascii = (int)(c = nextChar())) != 32) //space
                    log_error("Declaração de variável incorreta na linha: %d.\n", lineIndex + 1, c);

                char *varname = validaVariavel(&c, &ascii);
                char *dataLenght = validaDataLenght(&c, &ascii);

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
        else if (ascii == 125) //}
        {
            scopeToken = NULL;
            fechouChaves();
        }
    }

    return NULL;
}

void lexical_analysis(File *file)
{
    _file = file;
    Token *token = NULL;
    while ((token = nextToken()) != NULL)
    {
        log_info("Novo token: %s\n", token->name);
    }
    log_info("Finalizado analise lexica\n");
}