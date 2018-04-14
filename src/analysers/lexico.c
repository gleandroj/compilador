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

int charIndex = 0,
    lineIndex = 0,
    chavesCount = 0,
    ascii;

char c;

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
    char _c = file_get_char_at(_file, charIndex++);
    if ((int)_c == 10)
        lineIndex++;
    if ((int)_c == 0)
        return nextChar();
    c = _c;
    ascii = (int)_c;
    return _c;
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

Token *push_token(char *name,
                  TokenType type,
                  TokenDataType dataType,
                  char *tokenValue,
                  char *dataLenght,
                  int lineIndex,
                  int startTokenIndex)
{
    Token *token = allocate_memory(sizeof(Token));
    token->name = name;
    token->parent = scopeToken;
    token->value = tokenValue;
    token->dataLenght = dataLenght;
    token->startCharIndex = startTokenIndex;
    token->lineIndex = lineIndex;
    const char* _ttype = type == argumento ? "(argumento)" : (type == variavel ? "(variavel)" : type == funcao ? "(funcao)" : type == principal ? "(princial)" : type == funcao_reservada ? "funcao_reservada" : "palavra_reservada");
    const char* _tdatatype = dataType == inteiro ? "inteiro" : (dataType == caractere ? "caractere" : "vazio");
    log_info("Novo token: %s %s %s %s\n", _ttype,  _tdatatype ,token->name, dataLenght != NULL ? dataLenght : "");
    return token;
}

void set_scope(Token *scope)
{
    if (scopeToken != NULL)
        log_error("Erro na declaração de função, caracter esperado \'}\' na linha: %d.\n", lineIndex);
    scopeToken = scope;
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

char *validaExpressao(char *c, int *ascii)
{
    //TODO: Validar expressão
    return NULL;
}

void lexical_analysis(File *file)
{
    assert((_file = file) != NULL);
    while ((charIndex + 1) < _file->charactersCount)
    {
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
                if ((int)nextCharIgnoreSpaceAndBreakLine() != 40 || (int)nextCharIgnoreSpace() != 41) //()
                    log_error("Erro na declaração do módulo/função princial na linha: %d caracter: %c.\n", tokenLineIndex + 1, c);

                if ((int)nextCharIgnoreSpaceAndBreakLine() != 123) //{
                    log_error("Erro na declaração do módulo/função princial na linha: %d caracter: %c.\n", tokenLineIndex + 1, c);

                abriuChaves();

                set_scope(push_token((char *)reserverd_words[pwi], principal, vazio, NULL, NULL, tokenLineIndex, startTokenIndex));
            }
            else if (possibleType == funcao) //valida declaração/chamada de funcao
            {
                if ((int)nextChar() != 32) //space
                    log_error("Declaração/chamada de função incorreta na linha: %d.\n", lineIndex + 1, c);

                char *funname = validaFuncao(&c, &ascii);

                //TODO: Validar com a professora
                //Ignore space ex: funcao fsoma ();
                ascii = ascii == 10 ? (int)(c = nextCharIgnoreSpace()) : ascii;
                if (ascii != 40) //(
                    log_error("Declaração/chamada de função incorreta na linha: %d, caracter esperado \'(\'.\n", tokenLineIndex + 1);

                if ((int)nextCharIgnoreSpace() != 41) //)
                {
                    if (ascii == 38) //&
                    {
                        //Chamada de função sem argumento
                        //TODO: Verificar se a função existe, se não morre o processo.
                        //Varrer o código até achar e voltar para esse ponto
                        charIndex--;
                        do
                        {
                            char *varname = validaVariavel(&c, &ascii);
                            char *dataLenght = validaDataLenght(&c, &ascii);
                            //Variáveis da chamada da função
                            //TODO: verificar se foram declaras, se não morre o processo
                        } while (ascii == 44); //,

                        ascii == 10 ? (int)nextCharIgnoreSpace() : ascii;

                        if (ascii != 41) //)
                            log_error("Chamada de função incorreta na linha: %d, caracter esperado \')\'.\n", tokenLineIndex + 1);
                        if ((int)nextCharIgnoreSpace() != 59) //;
                            log_error("Chamada de função incorreta na linha: %d, caracter esperado \';\'.\n", tokenLineIndex + 1);
                    }
                    else
                    {
                        //Declaração de função com argumentos
                        set_scope(push_token(funname, funcao, vazio, NULL, NULL, tokenLineIndex, startTokenIndex));
                        do
                        {
                            verifyPossibleTokenType(&c, &pwi, &possibleType);
                            validaPalavraReservada(&c, tokenLineIndex);

                            if ((int)nextChar() != 32) //space
                                log_error("Declaração de argumento incorreto na linha: %d.\n", lineIndex + 1, c);

                            char *argname = validaVariavel(&c, &ascii);
                            char *dataLenght = validaDataLenght(&c, &ascii);

                            push_token(argname, argumento, pwi == inteiro_index ? inteiro : (pwi == caractere_index ? caractere : decimal), NULL, dataLenght, tokenLineIndex, startTokenIndex);
                        } while (ascii == 44 && (int)nextCharIgnoreSpace() != -1); //,

                        if (ascii != 41) //)
                            log_error("Declaração de função incorreta na linha: %d, caracter esperado \')\'.\n", tokenLineIndex + 1);

                        if ((int)nextCharIgnoreSpaceAndBreakLine() != 123) //{
                            log_error("Declaração de função incorreta na linha: %d, caracter esperado \'{\'.\n", tokenLineIndex + 1, c);

                        abriuChaves();
                    }
                }
                else if (ascii == 41 && (int)nextCharIgnoreSpace() == 59) //;
                {
                    //Chamada de função sem argumento
                    //TODO: Verificar se a função existe, se não morre o processo.
                    //Varrer o código até achar e voltar para esse ponto
                }
                else if (ascii == 123) // {
                {
                    //Declaração de função sem argumento
                    set_scope(push_token(funname, funcao, vazio, NULL, NULL, tokenLineIndex, startTokenIndex));
                    abriuChaves();
                }
                else
                {
                    log_error("Falha.");
                }
            }
            else if (possibleType == variavel) //valida declaração de variável
            {
                if (scopeToken == NULL)
                    log_error("Declaração de variável fora de escopo.\n");

                if (!leuVirgula && (ascii = (int)(c = nextChar())) != 32) //space
                    log_error("Declaração de variável incorreta na linha: %d.\n", tokenLineIndex, c);

                char *varname = validaVariavel(&c, &ascii);
                char *dataLenght = validaDataLenght(&c, &ascii);

                if (ascii == 44) //,
                {
                    leuVirgula = TRUE;
                    push_token(varname, variavel, pwi == inteiro_index ? inteiro : (pwi == caractere_index ? caractere : decimal), NULL, dataLenght, tokenLineIndex, startTokenIndex);
                }
                else if (ascii == 59) //; fim da declaração
                {
                    leuVirgula = FALSE;
                    push_token(varname, variavel, pwi == inteiro_index ? inteiro : (pwi == caractere_index ? caractere : decimal), NULL, dataLenght, tokenLineIndex, startTokenIndex);
                }
                else if (ascii == 61 && leuVirgula)
                {
                    log_error("Declaração de variável incorreta, não é permitido atribuição com multiplas variáveis, linha: %d", tokenLineIndex);
                }
                else if (ascii == 61) //=
                {
                    char *dataValue = validaExpressao(&c, &ascii);
                    //TODO Finalizar com ;
                }
            }
        }
        else if (ascii == 125) //}
        {
            //if (scopeToken == NULL)
            //    log_error("Erro na linha %d, caracter inesperado: \'}\'");
            scopeToken = NULL;
            fechouChaves();
        }
        else if (ascii == 38) //&
        {
            charIndex--;
            char *varname = validaVariavel(&c, &ascii);
            printf("Valida utilização de variável: %s.\n", varname);
            //exit(1);
        }
    }
    log_info("Finalizado analise lexica\n");
}