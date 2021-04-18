#ifndef FILE_H
#define FILE_H
#include "reiser.h"
#include "reiser_structures.h"
#include "linked_list.h"
#include <stdint.h>
char * get_file_by_name(char *, struct item_wrapper*, uint32_t);
char * get_file(struct reiser_key);
void copy(char *, char *, struct item_wrapper *, uint32_t);
#endif
