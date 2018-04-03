//#include <storage.h>

/**
 * Mensagens
**/
char * const messages[] = {
    "Módulo Principal Inexistente.\n",
    "Memória Insuficiente.\n"
};

char* const getMessage(int index){
    return messages[index];
}