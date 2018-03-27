#include <stdio.h>
#include <stdlib.h>

#include "typings.h"
#include "list.h"
#include "storage.h"

void free_string(void *data)
{
  free(*(char **)data);
}

void file_read(File* file, char *filename)
{
    List* lineList;
    list_new(lineList, sizeof(char), free_string);

    file->charactersCount = 0;
    file->linesCount = 0;
    //file->lines
    FILE *f;
    /*
    char *str;
    f = fopen("test.txt", "rt");
    while (fgets(str, 100, f) != EOF){
        file->
    }*/
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
