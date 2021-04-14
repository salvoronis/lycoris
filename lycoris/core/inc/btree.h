#ifndef BTREE_H
#define BTREE_H
#include "leaf.h"
#include "linked_list.h"
#include "reiser.h"
#include <stdint.h>
struct LinkedList * get_leaf_block_by_key(uint32_t, uint32_t);
struct LinkedList * parseLeafNode(uint32_t block_addr);
struct item_header * get_item_by_ids(uint32_t, uint32_t);
#endif
