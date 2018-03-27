#ifndef MAX_MEMORY_USAGE
/**
 * Max Allowed Memory Usage in kb
 */
#define MAX_MEMORY_USAGE 1000
#endif

#include <stdlib.h>
#include "memory.h"
#include "log.h"

int CURRENT_ALLOCATE_MEMORY = 0;

void* can_allocate_memory(size_t _sizeof)
{
    void* allocated;
    if(!(allocated=malloc(10 * sizeof (int)))){

    }

    return allocated;
}

void allocate_memory(size_t _sizeof)
{
}

void free_memory(void *pointer)
{
}

int used_memory()
{
}
