#ifndef __MEMORY_H
#define __MEMORY_H

#include <stdlib.h>

void* can_allocate_memory(size_t _sizeof);

void allocate_memory(size_t _sizeof);

void free_memory(void *pointer);

int used_memory();

#endif