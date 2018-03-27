#include <stdio.h>
#include <stdlib.h>

#include "typings.h"
#include "log.h"
#include "list.h"
#include "memory.h"
#include "storage.h"

void free_string(void *data)
{
    free_memory((char*)data);
}

void file_read(File *file, char *filename)
{
    list_new(&file->lines, sizeof(char), free_string);
    file->charactersCount = 0;
    file->linesCount = 0;
    FILE *arq;
    arq = fopen(filename, "r");
    add_external_allocated_memory(arq);
    char *buffer = (char*)allocate_memory(MAX_LINE_READ_CHAR * sizeof(char));
    if (arq == NULL) log_error("Falha na leitura do arquivo: (%s)\n", filename);

    while ((fgets(buffer, MAX_LINE_READ_CHAR * sizeof(char), arq)) != NULL)
        printf("%s\n", buffer);

    /*
    char *str;
    f = fopen("test.txt", "rt");
    while (fgets(str, 100, f) != EOF){
        file->
    }*/

    free_memory(arq);
    free_memory(buffer);
}

void file_destroy(File *file)
{
}

void file_character_for_each(File *file, FileIterator fileIteratorFn)
{
}

char *file_get_char_at(File *file, int position)
{
}

char *file_get_line(File *file, int line)
{
}
