#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>

#include "typings.h"
#include "memory.h"
#include "log.h"

unsigned int CURRENT_ALLOCATE_MEMORY = 0;

Booleano can_allocate_memory(size_t _sizeof)
{
    return (CURRENT_ALLOCATE_MEMORY + (int)_sizeof) < MAX_MEMORY_USAGE  ? TRUE : FALSE;
}

void add_external_allocated_memory(void *pointer){
    CURRENT_ALLOCATE_MEMORY += used_memory(pointer);
    debug_current(CURRENT_ALLOCATE_MEMORY);
}

void remove_external_allocated_memory(void *pointer){
    int memory = used_memory(pointer);
    CURRENT_ALLOCATE_MEMORY = ((CURRENT_ALLOCATE_MEMORY - memory) <= 0 ? 0 : CURRENT_ALLOCATE_MEMORY - memory);
    debug_current(CURRENT_ALLOCATE_MEMORY);
}

void *allocate_memory(size_t _sizeof)
{
    void *allocated;
    if (!can_allocate_memory(_sizeof) || !(allocated = malloc(_sizeof)))
    {
        log_fatal("Cannot allocate %f memory.", (int)_sizeof);
    }
    CURRENT_ALLOCATE_MEMORY += used_memory(allocated);
    debug_current(CURRENT_ALLOCATE_MEMORY);
    return allocated;
}

void free_memory(void *pointer)
{
    int memory = used_memory(pointer);
    debug_free(memory);
    CURRENT_ALLOCATE_MEMORY = ((CURRENT_ALLOCATE_MEMORY - memory) <= 0 ? 0 : CURRENT_ALLOCATE_MEMORY - memory);
    free(pointer);
    debug_current(CURRENT_ALLOCATE_MEMORY);
}

int current_used_memory()
{
    return CURRENT_ALLOCATE_MEMORY;
}

int used_memory(void *pointer)
{
    return (int)malloc_usable_size(pointer);
}
