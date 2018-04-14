#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include "typings.h"
#include "memory.h"
#include "log.h"

unsigned int CURRENT_ALLOCATE_MEMORY = 0;

Booleano can_allocate_memory(size_t _sizeof)
{
    return (CURRENT_ALLOCATE_MEMORY + (int)_sizeof) < MAX_MEMORY_USAGE ? TRUE : FALSE;
}

void add_external_allocated_memory(void *pointer)
{
    int used = used_memory(pointer);
    CURRENT_ALLOCATE_MEMORY += used;
    log_debug("External allocated more %d bytes of memory.\n", used);
    debug_current(CURRENT_ALLOCATE_MEMORY);
}

void remove_external_allocated_memory(void *pointer)
{
    int memory = used_memory(pointer);
    CURRENT_ALLOCATE_MEMORY = ((CURRENT_ALLOCATE_MEMORY - memory) <= 0 ? 0 : CURRENT_ALLOCATE_MEMORY - memory);
    log_debug("External free %d bytes of memory.\n", memory);
    debug_current(CURRENT_ALLOCATE_MEMORY);
}

void *allocate_memory(size_t _sizeof)
{
    void *allocated;
    int memory;
    if (!can_allocate_memory(_sizeof) || !(allocated = malloc(_sizeof)))
    {
        log_debug("Cannot allocate %d bytes of memory.", (int)_sizeof);
        log_error("Memória Insuficiente.\n");
    }
    CURRENT_ALLOCATE_MEMORY += memory = used_memory(allocated);
    log_debug("Allocated more %d bytes of memory.\n", memory);
    debug_current(CURRENT_ALLOCATE_MEMORY);
    return allocated;
}

void *realloc_memory(void *allocated, size_t _sizeof)
{
    int oldmemory, memory;
    CURRENT_ALLOCATE_MEMORY -= oldmemory = used_memory(allocated);

    if (!can_allocate_memory(_sizeof) || !(allocated = realloc(allocated, _sizeof)))
    {
        log_debug("Cannot reallocate %d bytes of memory.", (int)_sizeof);
        log_error("Memória Insuficiente.\n");
    }
    CURRENT_ALLOCATE_MEMORY += memory = used_memory(allocated);

    log_debug("Reallocated from %d bytes to %d bytes of memory.\n", oldmemory, memory);
    debug_current(CURRENT_ALLOCATE_MEMORY);

    return allocated;
}

void *copy_memory(void *from, void *to, size_t _sizeof)
{
    return memcpy(to, from, _sizeof);
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
