#include <string.h>
#include "helpers.h"
#include "memory.h"

char *hstrcat(char *s1, char *s2)
{
    const size_t a = strlen(s1);
    const size_t b = strlen(s2);
    const size_t size_ab = a + b + 1;
    s1 = realloc_memory(s1, size_ab);
    memcpy(s1 + a, s2, b + 1);
    return s1;
}