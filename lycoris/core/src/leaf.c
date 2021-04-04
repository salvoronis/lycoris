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
struct LinkedList * parseLeafNode(unsigned int block_addr){
	struct block_header * data = malloc(sizeof(struct block_header));
	fseek(fs, block_addr, SEEK_SET);
	fread(data, sizeof(struct block_header), 1, fs);
	/*#ifdef DEBUG
	printf(
		"number of items -> %d\n"
		"level -> %d\n"
		"free space -> %d\n\n",
		data->number_of_items,
		data->level,
		data->free_space
	);
	#endif*/
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
