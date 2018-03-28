#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>
#include "typings.h"
#include "log.h"
#include "list.h"
#include "memory.h"
#include "storage.h"

void free_line(void *data)
{
    Line *line = (Line*)data;
    free_memory(line->lineText);
    free_memory(line);
}

Line *line_new(size_t textLineSize)
{
    Line *line = (Line *)allocate_memory(sizeof(Line));
    line->lineText = (char *)allocate_memory(textLineSize);
    return line;
}

void file_read(File *file, char *filename)
{
    file->lines = (List *)allocate_memory(sizeof(List));
    list_new(file->lines, sizeof(Line), free_line);

    file->charactersCount = 0;
    file->linesCount = 0;

    FILE *arq;
    arq = fopen(filename, "r");
    add_external_allocated_memory(arq);

    if (arq == NULL) log_error("Falha na leitura do arquivo: (%s)\n", filename);

    size_t BUFFER_SIZE = MAX_LINE_READ_CHAR * sizeof(char);
    char *buffer = (char *)allocate_memory(BUFFER_SIZE);
    int bufferlen = 0;

    while ((fgets(buffer, BUFFER_SIZE, arq)) != NULL)
    {
        Line *line = line_new(((bufferlen = strlen(buffer)) + 1) * sizeof(char));
        line->lineNumber = file->linesCount;

        //bufferlen-- ignore \n char
        copy_memory(buffer, line->lineText, (--bufferlen) * sizeof(char));

        file->charactersCount += line->charCount = bufferlen;
        file->linesCount++;
        
        list_append(file->lines, line);
    }

    free_memory(buffer);
    free_memory(arq);
}

void file_destroy(File *file)
{
    list_destroy(file->lines);
    free_memory(file->lines);
    free_memory(file);
}

char file_get_char_at(File *file, int position)
{
    ListNode *node = file->lines->head;
    int charPosition = 0;
    assert(position >= 0 && position < file->charactersCount);
    while (node != NULL)
    {
        Line *line = ((Line *)(node->data));
        char *c, *lineText = line->lineText;
        for (c = lineText; *c; c++)
        {
            if (charPosition == position)
            {
                return *c;
            }
            charPosition++;
        }
        node = node->next;
    }
    return (char)-1;
}

Line *file_get_line(File *file, int line)
{
    ListNode *node = file->lines->head;
    while (node != NULL)
    {
        if (((Line *)(node->data))->lineNumber == line)
        {
            return (Line *)node->data;
        }
        node = node->next;
    }

    return NULL;
}
