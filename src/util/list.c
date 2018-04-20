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

void free_element(List *list, ListNode *node)
{
    if (list->freeFn)
    {
        list->freeFn(node->data);
    }
    else
    {
        free_memory(node->data);
    }
    free_memory(node);
}

void list_destroy(List *list)
{
    ListNode *current;
    while (list->head != NULL)
    {
        current = list->head;
        list->head = current->next;
        free_element(list, current);
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

void *list_pop(List *list)
{
    void *data = NULL;
    if (list->logicalLength == 0)
    {
        return NULL;
    }
    else if (list->logicalLength == 1)
    {
        data = list->head->data;
        free_memory(list->head);
        list->head = list->tail = NULL;
    }
    else
    {
        ListNode *nodeToPop = list->tail, *node = list->head;
        while (node != NULL && node != nodeToPop)
        {
            node = node->next;
        }
        data = nodeToPop->data;
        free_memory(nodeToPop);
        node->next = NULL;
        list->tail = node;
    }

    list->logicalLength--;
    return data;
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