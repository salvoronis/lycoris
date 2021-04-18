#include "../inc/reiser.h"
#include "../inc/reiser_structures.h"
#include "../inc/linked_list.h"
#include "../inc/util.h"
#include "../inc/btree.h"
#include "../inc/file.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG

#define SUPERBLOCK_START 0x10000

#define block_addr(block_num, block_size) (block_num * block_size)
#define key_offset(n) (24 + n * 16)
#define pointer_offset(n, k) (24 + k * 16 + n * 8)
#define item_headers_off(n) (24*(n+1))

struct superblock * meta;
FILE * fs;

/**
 * read and write meta data from superblock to global var 
 * */
void read_meta(char *path_to_fs) {
	fs = fopen(path_to_fs, "rb");
	meta = malloc(sizeof(struct superblock));
	fseek(fs, SUPERBLOCK_START, SEEK_SET);
	fread(meta, sizeof(struct superblock), 1, fs);
}

/**
 * wrapper function to get item type
 * */
static enum Item_type get_item_type(int16_t mode) {
	uint16_t mask = 0xF000;
	uint16_t value = mode & mask;
	switch (value) {
		case S_IFIFO:
			return FIFO;
		case S_IFCHR:
			return CHR;
		case S_IFDIR:
			return DIR;
		case S_IFBLK:
			return BLK;
		case S_IFREG:
			return REG;
		case S_IFLINK:
			return LINK;
		case S_IFSOCK:
			return SOCK;
		default:
			return ERR;
	}
}

/**
 * Compare magic field with expected value
 * */
int check_fs() {
	return strcmp("ReIsEr2Fs", (char*)meta->magic_string) == 0? 1:0;
}

/**
 * prints superblock's fields
 * */
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
}
