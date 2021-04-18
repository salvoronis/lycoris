#ifndef BTREE_H
#define BTREE_H
#include "linked_list.h"
#include "reiser.h"
#include <stdint.h>
struct LinkedList * get_leaf_block_by_key(struct reiser_key);
struct LinkedList * parseLeafNode(uint32_t block_addr);
struct item_header * get_item_by_ids(struct reiser_key);
#endif
