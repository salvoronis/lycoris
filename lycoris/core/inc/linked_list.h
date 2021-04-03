#ifndef LINKED_LIST
#include "reiser.h"
#include <stdlib.h>
#define LINKED_LIST
struct LinkedList {
	struct item_header header;
	void * item;
	struct LinkedList * next;
};
void push(struct LinkedList**, struct item_header, void*, size_t);
#endif
