#ifndef __STORAGE_H
#define __STORAGE_H

#include "list.h"

typedef struct _file
{
    int lines;
    int characters;
    List lineList;
} File;

typedef booleano (*fileIterator)(char *, int line, int chart);

File file_read(char *filename);
void file_destroy(File *file);
void file_character_for_each(File *file, fileIterator fileIteratorFn);
char *file_get_char_at(File *file, int position);
char *file_get_line(File *file, int line);

#endif