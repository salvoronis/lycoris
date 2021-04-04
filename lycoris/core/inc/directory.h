#ifndef DIRECTORY_H
#include <stdint.h>
#define DIRECTORY_H
unsigned int get_dir(int32_t dir_id, int32_t obj_id, struct item_wrapper **);
void list_directory(struct item_wrapper*, unsigned int inum);
unsigned int change_dir(struct item_wrapper**, unsigned int*, char*, int32_t*, int32_t*);
void print_working_dir(int32_t, int32_t);
#endif
