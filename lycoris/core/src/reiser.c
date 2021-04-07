#include "../inc/reiser.h"
#include "../inc/linked_list.h"
#include "../inc/util.h"
#include "../inc/btree.h"
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
 * MUST BE DELETED
 * */
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

	struct reiser_key * keys = malloc(sizeof(struct reiser_key)*(data->number_of_items));
	fread(keys, sizeof(struct reiser_key), data->number_of_items, fs);
	for (int i = 0; i < data->number_of_items; i++) {
		printf(
			"-->dir_id: %u\n"
			"-->obj_id: %u\n"
			"-->offset: %u\n"
			"-->type: %u\n\n",
			keys[i].dir_id,
			keys[i].obj_id,
			keys[i].u.k_offset_v1.offset,
			keys[i].u.k_offset_v1.type
			);
	}
	struct partition * prts = malloc(sizeof(struct partition)*(data->number_of_items + 1));
	fread(prts, sizeof(struct partition), data->number_of_items+1, fs);
	for (int i = 0; i < data->number_of_items + 1; i++) {
		printf(
			"pointer = %u\t"
			"size = %u\n",
			prts[i].pointer,
			prts[i].size
			);
	}
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
 * PRINT DIRS IN LEAF MUST BE DELETED
 * */
void get_directories(struct LinkedList * node){
	struct dir_header * dirs = malloc(node->header.count * sizeof(struct dir_header));
	dirs = (struct dir_header*) node->item;
	for (int i = 0; i < node->header.count; i++) {
		#ifdef DEBUG
		printf(
			"--offset %u\n"
			"--dir_id %u\n"
			"--obj_id %u\n"
			"--locati %u\n"
			"--state  %u\n-------\n",
			dirs[i].offset,
			dirs[i].dir_id,
			dirs[i].object_id,
			dirs[i].location,
			dirs[i].state
		);
		#endif
		char * name_tmp = calloc(node->header.length+1, 1);
		strcat(name_tmp, (char*)(node->item+dirs[i].location));
		printf("%s\n-------------\n",name_tmp); //8 byte align! /TODO
	}
}

/**
 * MUST BE DELETED
 * */
void print_root_leaf(uint32_t block_num) {
	unsigned int root = block_addr(block_num, meta->blocksize);
	struct block_header * data = malloc(sizeof(struct block_header));
	fseek(fs, root, SEEK_SET);
	fread(data, sizeof(struct block_header), 1, fs);
	#ifdef DEBUG
	printf(
		"number of items -> %d\n"
		"level -> %d\n"
		"free space -> %d\n\n",
		data->number_of_items,
		data->level,
		data->free_space
	);
	#endif
	struct item_header *headers = malloc(sizeof(struct item_header)*(data->number_of_items+1));
	fread(headers, sizeof(struct item_header), data->number_of_items + 1, fs);

	struct LinkedList *head = NULL;

	for (int i = 0; i <= data->number_of_items; i++) {
		void * item = malloc(headers[i].length);
		fseek(fs, root+headers[i].location, SEEK_SET);
		fread(
			item,
			headers[i].length,
			1,
			fs);
		push(&head, headers[i], item, headers[i].length);
		free(item);
	}

	struct LinkedList * tmp = head;
	while (tmp != NULL) {
		enum Type key_type;
		if (tmp->header.version == 0) {
			key_type = get_keyv1_type(tmp->header.key.u.k_offset_v1.type);
		} else
			key_type = get_keyv2_type(tmp->header.key.u.k_offset_v2.offset);
		#ifdef DEBUG
		uint32_t off;
		uint32_t tp;
		if (tmp->header.version == 0) {
			off = tmp->header.key.u.k_offset_v1.offset;
			tp = tmp->header.key.u.k_offset_v1.type;
		} else {
			off = get_keyv2_offset(tmp->header.key.u.k_offset_v2.offset);
			tp = get_keyv2_type(tmp->header.key.u.k_offset_v2.offset);
		}
		printf(
			"-key -> %X|%X|%X|%X\n"
			"-count -> %X\n"
			"-length -> %u\n"
			"-location -> %u\n"
			"-version -> %u\n",
			tmp->header.key.dir_id,
			tmp->header.key.obj_id,
			off, tp,
			tmp->header.count,
			tmp->header.length,
			tmp->header.location,
			tmp->header.version
		);
		#endif
		if (key_type == Directory) {
			get_directories(tmp);
		}

		puts("");
		tmp = tmp->next;
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
	get_leaf_block_by_key(52,57);
	print_root_block();
	//print_root_leaf(8211);
	//print_root_leaf(8229);
}
