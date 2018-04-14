#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>

#include "typings.h"
#include "log.h"
#include "memory.h"
#include "storage.h"
#include "helpers.h"

void file_read(File *file, char *filename)
{
    file->_file = (char*)allocate_memory(sizeof(char));
    file->charactersCount = 0;
    file->linesCount = 0;

    FILE *arq;
    arq = fopen(filename, "r");
    add_external_allocated_memory(arq);

    if (arq == NULL)
        log_error("Falha na leitura do arquivo: (%s)\n", filename);

    size_t BUFFER_SIZE = MAX_LINE_READ_CHAR * sizeof(char);
    char *buffer = (char *)allocate_memory(BUFFER_SIZE);
    int bufferlen = 0;

    while ((fgets(buffer, BUFFER_SIZE, arq)) != NULL)
    {
        bufferlen = strlen(buffer);
        file->_file = hstrcat(file->_file, buffer);
        file->charactersCount += bufferlen;
        file->linesCount++;
    }

    free_memory(buffer);
    free_memory(arq);
}

void file_destroy(File *file)
{
    free_memory(file->_file);
    free_memory(file);
}

char file_get_char_at(File *file, int position)
{
    int charPosition = 0;
    assert(position >= 0 && position < file->charactersCount);
    return (char)file->_file[position];
}