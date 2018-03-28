#ifndef __STORAGE_H
#define __STORAGE_H

#include "typings.h"
#include "list.h"

#ifndef MAX_LINE_READ_CHAR
#define MAX_LINE_READ_CHAR 100
#endif

typedef struct _line{
    int lineNumber;
    int charCount;
    char* lineText;
} Line;

typedef struct _file
{
    int charactersCount;
    int linesCount;
    List* lines;
} File;

//typedef Booleano (*FileCharIterator)(char* caracter, int lineNum, int charNum);
//typedef Booleano (*FileLineIterator)(Line* line, int lineNum);

void file_read(File* file, char* filename);
void file_destroy(File* file);

char file_get_char_at(File* file, int position);
Line* file_get_line(File* file, int line);

#endif