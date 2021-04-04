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
enum Type get_keyv1_type(int32_t);
enum Type get_keyv2_type(int32_t);
#endif
