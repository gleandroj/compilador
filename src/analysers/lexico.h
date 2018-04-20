#ifndef __LEXICO_H
#define __LEXICO_H

#include "../util/storage.h"
#include "../util/list.h"
#include "symbol.h"

#define MAX_VARIABLE_NAME_LEN 50

extern char *const reserved_words[];

typedef enum {
    principal_index, //0
    funcao_index,    //1
    leitura_index,   //2
    escrita_index,   //3
    se_index,        //4
    senao_index,     //5
    para_index,      //6
    inteiro_index,   //7
    caractere_index, //8
    decimal_index    //9
} ReservedWordsIndex;

typedef struct _FunStackData
{
    char *funname;
    int backtrackCharIndex;
    int backtrackLineIndex;
    int startIndex;
    Token *backtrackScope;
} FunStackData;

/**
 * Inicializa a pilha de analise de funções
 */
void newFunStack();

/**
 * Printa na tela e aborta o processo na análise léxica
 */
void log_abort(char *message, ...);

/**
 * Obtem o próximo caracter a ser analisado
 */
char nextChar();

/**
 * Obtem o próximo caracter ignorando espaço e quebra de linha
 */
char nextCharIgnoreSpaceAndBreakLine();

/**
 * Obtem o próximo caracter ignorando espaço
 */
char nextCharIgnoreSpace();

/**
 * Verifica qual o possível tipo do token que estou analisando
 */
void verifyPossibleTokenType();

/**
 * Define a função/modulo como escopo atual
 */
void setScope(Token *scope);

/**
 * Verifica a declaração do nome da variável
 */
char *checkVariableName();

/**
 * Verifica a declaração do nome de função
 */
char *checkFunctionName();

/**
 * Verifica a escrita da palavra reservada
 */
Booleano checkReservedWord(Booleano disableFail);

/**
 * Verifica a escrita do tamanho da variável
 * Ex: [30] | [1.3]
 */
char *checkDataLenght();

/**
 * Verifica a formação de expressões
 */
char *checkExpression();

/**
 * Leia até ponto-e-virgula (;) ou quebra de linha (\n)
 */
void readToSemicolonOrBreakLine();

/*
 * Leia até o caracter passado como parâmetro 
 */
void readTo(int chars[], int len);

/**
 * Pesquisa pelo início da declaração de uma função
 */
FunStackData *findFunctionStatement(char *funname);

/**
 * Realiza a análise para uma função ou falha se não encontrar
 */
void checkFunctionCallOrFail(char *funname);

/**
 * Inicializa a tabela de simbolos e a pilha de análise de funções
 * e os indices
 */
void initialize();

/**
 * Função responsável por orquestrar a análise léxica
 */
void findTokens();

/**
 * Realiza a análise léxica
 */
void lexical_analysis(File *file);

#endif