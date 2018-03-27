#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "typings.h"
#include "log.h"
#include "list.h"
#include "memory.h"
#include "storage.h"

void free_line(void *data)
{
    free_memory((Line*)data);
}

void file_read(File *file, char *filename)
{
    list_new(&file->lines, sizeof(Line), free_line);
    file->charactersCount = 0;
    file->linesCount = 0;
    FILE *arq;
    arq = fopen(filename, "r");
    add_external_allocated_memory(arq);
    char *buffer = (char *)allocate_memory(MAX_LINE_READ_CHAR * sizeof(char));
    if (arq == NULL)
        log_error("Falha na leitura do arquivo: (%s)\n", filename);
    Line *line = (Line *)allocate_memory(sizeof(Line));
    line->lineText = (char *)allocate_memory(MAX_LINE_READ_CHAR * sizeof(char));
    while ((fgets(buffer, MAX_LINE_READ_CHAR * sizeof(char), arq)) != NULL)
    {
        line->lineNumber = file->linesCount;
        copy_memory(buffer, line->lineText, strlen(buffer) * sizeof(char) - 1);
        list_append(&file->lines, line);
        file->linesCount++;
        file->charactersCount += strlen(buffer) - 1;
    }
    
    free_memory(line);
    free_memory(arq);
    free_memory(buffer);
}

void file_destroy(File *file)
{
    list_destroy(&file->lines);
    free_memory(file);
}

void file_character_for_each(File *file, FileCharIterator fileIteratorFn)
{
}

void file_line_for_each(File *file, FileLineIterator fileIteratorFn)
{
}

char *file_get_char_at(File *file, int position)
{
}

Line *file_get_line(File *file, int line)
{
    listNode *node = file->lines.head;
    while (node != NULL)
    {
        if (((Line *)(node->data))->lineNumber == line)
        {
            return (Line *)node->data;
        }
        node = node->next;
    }
}
