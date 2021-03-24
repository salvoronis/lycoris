#ifndef REISER
#define REISER
#include <bits/stdint-intn.h>
#include <stdint.h>
struct __attribute__((packed)) superblock {
	int32_t block_count;
	int32_t free_blocks;
	int32_t root_block;
	int32_t journal_block;

	int32_t journal_device;
	int32_t original_journal_size;
	int32_t journal_trans_max;
	int32_t journal_magic;

	int32_t journal_max_bath;
	int32_t journal_max_commit_age;
	int32_t journal_max_trans_age;
	int16_t blocksize;
	int16_t oid_max_size;

	int16_t oid_current_size;
	int16_t state;
	int8_t magic_string[12];

	int32_t hash_func_code;
	int16_t tree_height;
	int16_t bitmap_number;
	int16_t virsion;
	int16_t reserved;
	int32_t inode_generation;
};
struct __attribute__((packed)) block_header {
	int16_t level;
	int16_t number_of_items;
	int16_t free_space;
	int16_t reserved;
	int64_t right_key;
	int64_t right_key_cont;
};
int check_fs();
void read_meta(char * path_to_fs);
void print_meta(void);
#endif
