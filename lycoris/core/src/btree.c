#include "../inc/btree.h"
#include "../inc/reiser.h"
#include "../inc/linked_list.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern struct superblock * meta;
extern FILE * fs;

struct LinkedList * get_leaf_block_by_key(struct reiser_key skey) {
	uint32_t root_bl = meta->root_block;
	uint32_t root = block_addr(root_bl, meta->blocksize);
	struct block_header * data = malloc(sizeof(struct block_header));
	struct LinkedList * result;
	fseek(fs, root, SEEK_SET);
	fread(data, sizeof(struct block_header), 1, fs);

	uint32_t dir_tmp = 0;

	while (data->level != 1) {
		struct reiser_key * keys =
			malloc(sizeof(struct reiser_key)*data->number_of_items);
		fread(keys, sizeof(struct reiser_key), data->number_of_items, fs);
		
		struct partition * prts =
			malloc(sizeof(struct partition)*(data->number_of_items+1));
		fread(prts, sizeof(struct partition), data->number_of_items+1, fs);

		uint32_t iter = 0;
		uint32_t ditmp = 0;
		while (iter < data->number_of_items &&
				skey.dir_id > keys[iter].dir_id) {
			ditmp = keys[iter].dir_id;
			iter++;
		}

		while (iter < data->number_of_items &&
				skey.obj_id > keys[iter].obj_id &&
				keys[iter].dir_id == ditmp) {
			iter++;
		}

		if (keys[iter].obj_id < skey.obj_id) {
			iter++;
		}

		root_bl = prts[iter].pointer;

		root = block_addr(root_bl, meta->blocksize);
		fseek(fs, root, SEEK_SET);
		fread(data, sizeof(struct block_header), 1, fs);

		free(prts);
		free(keys);

		if (data->level == 1)
			return parseLeafNode(root);
	}
	return result;
}

/**
 * function parseLeafNode receive block addres in fs
 * read block header, items headers and items
 * then create a linked list with two values: header and item(with data)
 * return list's head
 * */
struct LinkedList * parseLeafNode(uint32_t block_addr){
	struct block_header * data = malloc(sizeof(struct block_header));
	fseek(fs, block_addr, SEEK_SET);
	fread(data, sizeof(struct block_header), 1, fs);
	struct item_header *headers =
		malloc(sizeof(struct item_header)*(data->number_of_items));

	fread(headers, sizeof(struct item_header), data->number_of_items, fs);

	struct LinkedList *head = NULL;

	for (int i = 0; i < data->number_of_items; i++) {
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

struct item_header * get_item_by_ids(struct reiser_key skey) {
	struct LinkedList * head = get_leaf_block_by_key(skey);
	while (head != NULL) {
		if (head->header.key.dir_id == skey.dir_id &&
		head->header.key.obj_id == skey.obj_id) {
			return &(head->header);
		}
		head = head->next;
	}
	return NULL;
}
