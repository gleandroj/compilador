#include <stdio.h>
#include "list.h"
#include "storage.h"

File file_read(char *filename)
{
    FILE *_f;
    _f = fopen(filename, "r");
}

void file_destroy(File *file)
{
}

void file_character_for_each(File *file, fileIterator fileIteratorFn)
{
}

char *file_get_char_at(File *file, int position)
{
}

char *file_get_line(File *file, int line)
{
}
