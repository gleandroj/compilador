#include <string.h>
#include <assert.h>
#include <regex.h>

#include "log.h"
#include "helpers.h"
#include "memory.h"

void substr(char *dest, char *input, int offset, int len)
{
    assert(offset + len < (strlen(input) + 1) && len > 0);
    //char *_substr = allocate_memory(sizeof(char) * (len + 1));
    strncpy(dest, input + offset, len);
    dest[len + 1] = '\0';
    //return _substr;
}