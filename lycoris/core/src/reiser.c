#include "../inc/reiser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SUPERBLOCK_START 0x10000

struct superblock * meta;

void read_meta(char *path_to_fs) {
	FILE * fs = fopen(path_to_fs, "rb");
	meta = malloc(sizeof(struct superblock));
	fseek(fs, SUPERBLOCK_START, SEEK_CUR);
	fread(meta, sizeof(struct superblock), 1, fs);
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
}
