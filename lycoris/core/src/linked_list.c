#include "../inc/linked_list.h"
#include "../inc/reiser.h"
#include <stdio.h>
#include <stdlib.h>

void push(struct LinkedList ** head, struct item_header header, void * item, size_t size) {
	struct LinkedList * new_node = (struct LinkedList*) malloc(sizeof(struct LinkedList));

	new_node->header = header;
	new_node->next = *head;

	new_node->item = malloc(size);
	for (int i = 0; i < size; i++)
		*(char *)(new_node->item + i) = *(char *)(item + i);

	*head = new_node;
}
