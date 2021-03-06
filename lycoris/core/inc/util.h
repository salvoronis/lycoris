#include "reiser_structures.h"
#ifndef UTIL_H
#include <stdint.h>
#define UTIL_H
#define S_IFSOCK 	0xC000 //socket
#define S_IFLINK 	0xA000 //symbolic link
#define S_IFREG 	0x8000 //regular file
#define S_IFBLK 	0x6000 //block device
#define S_IFDIR 	0x4000 //directory
#define S_IFCHR 	0x2000 //character device
#define S_IFIFO 	0x1000 //fifo
enum Key_format {
	KEY_FORMAT_1,
	KEY_FORMAT_2
};

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
uint64_t align_to_8(uint64_t);
uint64_t get_offset(struct reiser_key);
enum Type get_keyv1_type(int32_t);
enum Type get_keyv2_type(uint64_t);
uint64_t get_keyv2_offset(uint64_t);
int cmp_keys(struct reiser_key, struct reiser_key);
#endif
