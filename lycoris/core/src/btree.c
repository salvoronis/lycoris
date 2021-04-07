#include "../inc/btree.h"
#include "../inc/reiser.h"
#include "../inc/linked_list.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern struct superblock * meta;
extern FILE * fs;

struct LinkedList * get_leaf_block_by_key(uint32_t d_id, uint32_t o_id) {
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
		while (iter < data->number_of_items && d_id > keys[iter].dir_id) {
			iter++;
		}
		uint32_t dtmp = keys[iter].dir_id;
		while (iter < data->number_of_items && o_id >= keys[iter].obj_id && keys[iter].dir_id == dtmp) {
			iter++;
		}

		root_bl = prts[iter].pointer;

		//printf("new root -> %u\n", root_bl);
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
