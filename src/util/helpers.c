#include <string.h>
#include <assert.h>
#include <regex.h>

#include "log.h"
#include "helpers.h"
#include "memory.h"

char *substr(char *input, int offset, int len)
{
    assert(offset + len < (strlen(input) + 1) && len > 0);
    char *_substr = allocate_memory(sizeof(char) * len);
    strncpy(_substr, input + offset, len);
    _substr[len] = '\0';
    return _substr;
}