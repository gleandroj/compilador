#include <assert.h>
#include "log.h"
#include "memory.h"
#include "list.h"

void list_new(List *list, int elementSize, FreeFunction freeFn)
{
    assert(elementSize > 0);
    list->logicalLength = 0;
    list->elementSize = elementSize;
    list->head = list->tail = NULL;
    list->freeFn = freeFn;
}

void list_destroy(List *list)
{
    ListNode *current;
    while (list->head != NULL)
    {
        current = list->head;
        list->head = current->next;

        if (list->freeFn)
        {
            list->freeFn(current->data);
        }
        else
        {
            free_memory(current->data);
        }

        free_memory(current);
    }
}

void list_append(List *list, void *element)
{
    ListNode *node = allocate_memory(sizeof(ListNode));
    node->data = element;
    node->next = NULL;
    if (list->logicalLength == 0)
    {
        list->head = list->tail = node;
    }
    else
    {
        list->tail->next = node;
        list->tail = node;
    }
    list->logicalLength++;
}

void list_for_each(List *list, ListIterator iterator)
{
    assert(iterator != NULL);

    ListNode *node = list->head;
    Booleano result = TRUE;
    while (node != NULL && result)
    {
        result = iterator(node->data);
        node = node->next;
    }
}