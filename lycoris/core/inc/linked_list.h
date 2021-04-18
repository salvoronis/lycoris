#ifndef LINKED_LIST
#include <stdlib.h>
#include "../inc/reiser.h"
#include "../inc/reiser_structures.h"
#define LINKED_LIST
struct LinkedList {
	struct item_header header;
	void * item;
	struct LinkedList * next;
};
void push(struct LinkedList**, struct item_header, void*, size_t);
#endif
