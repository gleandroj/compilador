#include "util/helpers.h"
#include "util/typings.h"
#include "util/memory.h"
#include "util/storage.h"
#include "util/log.h"
#include "analysers/lexico.h"

#define FILE_DIR "/home/gabriel/Projects/gleandroj/compilador/resources/arquivo.txt"
File *file = NULL;

int main()
{
    file = (File *)allocate_memory(sizeof(File));
    log_info("Inicializando o processo de compilação...\n");
    log_info("Inicializando leitura do arquivo (%s)\n", FILE_DIR);
    file_read(file, FILE_DIR);
    log_info("Processo finalizado com sucesso...\n");
    log_info("Foram lidos %d caracteres.\n", file->charactersCount);
    lexical_analysis(file);
    //if(!principal_exists) log_error(getMessage(ERROR_MOD_PRI_INEX));
    //else log_info("Modulo principal detectado na linha: %d\n", principal_line);
    file_destroy(file);
    //log_info("Processo de compilação finalizado...\n");
}
