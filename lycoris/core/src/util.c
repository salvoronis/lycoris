#include "../inc/util.h"
#include <stdint.h>
#include <stdio.h>
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
