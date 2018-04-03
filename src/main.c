#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include <assert.h>
#include <string.h>
#include <regex.h>

#include "util/helpers.h"
#include "config/constants.h"
#include "util/typings.h"
#include "util/memory.h"
#include "util/storage.h"
#include "util/log.h"

#define FILE_DIR "/home/gabriel/Projects/compilador/resources/arquivo.txt"

int main()
{
    File *file = (File *)allocate_memory(sizeof(File));
    log_info("Inicializando o processo de compilação...\n");
    log_info("Inicializando leitura do arquivo (%s)\n", FILE_DIR);
    file_read(file, FILE_DIR);
    log_info("Processo finalizado com sucesso...\n");
    log_info("Foram lidas %d linhas e %d caracteres.\n", file->linesCount, file->charactersCount);

    //if(!principal_exists) log_error(getMessage(ERROR_MOD_PRI_INEX));
    //else log_info("Modulo principal detectado na linha: %d\n", principal_line);
    //file_destroy(file);
    log_info("Processo de compilação finalizado...\n");
}

