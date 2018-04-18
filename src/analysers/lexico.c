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

int charIndex,
    lineIndex,
    chavesCount,
    ascii;

char c;

TokenType possibleType = nao_identificado;
ReservedWordsIndex pwi = -1;

SymbolList *symbolList;

void free_all()
{
    list_destroy(&symbolList->tokens);
    free_memory(symbolList);
}

void free_token(void *pointer)
{
    Token *t = (Token *)pointer;
    free_memory(t->name);
    if (t->value != NULL)
        free_memory(t->value);
    if (t->dataLenght != NULL)
        free_memory(t->dataLenght);
    free_memory(t);
}

void log_abort(char *message, ...)
{
    va_list args;
    va_start(args, message);
    _log(message, LOG_ERROR, args);
    va_end(args);
    exit(0);
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

void verifyPossibleTokenType()
{
    pwi = -1;
    possibleType = nao_identificado;

    if (ascii == (int)reserverd_words[0][0])
    {
        pwi = principal_index;
        possibleType = principal;
        return;
    }
    else if (ascii == (int)reserverd_words[1][0])
    {
        pwi = funcao_index;
        possibleType = funcao;
        return;
    }
    else if (ascii == (int)reserverd_words[2][0])
    {
        pwi = leitura_index;
        possibleType = funcao_reservada;
        return;
    }
    else if (ascii == (int)reserverd_words[3][0])
    {
        pwi = escrita_index;
        possibleType = funcao_reservada;
        return;
    }
    else if (ascii == (int)reserverd_words[4][0])
    {
        pwi = se_index;
        possibleType = palavra_reservada;
        return;
    }
    else if (ascii == (int)reserverd_words[5][0])
    {
        pwi = senao_index;
        possibleType = palavra_reservada;
        return;
    }
    else if (ascii == (int)reserverd_words[6][0])
    {
        pwi = para_index;
        possibleType = palavra_reservada;
        return;
    }
    else if (ascii == (int)reserverd_words[7][0])
    {
        pwi = inteiro_index;
        possibleType = variavel;
        return;
    }
    else if (ascii == (int)reserverd_words[8][0])
    {
        pwi = caractere_index;
        possibleType = variavel;
        return;
    }
    else if (ascii == (int)reserverd_words[9][0])
    {
        pwi = decimal_index;
        possibleType = variavel;
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

Token *pushToken(char *name,
                 TokenType type,
                 TokenDataType dataType,
                 char *value,
                 char *dataLenght,
                 int lineIndex,
                 int startTokenIndex)
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

    token->startCharIndex = startTokenIndex;
    token->lineIndex = lineIndex;
    token->parent = scopeToken;

    list_append(&symbolList->tokens, token);
    symbolList->tokenCount++;

    const char *_ttype = type == argumento ? "(argumento)" : (type == variavel ? "(variavel)" : type == funcao ? "(funcao)" : type == principal ? "(princial)" : type == funcao_reservada ? "funcao_reservada" : "palavra_reservada");
    const char *_tdatatype = dataType == inteiro ? "inteiro" : (dataType == caractere ? "caractere" : "vazio");
    log_info("Novo token: %s %s %s %s\n", _ttype, _tdatatype, token->name, dataLenght != NULL ? dataLenght : "");

    return token;
}

void setScope(Token *scope)
{
    if (scope != NULL && scopeToken != NULL)
        log_abort("Erro na declaração de função, caracter esperado \'}\' na linha: %d.\n", lineIndex);
    scopeToken = scope;
}

char *validaVariavel()
{
    int varlen = 2;
    char buffer[MAX_VARIABLE_NAME_LEN];

    if ((int)nextCharIgnoreSpace() != 38) //&
        log_abort("Declaração de variável incorreta na linha: %d, caracter esperado: \'&\' .\n", lineIndex + 1);

    buffer[0] = c;
    if ((int)nextChar() < 97 || ascii > 122) //a..z
        log_abort("Declaração de variável incorreta na linha: %d, caracteres esperados: a..z .\n", lineIndex + 1);
    buffer[1] = c;
    //a..z A..Z 0..9
    while (((int)nextChar() >= 97 && ascii <= 122) || (ascii >= 65 && ascii <= 90) || (ascii >= 48 && ascii <= 57))
    {
        buffer[varlen] = c;
        varlen++;
    }
    buffer[varlen] = '\0';
    char *varname = (char *)allocate_memory(sizeof(char) * (varlen + 1));
    memcpy(varname, buffer, varlen + 1);
    return varname;
}

char *validaFuncao()
{
    int varlen = 1;
    char buffer[MAX_VARIABLE_NAME_LEN];

    if ((int)nextChar() != 102)
    { //f
        log_abort("Declaração/chamada de função incorreta na linha: %d, caracter esperado: \'f\' .\n", lineIndex + 1);
    }
    buffer[0] = c;
    //a..z A..Z 0..9
    while ((((int)nextChar() >= 97 && ascii <= 122) || (ascii >= 65 && ascii <= 90) || (ascii >= 48 && ascii <= 57)))
    {
        buffer[varlen] = c;
        varlen++;
    }
    buffer[varlen] = '\0';
    char *fname = (char *)allocate_memory(sizeof(char) * (varlen + 1));
    memcpy(fname, buffer, varlen + 1);
    return fname;
}

void validaPalavraReservada()
{
    int ci = 1, pwilen = strlen(reserverd_words[pwi]) - 1, ascii;
    //valida palavra reservada
    do
    {
        if ((int)nextChar() != ((int)reserverd_words[pwi][ci]))
            log_abort("Caracter não identificado na linha: %d caracter: %c.\n", lineIndex + 1, c);
        ci++;
    } while (ci <= pwilen);
}

char *validaDataLenght()
{
    if (pwi == caractere_index || pwi == decimal_index)
    {
        //TODO: Validar com a professora
        //Ignore space ex: caractere &teste [];
        ascii == 10 ? (int)nextCharIgnoreSpace() : ascii;
        if (ascii != 91) //[
            log_abort("Declaração de variável incorreta na linha: %d, caracter esperado: \'[\' .\n", lineIndex + 1);

        char buffer[MAX_VARIABLE_NAME_LEN];
        int len = 0;

        //[(0..9)(0..9)(.)?(0..9)*]
        while (((int)nextChar() >= 48 && ascii <= 57) || (pwi == decimal_index && ascii == 46))
        {
            if (pwi == caractere_index && (ascii < 48 || ascii > 57))
                log_abort("Declaração de variável incorreta na linha: %d, caracter: \'%c\' .\n", lineIndex + 1, c);
            else if (pwi == decimal_index && ((ascii < 48 || ascii > 57) || ascii != 46))
                log_abort("Declaração de variável incorreta na linha: %d, caracter: \'%c\' .\n", lineIndex + 1, c);

            buffer[len] = c;
            len++;
        }

        buffer[len] = '\0';

        if (ascii != 93) //]
            log_abort("Declaração de variável incorreta na linha: %d, caracter esperado: \']\' .\n", lineIndex + 1);

        char *dataLenght = (char *)allocate_memory(sizeof(char) * (len + 1));
        memcpy(dataLenght, buffer, len + 1);
        nextCharIgnoreSpace();
        return dataLenght;
    }

    return NULL;
}

char *validaExpressao()
{
    char buffer[MAX_VARIABLE_NAME_LEN];
    int len = 0;

    do
    {
        nextChar();
        if (ascii == 38) //&
        {
            charIndex--;
            char *varname = validaVariavel();
            int i, varlen = strlen(varname);
            for (i = 0; i < varlen; i++, len++)
                buffer[len] = varname[i];
        }
        else if (ascii >= 48 && ascii <= 57) //0..9
        {
            do
            {
                buffer[++len] = c;
            } while ((int)nextChar() >= 48 && ascii <= 57);
        }
        else if (ascii == 34) //"
        {
        }

        if (ascii == 42 || ascii == 43 || ascii == 45 || ascii == 47 || ascii == 94) // * +, -, /, ^
        {
            buffer[len++] = c;
        }
        else if (ascii == 40 || ascii == 41) //()
        {
            buffer[len++] = c;
        }

    } while (ascii != 10 && ascii != 59); //\n ;

    if (ascii != 59) //;
        log_abort("Finalização de expressão inválida, caracter esperado: \';\'");

    buffer[len] = '\0';
    char *expressao = (char *)allocate_memory(sizeof(char) * (len + 1));
    memcpy(expressao, buffer, len + 1);
    return expressao;
}

void readToSemicolonOrBreakLine() //; or \n
{
    do
    {
        //leia até ;
        nextChar();
    } while (ascii != 10 && ascii != 59); //\n, ;
}

void lexical_analysis(File *file)
{
    assert((_file = file) != NULL);
    charIndex = lineIndex = chavesCount = 0;

    symbolList = (SymbolList *)allocate_memory(sizeof(SymbolList));
    symbolList->tokenCount = 0;
    list_new(&symbolList->tokens, sizeof(Token), free_token);

    while ((charIndex + 1) < _file->charactersCount)
    {
        nextCharIgnoreSpaceAndBreakLine();
        verifyPossibleTokenType();

        int startTokenIndex = charIndex;

        if (possibleType != nao_identificado)
        {
            //valida palavra reservada
            validaPalavraReservada();

            //Valida modulo/funcao principal
            if (possibleType == principal)
            {
                if ((int)nextCharIgnoreSpaceAndBreakLine() != 40 || (int)nextCharIgnoreSpace() != 41) //()
                    log_abort("Erro na declaração do módulo/função princial na linha: %d caracter: %c.\n", lineIndex + 1, c);

                if ((int)nextCharIgnoreSpaceAndBreakLine() != 123) //{
                    log_abort("Erro na declaração do módulo/função princial na linha: %d caracter: %c.\n", lineIndex + 1, c);

                abriuChaves();

                setScope(pushToken((char *)reserverd_words[pwi], principal, vazio, NULL, NULL, lineIndex, startTokenIndex));
                continue;
            }
            else if (possibleType == funcao) //valida declaração/chamada de funcao
            {
                if ((int)nextChar() != 32) //space
                    log_abort("Declaração/chamada de função incorreta na linha: %d.\n", lineIndex + 1, c);

                char *funname = validaFuncao();

                //TODO: Validar com a professora
                //Ignore space ex: funcao fsoma ();
                ascii = ascii == 10 ? (int)(c = nextCharIgnoreSpace()) : ascii;
                if (ascii != 40) //(
                    log_abort("Declaração/chamada de função incorreta na linha: %d, caracter esperado \'(\'.\n", lineIndex + 1);

                if ((int)nextCharIgnoreSpace() != 41) //)
                {
                    verifyPossibleTokenType();
                    if (pwi == inteiro_index || pwi == caractere_index || pwi == decimal_index)
                    {
                        //Declaração de função com argumentos
                        setScope(pushToken(funname, funcao, vazio, NULL, NULL, lineIndex, startTokenIndex));
                        do
                        {
                            verifyPossibleTokenType();
                            validaPalavraReservada();

                            if ((int)nextChar() != 32) //space
                                log_abort("Declaração de argumento incorreto na linha: %d.\n", lineIndex + 1, c);

                            char *argname = validaVariavel();
                            char *dataLenght = validaDataLenght();

                            pushToken(argname, argumento, pwi == inteiro_index ? inteiro : (pwi == caractere_index ? caractere : decimal), NULL, dataLenght, lineIndex, startTokenIndex);
                        } while (ascii == 44 && (int)nextCharIgnoreSpace() != -1); //,

                        if (ascii != 41) //)
                            log_abort("Declaração de função incorreta na linha: %d, caracter esperado \')\'.\n", lineIndex + 1);

                        if ((int)nextCharIgnoreSpaceAndBreakLine() != 123) //{
                            log_abort("Declaração de função incorreta na linha: %d, caracter esperado \'{\'.\n", lineIndex + 1, c);

                        abriuChaves();
                    }
                    else
                    {
                        //Chamada de função com argumentos: OK
                        //TODO: Verificar se a função existe, se não morre o processo.
                        //Varrer o código até achar e voltar para esse ponto
                        printf("Verificar se afunção %s já foi declarada.\n", funname);
                        do
                        {
                            //Ignora validação da chamada da função, leia até ;
                            nextChar();
                        } while (ascii != 10 && ascii != 59); //\n, ;

                        if (ascii != 59)
                            log_abort("Finalização de chamada de função inválida, caracter esperado: \';\'.\n");
                    }
                }
                else if (ascii == 41 && (int)nextCharIgnoreSpace() == 59) //), ;
                {
                    //Chamada de função sem argumento: OK.
                    //TODO: Verificar se a função existe, se não morre o processo.
                    //Varrer o código até achar e voltar para esse ponto
                    printf("Verificar se afunção %s já foi declarada.\n", funname);
                }
                else if (ascii == 123) // {
                {
                    //Declaração de função sem argumento
                    setScope(pushToken(funname, funcao, vazio, NULL, NULL, lineIndex, startTokenIndex));
                    abriuChaves();
                }
                else
                    log_abort("Declaração/chamada de função incorreta na linha: %d.\n", lineIndex + 1, c);
            }
            else if (possibleType == variavel) //valida declaração de variável
            {
                if (scopeToken == NULL)
                    log_abort("Declaração de variável fora de escopo.\n");

                if ((int)nextChar() != 32) //space
                    log_abort("Declaração de variável incorreta na linha: %d.\n", lineIndex + 1);

                Booleano leuVirgula = FALSE;

                do
                {
                    ascii == 38 ? --charIndex : charIndex; //&
                    char *varname = validaVariavel();
                    char *dataLenght = validaDataLenght(&c, &ascii);
                    ascii == 32 ? nextCharIgnoreSpace() : ascii; // Ignore Space

                    if (ascii == 44) //,
                    {
                        leuVirgula = TRUE;
                        pushToken(varname, variavel, pwi == inteiro_index ? inteiro : (pwi == caractere_index ? caractere : decimal), NULL, dataLenght, lineIndex, startTokenIndex);
                        continue;
                    }
                    else if (ascii == 59) //; fim da declaração
                    {
                        leuVirgula = FALSE;
                        pushToken(varname, variavel, pwi == inteiro_index ? inteiro : (pwi == caractere_index ? caractere : decimal), NULL, dataLenght, lineIndex, startTokenIndex);
                        break;
                    }
                    else if (ascii == 61 && leuVirgula) //=
                    {
                        log_abort("Declaração de variável incorreta, não é permitido atribuição com multiplas variáveis, linha: %d.\n", lineIndex + 1);
                    }
                    else if (ascii == 61) //=
                    {
                        char *dataValue = validaExpressao();
                        printf("Declaração de variável com atribuição.\n");
                        //TODO Finalizar com ;
                    }
                    else
                    {
                        log_abort("Declaração de variável incorreta, linha: %d, caracter inesperado: %c.\n", lineIndex + 1, c);
                    }
                } while (ascii == 44 && (int)nextCharIgnoreSpace() == 38); //, &
            }
            else if (possibleType == funcao_reservada)
            {
                if ((int)nextChar() != 40) //(
                    log_abort("Chamada de função reservada incorreta, caracter esperado: \'(\' na linha: %d.\n", lineIndex + 1);

                readToSemicolonOrBreakLine();

                if (ascii != 59)
                    log_abort("Finalização de expressão inválida, caracter esperado: \';\', na linha: %d.\n", lineIndex + 1);
            }
        }
        else if (ascii == 125) //}
        {
            //if (scopeToken == NULL)
            //    log_abort("Erro na linha %d, caracter inesperado: \'}\'");
            setScope(NULL);
            fechouChaves();
        }
        else if (ascii == 38) //&
        {
            charIndex--;
            char *varname = validaVariavel();
            printf("Valida utilização de variável: %s.\n", varname);

            if (ascii == 61) //=
            {
                char *dataValue = validaExpressao();
                printf("Expressão: %s.\n", dataValue);
            }

            //free_memory(varname);
            //exit(1);
        }
    }
    //free_all();
    log_info("Finalizado analise lexica\n");
}