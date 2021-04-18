#include "../inc/util.h"
#include "../inc/reiser_structures.h"
#include <stdint.h>
#include <stdio.h>

uint64_t align_to_8(uint64_t x) {
	return (x % 8 > 0)? ((x/8)+1)*8 : x;
}

static int cmp_elements(uint64_t x, uint64_t y) {
	if (x > y) {
		return 1;
	} else if (x < y) {
		return -1;
	}
	return 0;
}

int cmp_keys(struct reiser_key key1, struct reiser_key key2) {
	int result = 0;
	uint64_t off1 = get_offset(key1);
	uint64_t off2 = get_offset(key2);
	if ((result = cmp_elements(key1.dir_id, key2.dir_id)) == 0) {
		if ((result = cmp_elements(key1.obj_id, key2.obj_id)) == 0) {
			if ((result = cmp_elements(off1, off2)) == 0) {
				return 0;
			} else
				return result;
		} else
			return result;
	} else
		return result;
}

static inline int is_key_format_1(uint32_t type){
	return (type == 0 || type == 15) ? 1 : 0;
}

enum Key_format key_format(struct reiser_key key) {
	uint32_t type = key.u.k_offset_v2.offset >> 60;
	if (is_key_format_1(type)) {
		return KEY_FORMAT_1;
	}
	return KEY_FORMAT_2;
}

uint64_t get_offset(struct reiser_key key){
	if (key_format(key) == KEY_FORMAT_1)
		return key.u.k_offset_v1.offset;
	return get_keyv2_offset(key.u.k_offset_v2.offset);
}

/**
 * function wrapper to get key type for 1v and 2v
 * */
enum Type get_keyv1_type(int32_t type) {
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

enum Type get_keyv2_type(uint64_t off) {
	uint8_t value = off >> 60;
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

uint64_t get_keyv2_offset(uint64_t off) {
	uint64_t mask = 0xfffffffffffffff; //mask to get 60 bit of offset
	return off & mask;
}

/**
 * wrapper function to get item type
 * */
enum Item_type get_item_type(int16_t mode) {
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
