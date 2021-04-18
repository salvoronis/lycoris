#include "../inc/btree.h"
#include "../inc/util.h"
#include "../inc/reiser.h"
#include "../inc/linked_list.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern struct superblock * meta;
extern FILE * fs;

static uint32_t key_binsearch(struct reiser_key * keys,
				int32_t left,
				int32_t right,
				struct reiser_key skey) {
	if (right >= left) {
		int32_t middle = (left + right) / 2;
		if (cmp_keys(keys[middle], skey) == 0) {
			return middle;
		} else if (cmp_keys(keys[middle], skey) > 0) {
			return key_binsearch(keys, left, middle - 1, skey);
		}
		return key_binsearch(keys, middle + 1, right, skey);
	}
	return left - 1;
}

struct LinkedList * get_leaf_block_by_key(struct reiser_key skey) {
	uint32_t root_bl = meta->root_block;
	uint32_t root = block_addr(root_bl, meta->blocksize);
	struct block_header * data = malloc(sizeof(struct block_header));
	struct LinkedList * result;
	if (skey.u.k_offset_v2.offset == 0x50) {
		skey.u.k_offset_v2.offset = 0;
	}
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

		uint32_t iter = key_binsearch(keys, 0, data->number_of_items - 1, skey) + 1;

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
