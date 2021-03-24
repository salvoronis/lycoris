#include "../inc/reiser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SUPERBLOCK_START 0x10000
#define block_addr(block_num, block_size) (block_num * block_size)

struct superblock * meta;
FILE * fs;

void read_meta(char *path_to_fs) {
	fs = fopen(path_to_fs, "rb");
	meta = malloc(sizeof(struct superblock));
	fseek(fs, SUPERBLOCK_START, SEEK_SET);
	fread(meta, sizeof(struct superblock), 1, fs);
}

void print_root_block(){
	unsigned int root = block_addr(meta->root_block, meta->blocksize);
	struct block_header * data = malloc(sizeof(struct block_header));
	fseek(fs, root, SEEK_SET);
	fread(data, sizeof(struct block_header), 1, fs);
	printf(
		"number of items -> %d\n"
		"level -> %d\n"
		"free space -> %d\n",
		data->number_of_items,
		data->level,
		data->free_space
		);

	struct key * keys = malloc(sizeof(struct key) * (data->number_of_items + 1));
	fread(keys, sizeof(struct key), data->number_of_items + 1, fs);
	for (int i = 0; i < data->number_of_items + 1; i++) {
		printf(
			"-->block num: %d\n"
			"-->size: %d\n"
			"-->reserved: %d\n\n",
			keys[i].block_number,
			keys[i].size,
			keys[i].reserved
			);
	}
	struct partition * prts = malloc(sizeof(struct partition) * (data->number_of_items + 2));
	fread(prts, sizeof(struct partition), data->number_of_items+2, fs);
	for (int i = 0; i < data->number_of_items + 2; i++) {
		printf(
			"1 = %d\t"
			"2 = %d\n",
			prts[i].prt1,
			prts[i].prt2
			);
	}
}

int check_fs() {
	if (strcmp("ReIsEr2Fs", (char*)meta->magic_string) == 0) {
		return 1;
	}
	return 0;
}

void print_meta() {
	printf(
		"Meta data\n"
		"\t-->Block count: %d\n"
		"\t-->Free blocks: %d\n"
		"\t-->Root block: %d\n"
		"\t-->Journal block: %d\n"
		"\t-->Journal device: %d\n"
		"\t-->Original journal size: %d\n"
		"\t-->Journal trans max: %d\n"
		"\t-->Journal block count: %d\n"
		"\t-->Journal max bath: %d\n"
		"\t-->Journal max commit age: %d\n"
		"\t-->Journal max trans age: %d\n"
		"\t-->Blocksize: %d\n"
		"\t-->OID max size: %d\n"
		"\t-->OID current size: %d\n"
		"\t-->State: %d\n"
		"\t-->Magical string: %s\n"
		"\t-->Hash function code: %d\n"
		"\t-->Tree height: %d\n"
		"\t-->Bitmap number: %d\n"
		"\t-->Version: %d\n"
		"\t-->Reserved: %d\n"
		"\t-->Inode Generation: %d\n",
		meta->block_count,
		meta->free_blocks,
		meta->root_block,
		meta->journal_block,
		meta->journal_device,
		meta->original_journal_size,
		meta->journal_trans_max,
		meta->journal_magic,
		meta->journal_max_bath,
		meta->journal_max_commit_age,
		meta->journal_max_trans_age,
		meta->blocksize,
		meta->oid_max_size,
		meta->oid_current_size,
		meta->state,
		meta->magic_string,
		meta->hash_func_code,
		meta->tree_height,
		meta->bitmap_number,
		meta->virsion,
		meta->reserved,
		meta->inode_generation
	);
	print_root_block();
}
