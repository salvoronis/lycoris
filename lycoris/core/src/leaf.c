#include "../inc/leaf.h"
#include "../inc/reiser.h"
#include "../inc/linked_list.h"
#include <stdlib.h>
#include <stdio.h>


extern FILE * fs;
/**
 * function parseLeafNode receive block addres in fs
 * read block header, items headers and items
 * then create a linked list with two values: header and item(with data)
 * return list's head
 * */
/*struct LinkedList * parseLeafNode(unsigned int block_addr){
	struct block_header * data = malloc(sizeof(struct block_header));
	fseek(fs, block_addr, SEEK_SET);
	fread(data, sizeof(struct block_header), 1, fs);
	struct item_header *headers =
		malloc(sizeof(struct item_header)*(data->number_of_items+1));

	fread(headers, sizeof(struct item_header), data->number_of_items + 1, fs);

	struct LinkedList *head = NULL;

	for (int i = 0; i <= data->number_of_items; i++) {
		void * item = malloc(headers[i].length);
		fseek(fs, block_addr+headers[i].location, SEEK_SET);
		fread(
			item,
			headers[i].length,
			1,
			fs);
		push(&head, headers[i], item, headers[i].length);
		free(item);
	}
	return head;
}

struct LinkedList * get_item_by_ids(uint32_t dir, uint32_t obj) {
	struct LinkedList * head = get_leaf_block_by_key(dir, obj);
	while (head != NULL) {
		if (head->header.key.dir_id == dir &&
		head->header.key.obj_id == obj)
			return head;
		head = head->next;
	}
	return NULL;
}

*/
