#include <stdlib.h>
#include <stdio.h>

#include "util/storage.h"
#include "util/list.h"

Booleano iterate_int(void *data)
{
    printf("Found value: %d\n", *(int *)data);
    return TRUE;
}

int main()
{
    int numbers = 10;
    printf("Generating list with the first %d positive numbers...\n", numbers);

    int i;
    List list;
    list_new(&list, sizeof(int), NULL);

    for (i = 1; i <= numbers; i++)
    {
        list_append(&list, &i);
    }

    list_for_each(&list, iterate_int);

    list_destroy(&list);
    printf("Successfully freed %d numbers...\n", numbers);
}