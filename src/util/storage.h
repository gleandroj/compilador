#ifndef __STORAGE_H
#define __STORAGE_H

#include "typings.h"

#ifndef MAX_LINE_READ_CHAR
#define MAX_LINE_READ_CHAR 100
#endif

typedef struct _file
{
    int charactersCount;
    int linesCount;
    char* _file;
} File;

void file_read(File* file, char* filename);
void file_destroy(File* file);
char file_get_char_at(File* file, int position);

#endif