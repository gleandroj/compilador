#include "util/storage.h"

extern char * const reserved_words[];
/*
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
*/
typedef char* TokenValue;

typedef enum {
    variavel,
    funcao,
    argumento
} TokenType;

typedef enum {
    inteiro,
    caractere,
    decimal
} TokenDataType;

typedef struct {
    TokenType type;
    TokenDataType dataType;
    TokenValue tokenValue;
    float tokenDataLenght;
    Token *tokenParent;
    Line *line;
} Token;
