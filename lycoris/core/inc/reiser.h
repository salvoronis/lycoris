#ifndef REISER
#define REISER
#define block_addr(block_num, block_size) (block_num * block_size)
#define key_offset(n) (24 + n * 16)
#define pointer_offset(n, k) (24 + k * 16 + n * 8)
#define item_headers_off(n) (24*(n+1))
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
struct __attribute__((packed)) offset_v1 {
	uint32_t offset;
	uint32_t type;
};
struct __attribute__((packed)) offset_v2 {
	uint64_t offset;
};
struct __attribute__((packed)) reiser_key {
	uint32_t dir_id;
	uint32_t obj_id;
	union {
		struct offset_v1 k_offset_v1;
		struct offset_v2 k_offset_v2;
	} __attribute__((packed)) u;
};
struct __attribute__((packed)) block_header {
	int16_t level;
	int16_t number_of_items;
	int16_t free_space;
	int16_t reserved;
	struct reiser_key rkey;
};
struct __attribute__((packed)) key {
	int32_t block_number;
	int32_t size;
	int32_t reserved;
	int32_t type; //this part is missing in the fucking documentation, but it in use in hex.
};
struct __attribute__((packed)) partition {
	int16_t prt1;
	int16_t shit; //this part is missing too. fuck this fs
	int16_t prt2;
	int16_t junk; //this is missing too, oh I'm so fucking love to read hex.
};
struct __attribute__((packed)) item_header {
	struct reiser_key key;
	int16_t count;
	int16_t length;
	int16_t location;
	int16_t version;
};
struct __attribute__((packed)) stat_item_v1 {
	int16_t mode;
	int16_t num_links;
	int16_t UID;
	int16_t GID;
	int32_t size;
	int32_t atime;
	int32_t mtime;
	int32_t ctime;
	int32_t rdev_blocks;
	int32_t first_dir_byte;
};
struct __attribute__((packed)) stat_item_v2 {
	int16_t mode;
	int16_t reserved;
	uint32_t num_links;
	int64_t size;
	int32_t UID;
	int32_t GID;
	int32_t atime;
	int32_t mtime;
	int32_t ctime;
	int32_t blocks;
	int32_t rdev_gen_first;
};
struct __attribute__((packed)) dir_header {
	int32_t offset;
	uint32_t dir_id;
	uint32_t object_id;
	int16_t location;
	int16_t state;
};
struct item_wrapper {
	uint32_t dir_id;
	uint32_t obj_id;
	char * name;
};
int check_fs();
void read_meta(char * path_to_fs);
void print_meta(void);
unsigned int get_dir(int32_t dir_id, int32_t obj_id, struct item_wrapper **);
#endif
