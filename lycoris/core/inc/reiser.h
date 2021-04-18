#ifndef REISER
#define REISER
#define block_addr(block_num, block_size) (block_num * block_size)
#define key_offset(n) (24 + n * 16)
#define pointer_offset(n, k) (24 + k * 16 + n * 8)
#define item_headers_off(n) (24*(n+1))
#include <stdint.h>
#include "util.h"
struct item_wrapper {
	uint32_t dir_id;
	uint32_t obj_id;
	char * name;
	enum Type type;
};
int check_fs();
void read_meta(char * path_to_fs);
void print_meta(void);
void print_root_leaf(uint32_t);
#endif
