#include "reiser_structures.h"
#include "reiser.h"
#ifndef DIRECTORY_H
#include <stdint.h>
#define DIRECTORY_H
unsigned int get_dir(struct reiser_key, struct item_wrapper **);
unsigned int change_dir(struct item_wrapper**, unsigned int*, char*, struct reiser_key *);
void print_working_dir(struct reiser_key);
#endif
