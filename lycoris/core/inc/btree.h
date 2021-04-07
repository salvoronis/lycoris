#ifndef BTREE_H
#define BTREE_H
#include "leaf.h"
#include "reiser.h"
#include <stdint.h>
struct LinkedList * get_leaf_block_by_key(uint32_t, uint32_t);
#endif
