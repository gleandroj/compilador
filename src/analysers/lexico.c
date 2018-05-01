#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#include "lexico.h"
#include "symbol.h"
#include "../util/storage.h"
#include "../util/memory.h"
#include "../util/list.h"
#include "../util/helpers.h"
#include "../util/log.h"

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
Token *scopeToken = NULL;
SymbolList *symbolList = NULL;
List *funStack = NULL;

int charIndex,
    lineIndex,
    ascii,
    startTokenIndex;

char c;

TokenType possibleType = nao_identificado;
ReservedWordsIndex pwi = -1;

void freeFunStackData(void *data)
{
    FunStackData *fs = (FunStackData *)data;
    free_memory(fs->funname);
    free_memory(fs);
}

void newFunStack()
{
    funStack = (List *)allocate_memory(sizeof(List));
    list_new(funStack, sizeof(FunStackData), freeFunStackData);
}

void log_abort(char *message, ...)
{
    va_list args;
    va_start(args, message);
    _log(message, LOG_ERROR, args);
    va_end(args);
    exit(0);
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

void verifyPossibleTokenType()
{
    pwi = -1;
    possibleType = nao_identificado;

    if (ascii == (int)reserverd_words[0][0])
    {
        int _ascii = ascii;
        char _c = c;
        if ((int)nextChar() == (int)reserverd_words[0][1])
        {
            pwi = principal_index;
            possibleType = principal;
        }
        else if (ascii == (int)reserverd_words[6][1])
        {
            pwi = para_index;
            possibleType = palavra_reservada;
        }
        ascii = _ascii;
        c = _c;
        charIndex--;
    }
    else if (ascii == (int)reserverd_words[1][0])
    {
        pwi = funcao_index;
        possibleType = funcao;
    }
    else if (ascii == (int)reserverd_words[2][0])
    {
        pwi = leitura_index;
        possibleType = funcao_reservada;
    }
    else if (ascii == (int)reserverd_words[3][0])
    {
        pwi = escrita_index;
        possibleType = funcao_reservada;
    }
    else if (ascii == (int)reserverd_words[4][0])
    {
        int _ascii = ascii;
        char _c = c;
        if ((int)nextChar() == (int)reserverd_words[5][1] && (int)nextChar() == (int)reserverd_words[5][2])
        {
            pwi = senao_index;
            possibleType = palavra_reservada;
        }
        else
        {
            pwi = se_index;
            possibleType = palavra_reservada;
        }
        ascii = _ascii;
        c = _c;
        charIndex -= 2;
    }
    else if (ascii == (int)reserverd_words[7][0])
    {
        pwi = inteiro_index;
        possibleType = variavel;
    }
    else if (ascii == (int)reserverd_words[8][0])
    {
        pwi = caractere_index;
        possibleType = variavel;
    }
    else if (ascii == (int)reserverd_words[9][0])
    {
        pwi = decimal_index;
        possibleType = variavel;
    }
}

void setScope(Token *scope)
{
    if (scope != NULL && scopeToken != NULL)
        log_abort("Erro na declaração de função, caracter esperado \'}\' na linha: %d.\n", lineIndex);
    scopeToken = scope;
}

char *checkVariableName()
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

char *checkFunctionName()
{
    int len = 1;
    char buffer[MAX_VARIABLE_NAME_LEN];

    if ((int)nextChar() != 102)
    { //f
        log_abort("Declaração/chamada de função incorreta na linha: %d, caracter esperado: \'f\' .\n", lineIndex + 1);
    }
    buffer[0] = c;
    //a..z A..Z 0..9
    while ((((int)nextChar() >= 97 && ascii <= 122) || (ascii >= 65 && ascii <= 90) || (ascii >= 48 && ascii <= 57)))
    {
        buffer[len] = c;
        len++;
    }
    buffer[len] = '\0';
    char *fname = (char *)allocate_memory(sizeof(char) * (len + 1));
    memcpy(fname, buffer, len + 1);
    return fname;
}

Booleano checkReservedWord(Booleano disableFail)
{
    int ci = 1, pwilen = strlen(reserverd_words[pwi]) - 1, ascii;
    //valida palavra reservada
    do
    {
        if ((int)nextChar() != ((int)reserverd_words[pwi][ci]))
        {
            if (!disableFail)
                log_abort("Caracter inesperado na linha: %d, caracter: %c.\n", lineIndex + 1, c);
            return FALSE;
        }
        ci++;
    } while (ci <= pwilen);

    return TRUE;
}

char *checkDataLenght()
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
            else if (pwi == decimal_index && (ascii < 48 || ascii > 57) && ascii != 46)
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

char *checkExpression()
{
    char buffer[MAX_VARIABLE_NAME_LEN];
    Booleano lo = FALSE, fi = FALSE;
    int len = 0, la, varlen;

    do
    {
        nextChar();
        if (ascii == 38) //&
        {
            charIndex--;
            char *varname = checkVariableName();
            int i;
            varlen = strlen(varname);
            for (i = 0; i < varlen; i++, len++)
                buffer[len] = varname[i];
            lo = FALSE;
            free_memory(varname);
            if (ascii == 42 || ascii == 43 || ascii == 45 || ascii == 47 || ascii == 94) // * +, -, /, ^
            {
                charIndex--;
                continue;
            }
        }
        else if (ascii >= 48 && ascii <= 57) //0..9
        {
            do
            {
                buffer[len++] = c;
            } while ((int)nextChar() >= 48 && ascii <= 57);
            lo = FALSE;
            varlen = 0;
        }
        else if (ascii == 34) //"
        {
            do
            {
                buffer[len++] = c;
            } while ((int)nextChar() != 34 && ascii != 10 && ascii != 59); // " \n ;

            if (c == 34)
                buffer[len++] = c;
            else
                log_abort("Formação de string incorreta na linha: %d", lineIndex + 1);
            lo = FALSE;
            varlen = 0;
        }
        else if ((!lo || (la == 43 && ascii == 43) || (la == 45 && ascii == 45)) && (ascii == 42 || ascii == 43 || ascii == 45 || ascii == 47 || ascii == 94)) // * +, -, /, ^
        {
            buffer[len++] = c;
            Booleano isSameOp = (la == 43 && ascii == 43) || (la == 45 && ascii == 45);
            if (isSameOp && ((int)buffer[len - 2] != ascii || len == 2))
            {
                if (len > 2 && ((varlen + 1) != len))
                    log_abort("Expressão incorreta, linha: %d, caracter inesperado: %c.\n", lineIndex + 1, c);
                else if (len == 2)
                {
                    char *varname = checkVariableName();
                    int i, varlen = strlen(varname);
                    for (i = 0; i < varlen; i++, len++)
                        buffer[len] = varname[i];
                    free_memory(varname);
                    charIndex--;
                }
                nextChar();
                fi = TRUE;
                break;
            }else if(isSameOp && (int)buffer[len - 2] == ascii){
                nextChar();
                fi = TRUE;
                break;
            }
            lo = TRUE;
        }
        else if (ascii == 40 || ascii == 41) //()
            buffer[len++] = c;
        else if (ascii != 32 && ascii != 10 && ascii != 59) //space, \n, ;
            log_abort("Expressão incorreta, linha: %d, caracter inesperado: %c.\n", lineIndex + 1, c);

        la = ascii;
    } while (ascii != 10 && ascii != 59); //\n ;

    int lb = (int)buffer[len - 1];
    if (!fi && (lb == 42 || lb == 43 || lb == 45 || lb == 47 || lb == 94))
        log_abort("Expressão incorreta, linha: %d, caracter inesperado: %c.\n", lineIndex + 1, buffer[len - 1]);
    if (ascii != 59) //;
        log_abort("Finalização de expressão inválida, caracter esperado: \';\', na linha: %d.\n", lineIndex + 1);

    buffer[len] = '\0';
    char *expressao = (char *)allocate_memory(sizeof(char) * (len + 1));
    memcpy(expressao, buffer, len + 1);
    return expressao;
}

void readTo(int chars[], int len)
{
    Booleano done = FALSE;
    do
    {
        //leia até
        nextChar();
        int i;
        for (i = 0; i < len; i++)
            done = done || chars[i] == ascii;
    } while (!done);
}

FunStackData *findFunctionStatement(char *funname)
{
    int backtrackIndex = charIndex,
        backtrackLineIndex = lineIndex;
    Token *scope = scopeToken;
    do
    {
        verifyPossibleTokenType();
        if (possibleType == funcao && checkReservedWord(TRUE))
        {
            if ((int)nextChar() != 32) //space
                log_abort("Declaração/chamada de função incorreta na linha: %d.\n", lineIndex + 1, c);

            char *checkfun = checkFunctionName();

            if (strcmp(funname, checkfun) == 0 && ascii == 40) //(
            {
                int readLen = strlen("funcao ") + strlen(funname) + 1;
                do
                {
                    nextChar(); //leia até ; or {
                    readLen++;
                } while (ascii != 59 && ascii != 123); // ; {

                if (ascii == 59) //;
                    continue;
                else if (ascii == 123) //\n, {
                {
                    FunStackData *fs = (FunStackData *)allocate_memory(sizeof(FunStackData));
                    fs->backtrackLineIndex = backtrackLineIndex;
                    fs->backtrackCharIndex = backtrackIndex;
                    fs->backtrackScope = scope;
                    fs->funname = funname;
                    fs->startIndex = charIndex - readLen;
                    fs->lineIndex = lineIndex;
                    return fs;
                }
            }
        }
        nextChar();
    } while (charIndex + 1 < _file->charactersCount);

    return NULL;
}

void analiseFunctionByNameOrFail(char *funname)
{
    //Verifica se a função existe, se não morre o processo.
    //Varrer o código até achar e voltar para esse ponto
    FunStackData *fs = findFunctionStatement(funname);
    if (fs == NULL)
        log_abort("Chamada de função não declarada na linha: %d, função: funcao %s.\n", lineIndex + 1, funname);
    //Coloca na pilha de analise de função
    list_append(funStack, fs);
    //Aponta para o caracter/linha do inicio da declaração da função
    charIndex = fs->startIndex;
    lineIndex = fs->lineIndex;
    //Ignora o Scopo atual
    setScope(NULL);
    //Faz analise da função
    //Aguarda retornar da análise
    findTokens();
}

void checkFunctionCallOrFail(char *funname, Booleano allowAnalyse)
{
    //Verifica se a função existe, se não morre o processo
    if (allowAnalyse)
        analiseFunctionByNameOrFail(funname);

    //Ignora validação da chamada da função, leia até ;
    int w[] = {10, 59}; // ; || \n
    readTo(w, 2);

    if (ascii != 59) //;
        log_abort("Declaração/chamada de função inválida, na linha: %d, caracter esperado: \';\'.\n", lineIndex);
}

void checkReservedFunctionCall()
{
    if ((int)nextChar() != 40) //(
        log_abort("Chamada de função reservada incorreta, caracter esperado: \'(\' na linha: %d.\n", lineIndex + 1);

    //leia até ;
    int w[] = {10, 59}; // ; || \n
    readTo(w, 2);

    if (ascii != 59) // ;
        log_abort("Chamada de função reservada inválida, caracter esperado: \';\', na linha: %d.\n", lineIndex + 1);
}

void checkVariableDeclaration()
{
    if (scopeToken == NULL)
        log_abort("Declaração de variável fora de escopo.\n");

    if ((int)nextChar() != 32) //space
        log_abort("Declaração de variável incorreta na linha: %d.\n", lineIndex + 1);

    Booleano leuVirgula = FALSE;
    do
    {
        ascii == 38 ? --charIndex : charIndex; //&
        char *varname = checkVariableName();

        Token *token = getTokenByName(varname);
        if (token != NULL)
            log_abort("Variável já declarada, variável: %s, linha: %d.\n", varname, lineIndex + 1);

        char *dataLenght = checkDataLenght(&c, &ascii);
        ascii == 32 ? nextCharIgnoreSpace() : ascii; // Ignore Space

        if (ascii == 44) //,
        {
            leuVirgula = TRUE;
            pushToken(varname, variavel, pwi == inteiro_index ? inteiro : (pwi == caractere_index ? caractere : decimal), NULL, dataLenght, lineIndex, startTokenIndex, scopeToken);
            continue;
        }
        else if (ascii == 59) //; fim da declaração
        {
            leuVirgula = FALSE;
            pushToken(varname, variavel, pwi == inteiro_index ? inteiro : (pwi == caractere_index ? caractere : decimal), NULL, dataLenght, lineIndex, startTokenIndex, scopeToken);
            break;
        }
        else if (ascii == 61 && leuVirgula) //=
            log_abort("Declaração de variável incorreta, não é permitido atribuição com multiplas variáveis, linha: %d.\n", lineIndex + 1);
        else if (ascii == 61) //=
        {
            char *dataValue = checkExpression();
            pushToken(varname, variavel, pwi == inteiro_index ? inteiro : (pwi == caractere_index ? caractere : decimal), dataValue, dataLenght, lineIndex, startTokenIndex, scopeToken);
            break;
        }
        else
            log_abort("Declaração de variável incorreta, linha: %d, caracter inesperado: %c.\n", lineIndex + 1, c);
    } while (ascii == 44 && (int)nextCharIgnoreSpace() == 38); //, &
}

void checkFunctionCallOrStatment()
{
    if ((int)nextChar() != 32) //space
        log_abort("Declaração/chamada de função incorreta na linha: %d.\n", lineIndex + 1, c);

    char *funname = checkFunctionName();
    Token *funToken = getTokenByName(funname);

    if (funToken != NULL && funToken->startLineIndex == lineIndex)
    {
        charIndex = funToken->endCharIndex;
        lineIndex = funToken->endLineIndex;
        return;
    }

    //TODO: Validar com a professora
    //Ignore space ex: funcao fsoma ();
    ascii = ascii == 10 ? (int)(c = nextCharIgnoreSpace()) : ascii;
    if (ascii != 40) //(
        log_abort("Declaração/chamada de função incorreta na linha: %d, caracter esperado \'(\'.\n", lineIndex + 1);

    if ((int)nextCharIgnoreSpace() != 41) // ) { possivel chamada/declaração de função
    {
        verifyPossibleTokenType();

        if (pwi == inteiro_index || pwi == caractere_index || pwi == decimal_index)
        {
            //Declaração de função com argumentos
            setScope(pushToken(funname, funcao, vazio, NULL, NULL, lineIndex, startTokenIndex, scopeToken));
            do
            {
                verifyPossibleTokenType();
                checkReservedWord(FALSE);

                if ((int)nextChar() != 32) //space
                    log_abort("Declaração de argumento incorreto na linha: %d.\n", lineIndex + 1, c);

                char *argname = checkVariableName();
                char *dataLenght = checkDataLenght();

                pushToken(argname, argumento, pwi == inteiro_index ? inteiro : (pwi == caractere_index ? caractere : decimal), NULL, dataLenght, lineIndex, startTokenIndex, scopeToken);
            } while (ascii == 44 && ((int)nextCharIgnoreSpace() == 105 || ascii == 99 || ascii == 100)); //, &, i, c, d

            if (ascii != 41) //)
                log_abort("Declaração de função incorreta na linha: %d, caracter esperado \')\'.\n", lineIndex + 1);

            if ((int)nextCharIgnoreSpaceAndBreakLine() != 123) //{
                log_abort("Declaração de função incorreta na linha: %d, caracter esperado \'{\'.\n", lineIndex + 1, c);
        }
        else if (ascii != 123)
        {
            //Chamada de função com argumentos: OK
            checkFunctionCallOrFail(funname, funToken == NULL);
        }
        else
            log_abort("Declaração/chamada de função inválida, na linha: %d, caracter inesperado: \'%c\'.\n", lineIndex + 1, c);
    }
    else if (ascii == 41 && (int)nextCharIgnoreSpace() == 59) //), ;
    {
        //Chamada de função sem argumento: OK.
        //Verifica se a função existe, se não morre o processo
        analiseFunctionByNameOrFail(funname);
    }
    else if (ascii == 123 && funToken != NULL) // {
        log_abort("Função %s já declarada na linha: %d, redeclaração na linha: %d.\n", funname, funToken->startLineIndex + 1, lineIndex + 1);
    else if (ascii == 123) // {
    {
        //Declaração de função sem argumento
        setScope(pushToken(funname, funcao, vazio, NULL, NULL, lineIndex, startTokenIndex, scopeToken));
    }
    else
        log_abort("Declaração/chamada de função incorreta na linha:) %d, caracter inesperado: %c.\n", lineIndex + 1, c);
}

void checkMainFunction()
{
    if ((int)nextCharIgnoreSpaceAndBreakLine() != 40 || (int)nextCharIgnoreSpace() != 41) //()
        log_abort("Erro na declaração do módulo/função princial na linha: %d caracter: %c.\n", lineIndex + 1, c);

    if ((int)nextCharIgnoreSpaceAndBreakLine() != 123) //{
        log_abort("Erro na declaração do módulo/função princial na linha: %d caracter: %c.\n", lineIndex + 1, c);

    setScope(pushToken((char *)reserverd_words[pwi], principal, vazio, NULL, NULL, lineIndex, startTokenIndex, scopeToken));
}

void checkIfStatmentLine()
{
    verifyPossibleTokenType();

    if (possibleType == variavel)
        log_abort("Declaração de variável não é permitida dentro do bloco do se/senão. Linha: %d.\n", lineIndex + 1);
    else if (possibleType == palavra_reservada && pwi == para_index)
    {
        checkReservedWord(FALSE);
        checkForStatment();
    }
    else if (possibleType == palavra_reservada && (pwi == se_index || pwi == senao_index))
    {
        checkReservedWord(FALSE);
        checkIfElseStatment();
    }
    else if ((possibleType == funcao || possibleType == funcao_reservada) && checkReservedWord(FALSE))
    {
        if (possibleType == funcao && (int)nextChar() != 32) //space
            log_abort("Chamada de função incorreta na linha: %d.\n", lineIndex + 1, c);
        else if (possibleType == funcao)
        {
            char *funName = checkFunctionName();
            Token *funToken = getTokenByName(funName);
            checkFunctionCallOrFail(funName, funToken == NULL);
        }
        else
            checkReservedFunctionCall();
    }
    else if (ascii == 38) //&
    {
        //Utilização de variável
        checksForVariableUsage(FALSE);
    }
    else
        log_abort("Caracter inesperado na linha: %d, caracter: %c.\n", lineIndex + 1, c);
}

void checkIfElseStatment()
{
    Booleano leuSe = FALSE;
    do
    {
        if (pwi == se_index)
        {
            leuSe = TRUE;
            if ((int)nextCharIgnoreSpace() != 40) //(
                log_abort("Utilização de palavra reservada incorreta na linha: %d, caracter esperado \'(\'.\n", lineIndex + 1);
            int _di[] = {41, 10, 123, 59}; // ), \n, {, ;
            readTo(_di, 4);
            if (ascii != 41) // )
                log_abort("Utilização de palavra reservada incorreta na linha: %d, caracter esperado \')\'.\n", lineIndex + 1);
        }
        else if (!leuSe)
            log_abort("Não é permitido a utilização da palavra reservada senao sem um se, na linha: %d.\n", lineIndex + 1);
        else
            leuSe = FALSE;

        //Se de uma linha, não permite declaração, permite expressão e chamada de funções
        if ((int)nextCharIgnoreSpaceAndBreakLine() != 123 && ascii != 59) //{, ;
        {
            checkIfStatmentLine();
        }
        else if (ascii == 123 && (int)nextCharIgnoreSpaceAndBreakLine() != 125) //{ }
        {
            do
            {
                checkIfStatmentLine();
            } while ((int)nextCharIgnoreSpaceAndBreakLine() != 125); //}
        }

        nextCharIgnoreSpaceAndBreakLine();
        verifyPossibleTokenType();
        if (possibleType == palavra_reservada && (pwi == se_index || pwi == senao_index))
            checkReservedWord(FALSE);
        else
            charIndex--;
    } while (pwi == se_index || pwi == senao_index);
}

void checksForVariableUsage(Booleano allowAssign)
{
    charIndex--;
    char *varname = checkVariableName();
    Token *token = getTokenByName(varname);
    ascii == 32 ? nextCharIgnoreSpace() : ascii; //Ignore Space
    if (ascii == 61 && token != NULL)            //=
    {
        char *expression = checkExpression();
        if (allowAssign)
            token->value = expression;
    }
    else if (token != NULL && (ascii == 42 || ascii == 43 || ascii == 45 || ascii == 47 || ascii == 94)) // * +, -, /, ^
    {
        charIndex -= (strlen(varname) + 2);
        checkExpression();
    }
    else if (token == NULL)
        log_abort("Utilização de variável não declarada, variável: %s, linha: %d.\n", varname, lineIndex + 1);
}

void checkForStatmentLine()
{
    verifyPossibleTokenType();

    if (possibleType == variavel)
    {
        checkReservedWord(FALSE);
        checkVariableDeclaration();
    }
    else if (possibleType == palavra_reservada && pwi == para_index)
    {
        checkReservedWord(FALSE);
        checkForStatment();
    }
    else if (possibleType == palavra_reservada && (pwi == se_index || pwi == senao_index))
    {
        checkReservedWord(FALSE);
        checkIfElseStatment();
    }
    else if ((possibleType == funcao || possibleType == funcao_reservada) && checkReservedWord(FALSE))
    {
        if (possibleType == funcao && (int)nextChar() != 32) //space
            log_abort("Chamada de função incorreta na linha: %d.\n", lineIndex + 1, c);
        else if (possibleType == funcao)
        {
            char *funName = checkFunctionName();
            Token *funToken = getTokenByName(funName);
            checkFunctionCallOrFail(funName, funToken != NULL);
        }
        else
            checkReservedFunctionCall();
    }
    else if (ascii == 38) //&
    {
        //Utilização de variável
        checksForVariableUsage(FALSE);
    }
    else
        log_abort("Caracter inesperado na linha: %d, caracter: %c.\n", lineIndex + 1, c);
}

void checkForStatment()
{
    if ((int)nextCharIgnoreSpace() != 40) //(
        log_abort("Utilização de palavra reservada incorreta na linha: %d, caracter esperado \'(\'.\n", lineIndex + 1);
    int _di[] = {41, 10, 123, 59}, // ), \n, {, ;
        _pt[] = {59};              // ;

    //Ignora os 2 ;
    readTo(_pt, 1);  // ;
    if (ascii != 59) // ;
        log_abort("Utilização de palavra reservada incorreta na linha: %d, caracter esperado \';\'.\n", lineIndex + 1);
    readTo(_pt, 1);  // ;
    if (ascii != 59) // ;
        log_abort("Utilização de palavra reservada incorreta na linha: %d, caracter esperado \';\'.\n", lineIndex + 1);

    readTo(_di, 4);

    if (ascii != 41) // )
        log_abort("Utilização de palavra reservada incorreta na linha: %d, caracter esperado \')\'.\n", lineIndex + 1);

    //Se de uma linha, não permite declaração, permite expressão e chamada de funções
    if ((int)nextCharIgnoreSpaceAndBreakLine() != 123 && ascii != 59) //{, ;
    {
        checkForStatmentLine();
    }
    else if (ascii == 123 && (int)nextCharIgnoreSpaceAndBreakLine() != 125) //{ }
    {
        do
        {
            checkForStatmentLine();
        } while ((int)nextCharIgnoreSpaceAndBreakLine() != 125); //}
    }
}

void initialize()
{
    newSymbolList();
    newFunStack();
    charIndex = lineIndex = 0;
}

void findTokens()
{
    assert(symbolList != NULL && funStack != NULL);
    while ((charIndex + 1) <= _file->charactersCount)
    {
        nextCharIgnoreSpaceAndBreakLine();
        verifyPossibleTokenType();

        startTokenIndex = charIndex;

        if (possibleType != nao_identificado)
        {
            //valida palavra reservada
            checkReservedWord(FALSE);

            //Valida modulo/funcao principal
            if (possibleType == principal)
                checkMainFunction();
            else if (possibleType == funcao) //valida declaração/chamada de funcao
                checkFunctionCallOrStatment();
            else if (possibleType == variavel) //valida declaração de variável
                checkVariableDeclaration();
            else if (possibleType == funcao_reservada) //valida chamada de função reservada (leitura, escrita)
                checkReservedFunctionCall();
            else if (possibleType == palavra_reservada && pwi == para_index) //valida palavra reservada (para)
                checkForStatment();
            else if (possibleType == palavra_reservada && (pwi == se_index || pwi == senao_index)) //valida palavra reservada (se, senao)
                checkIfElseStatment();
        }
        else if (ascii == 125) //}, fim de escopo de função
        {
            if (scopeToken != NULL)
            {
                scopeToken->endCharIndex = charIndex;
                scopeToken->endLineIndex = lineIndex;
                setScope(NULL);
            }

            if (funStack->logicalLength > 0)
            {
                FunStackData *fs = (FunStackData *)list_pop(funStack);
                charIndex = fs->backtrackCharIndex;
                lineIndex = fs->backtrackLineIndex;
                setScope(fs->backtrackScope);
                freeFunStackData(fs);
                return; //Retornar para a função checkFunctionCallOrFail.
            }
        }
        else if (ascii == 38) //&, valida utilização de variável
            checksForVariableUsage(TRUE);
        else if (ascii == 43 || ascii == 45) //-, +
        {
            charIndex--;
            checkExpression();
        }
        else
            log_abort("Caracter inesperado na linha: %d, caracter: %c.\n", lineIndex + 1, c);
    }
}

void lexicalAnalysis(File *file)
{
    assert((_file = file) != NULL);
    initialize();
    findTokens();
    list_destroy(funStack);
    log_info("Finalizado analise lexica.\n");
}