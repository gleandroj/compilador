#ifndef __LEXICO_H
#define __LEXICO_H

#include "../util/storage.h"

extern char * const reserved_words[];

#define MAX_VARIABLE_NAME_LEN 100

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
} ReservedWordsIndex ;

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

typedef struct _Token {
    TokenType type;
    TokenDataType dataType;
    char *value;
    char *name;
    char *dataLenght;
    struct _Token *parent;
    int lineIndex;
    int startCharIndex;
} Token;

typedef struct _pilhaItem {
    int charIndex;
    int ascii;
} PilhaItem;

typedef struct _SymbolList {
    int tokenCount;
    List tokens;
} SymbolList;

void lexical_analysis(File *file);

#endif