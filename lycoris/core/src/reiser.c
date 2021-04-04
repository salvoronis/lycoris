#include "../inc/reiser.h"
#include "../inc/linked_list.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define DEBUG

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

enum Item_type {
	ERR,
	FIFO,
	CHR,
	DIR,
	BLK,
	REG,
	LINK,
	SOCK
};

char *Item_type_wrap[] = {"error","fifo","char dev","dir","blk","regular file","link","socket"};//TEST

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

static enum Type get_keyv1_type(int32_t type) {
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

static enum Type get_keyv2_type(int32_t type) {
	int mask = 0xF; //mask to get 4 bits
	int value = type & mask;
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
		printf("%s\n-------------\n",(char*)(node->item+dirs[i].location));
	}
}

void print_root_leaf() {
	unsigned int root = block_addr(meta->root_block, meta->blocksize);
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
		#ifdef DEBUG
		printf(
			"-key -> %X|%X|%X|%X\n"
			"-count -> %X\n"
			"-length -> %u\n"
			"-location -> %u\n"
			"-version -> %u\n",
			tmp->header.key[0],tmp->header.key[1],
			tmp->header.key[2],tmp->header.key[3],
			tmp->header.count,
			tmp->header.length,
			tmp->header.location,
			tmp->header.version
		);
		#endif
		enum Type key_type;
		if (tmp->header.version == 0) {
			key_type = get_keyv1_type(tmp->header.key[3]);
		} else
			key_type = get_keyv2_type(tmp->header.key[3]);

		if (key_type == Directory) {
			get_directories(tmp);
		}

		puts("");
		tmp = tmp->next;
	}
}

int check_fs() {
	if (strcmp("ReIsEr2Fs", (char*)meta->magic_string) == 0) {
		return 1;
	}
	return 0;
}

static struct LinkedList * parseLeafNode(unsigned int block_addr){
	struct block_header * data = malloc(sizeof(struct block_header));
	fseek(fs, block_addr, SEEK_SET);
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

unsigned int get_dir(int32_t dir_id, struct item_wrapper ** items_in_dir) {
	struct LinkedList * head = parseLeafNode(block_addr(meta->root_block,meta->blocksize));
	unsigned int items_count = 0;
	while (head != NULL) {
		enum Type key_type;
		if (head->header.version == 0) {
			key_type = get_keyv1_type(head->header.key[3]);
		} else
			key_type = get_keyv2_type(head->header.key[3]);

		if (key_type == Directory && head->header.key[0] == dir_id) {
			items_count += head->header.count;
			struct dir_header * dirs =
				malloc(head->header.count * sizeof(struct dir_header));
			dirs = (struct dir_header*) head->item;
			*items_in_dir = realloc(*items_in_dir, 
					head->header.count*sizeof(struct item_wrapper));
			for (int i = 0; i < head->header.count; i++) {
				(*items_in_dir)[i].dir_id = dirs[i].dir_id;
				(*items_in_dir)[i].obj_id = dirs[i].object_id;
				(*items_in_dir)[i].name = (char*)(head->item+dirs[i].location);
			}

		}
		head = head->next;
	}
	return items_count;
}
