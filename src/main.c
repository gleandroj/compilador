
#include <stdlib.h>
#include <stdio.h>

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
    log_info("%d characters and %d lines were read.\n", file->charactersCount, file->linesCount);
    Line *line = file_get_line(file, 1);
    printf("Value: %s\n", line->lineText);
    file_destroy(file);
}