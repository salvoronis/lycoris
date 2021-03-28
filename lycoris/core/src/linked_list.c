#include "../inc/linked_list.h"
#include "../inc/reiser.h"
#include <stdio.h>
#include <stdlib.h>

void push(struct LinkedList ** head, struct item_header header, void * item) {
	struct LinkedList * new_node = (struct LinkedList*) malloc(sizeof(struct LinkedList));

	new_node->header = header;
	new_node->next = *head;

	size_t size = header.version == 0 ? 32 : 44;
	new_node->item = malloc(size);
	for (int i = 0; i < size; i++)
		*(char *)(new_node->item + i) = *(char *)(item + i);

	*head = new_node;
}

void LinkedList_print(struct LinkedList * head) {
	while (head != NULL) {
		printf("%X, ver %d\n",head->header.location, head->header.version);
		if (head->header.version == 0) {
			printf("item size - %X\n\n",((struct stat_item_v1*)(head->item))->size);
		} else
			printf("v2 item size - %X\n\n", ((struct stat_item_v2*)(head->item))->size);
		head = head->next;
	}
}
