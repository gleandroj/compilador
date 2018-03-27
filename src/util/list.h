#ifndef __LIST_H
#define __LIST_H

#include "typings.h"

// a common function used to free malloc'd objects
typedef void (*FreeFunction)(void *);
typedef Booleano (*ListIterator)(void *);

typedef struct _listNode
{
    void *data;
    struct _listNode *next;
} listNode;

typedef struct
{
    int logicalLength;
    int elementSize;
    listNode *head;
    listNode *tail;
    FreeFunction freeFn;
} List;

void list_new(List *list, int elementSize, FreeFunction freeFn);
void list_destroy(List *list);

void list_prepend(List *list, void *element);
void list_append(List *list, void *element);
int list_size(List *list);

void list_for_each(List *list, ListIterator iterator);
void list_head(List *list, void *element, Booleano removeFromList);
void list_tail(List *list, void *element);

#endif