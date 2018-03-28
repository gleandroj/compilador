#ifndef __MEMORY_H
#define __MEMORY_H

#include <stdlib.h>
#include "typings.h"

/**
 * Max Allowed Memory Usage in kb
 */
#ifndef MAX_MEMORY_USAGE
#define MAX_MEMORY_USAGE 1000
#endif

#define debug_current(memory) log_debug("Currect allocated memory usage: %d bytes\n", memory);
#define debug_free(memory) log_debug("Free %d bytes of memory\n", memory);

Booleano can_allocate_memory(size_t _sizeof);

void add_external_allocated_memory(void *pointer);

void remove_external_allocated_memory(void *pointer);

void *allocate_memory(size_t _sizeof);

void *realloc_memory(void *allocated, size_t _sizeof);

void free_memory(void *pointer);

int current_used_memory();

int used_memory(void *pointer);

void *copy_memory(void *from, void *to, size_t _sizeof);

extern unsigned int CURRENT_ALLOCATE_MEMORY;

#endif