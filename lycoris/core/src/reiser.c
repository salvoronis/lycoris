#include "../inc/reiser.h"
#include "../inc/linked_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SUPERBLOCK_START 0x10000
#define S_IFSOCK 	0xC000 //socket
#define S_IFLINK 	0xA000 //symbolic link
#define S_IFREG 	0x8000 //regular file
#define S_IFBLK 	0x6000 //block device
#define S_IFDIR 	0x4000 //directory
#define S_IFCHR 	0x2000 //character device
#define S_IFIFO 	0x1000 //fifo

#define block_addr(block_num, block_size) (block_num * block_size)
#define key_offset(n) (24 + n * 16)
#define pointer_offset(n, k) (24 + k * 16 + n * 8)
#define item_headers_off(n) (24*(n+1))

enum Type {
	Stat,
	Indirect,
	Direct,
	Directory,
	Any
};

struct superblock * meta;
FILE * fs;

static inline int is_key_format_1 (int keys_type) {
	return ((keys_type == 0 || keys_type == 15) ? 1 : 0);
}

void read_meta(char *path_to_fs) {
	fs = fopen(path_to_fs, "rb");
	meta = malloc(sizeof(struct superblock));
	fseek(fs, SUPERBLOCK_START, SEEK_SET);
	fread(meta, sizeof(struct superblock), 1, fs);
}

static int get_keyv1_type(int type) {
	switch (type) {
		case 0x0:
			return Stat;
		case 0xfffffffe:
			return Indirect;
		case 0xffffffff:
			return Direct;
		case 500:
			return Directory;
		default:
			return Any;

	}
}

static int get_keyv2_type(int type) {
	int mask = 0xF; //mask to get 4 bits
	int value = type & mask;
	printf("mask -> %d\n", value);
	switch (value) {
		case 0:
			return Stat;
		case 1:
			return Indirect;
		case 2:
			return Direct;
		case 3:
			return Directory;
		default:
			return Any;
	}
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

	struct key * keys = malloc(sizeof(struct key) * (data->number_of_items));
	fread(keys, sizeof(struct key), data->number_of_items, fs);
	for (int i = 0; i < data->number_of_items; i++) {
		printf(
			"-->block num: %u\n"
			"-->size: %u\n"
			"-->reserved: %u\n"
			"-->type: %u\n\n",
			keys[i].block_number,
			keys[i].size,
			keys[i].reserved,
			keys[i].type
			);
	}
	struct partition * prts = malloc(sizeof(struct partition)*(data->number_of_items + 1));
	fread(prts, sizeof(struct partition), data->number_of_items+1, fs);
	for (int i = 0; i < data->number_of_items + 1; i++) {
		printf(
			"1 = %u\t"
			"2 = %u\n",
			prts[i].prt1,
			prts[i].prt2
			);
	}
}

void print_root_leaf() {
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
	struct item_header *headers = malloc(sizeof(struct item_header)*(data->number_of_items+1));
	fread(headers, sizeof(struct item_header), data->number_of_items + 1, fs);

	unsigned int offset = block_addr(meta->root_block, meta->blocksize) +
		item_headers_off(data->number_of_items) + data->free_space;
	fseek(fs, offset, SEEK_SET);
	void * item = malloc(44); //44 is a item max size
	struct LinkedList *head = NULL;

	for (int i = data->number_of_items; i >= 0; i--) {
		fread(item, headers[i].version == 0? 32:44, 1, fs);
		push(&head, headers[i], item);
	}
	free(item);
	LinkedList_print(head);
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
	//print_root_block();
	print_root_leaf();
}
