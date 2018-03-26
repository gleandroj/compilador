#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "list.h"

void list_new(List *list, int elementSize, freeFunction freeFn)
{
    assert(elementSize > 0);
    list->logicalLength = 0;
    list->elementSize = elementSize;
    list->head = list->tail = NULL;
    list->freeFn = freeFn;
}

void list_destroy(List *list)
{
    listNode *current;
    while (list->head != NULL)
    {
        current = list->head;
        list->head = current->next;

        if (list->freeFn)
        {
            list->freeFn(current->data);
        }

        free(current->data);
        free(current);
    }
}

void list_prepend(List *list, void *element)
{
    listNode *node = malloc(sizeof(listNode));
    node->data = malloc(list->elementSize);
    memcpy(node->data, element, list->elementSize);

    node->next = list->head;
    list->head = node;

    // first node?
    if (!list->tail)
    {
        list->tail = list->head;
    }

    list->logicalLength++;
}

void list_append(List *list, void *element)
{
    listNode *node = malloc(sizeof(listNode));
    node->data = malloc(list->elementSize);
    node->next = NULL;

    memcpy(node->data, element, list->elementSize);

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

void list_for_each(List *list, listIterator iterator)
{
    assert(iterator != NULL);

    listNode *node = list->head;
    booleano result = TRUE;
    while (node != NULL && result)
    {
        result = iterator(node->data);
        node = node->next;
    }
}

void list_head(List *list, void *element, booleano removeFromList)
{
    assert(list->head != NULL);

    listNode *node = list->head;
    memcpy(element, node->data, list->elementSize);

    if (removeFromList)
    {
        list->head = node->next;
        list->logicalLength--;

        free(node->data);
        free(node);
    }
}

void list_tail(List *list, void *element)
{
    assert(list->tail != NULL);
    listNode *node = list->tail;
    memcpy(element, node->data, list->elementSize);
}

int list_size(List *list)
{
    return list->logicalLength;
}