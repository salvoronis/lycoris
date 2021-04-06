#ifndef FILE_H
#define FILE_H
#include "leaf.h"
#include "reiser.h"
#include <stdint.h>
char * get_file_by_name(char *, struct item_wrapper*, uint32_t);
char * get_file(uint32_t,uint32_t);
char * get_file_chunk(struct LinkedList * node);
#endif
