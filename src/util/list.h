#ifndef __LIST_H
#define __LIST_H

#include "typings.h"

// a common function used to free malloc'd objects
typedef void (*FreeFunction)(void *);
typedef Booleano (*ListIterator)(void *);

typedef struct _ListNode
{
    void *data;
    struct _ListNode *next;
} ListNode;

typedef struct
{
    int logicalLength;
    int elementSize;
    ListNode *head;
    ListNode *tail;
    FreeFunction freeFn;
} List;

void list_new(List *list, int elementSize, FreeFunction freeFn);
void list_destroy(List *list);
void list_append(List *list, void *element);
void list_for_each(List *list, ListIterator iterator);

#endif