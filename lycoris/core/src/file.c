#include "../inc/file.h"
#include "../inc/reiser.h"
#include "../inc/leaf.h"
#include "../inc/util.h"
#include "../inc/linked_list.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern struct superblock * meta;

char * get_file_by_name(char * name, struct item_wrapper * cur, uint32_t inum) {
	for (uint32_t i = 0; i < inum; i++) {
		if (strcmp(cur[i].name, name) == 0) {
			return get_file(cur[i].dir_id, cur[i].obj_id);
		}
	}
	return get_file(1, 2);
}

char * get_file(uint32_t dir_id, uint32_t obj_id) {
	struct LinkedList * head = parseLeafNode(block_addr(meta->root_block, meta->blocksize));
	struct LinkedList * tmp;
	uint32_t offset = 0;
	enum Type key_t;
	tmp = head;
	while (tmp != NULL) {
		if (tmp->header.key.dir_id == dir_id && tmp->header.key.obj_id == obj_id) {
			if (tmp->header.version == 0) {
				if (tmp->header.key.u.k_offset_v1.offset == 0) {
					offset = ((struct stat_item_v1*)head->item)->num_links;
					break;
				}
			} else if (tmp->header.version == 1) {
				if (get_keyv2_offset(tmp->header.key.u.k_offset_v2.offset)==0) {
					offset = ((struct stat_item_v2*)tmp->item)->num_links;
					break;
				}
			}
		}
		tmp = tmp->next;
	}
	char * data = calloc(0, 0);
	for (uint64_t i = 1; i <= offset; i++){
		tmp = head;
		while (tmp != NULL) {
			enum Type key_type;
			if (tmp->header.version == 0) {
				key_type =
					get_keyv1_type(tmp->header.key.u.k_offset_v1.type);
			} else 
				key_type =
					get_keyv2_type(tmp->header.key.u.k_offset_v2.offset);
			if (key_type == Direct &&
			tmp->header.key.dir_id == dir_id &&
			tmp->header.key.obj_id == obj_id) {
				data = realloc(data, strlen(data) + tmp->header.length);
				char * tmp_str = get_file_chunk(tmp);
				strcat(data,tmp_str);
				//free(tmp_str);
			}
			tmp = tmp->next;
		}
	}
	free(tmp);
	//TODO make list_free func
	return data;
}

char * get_file_chunk(struct LinkedList * node) {
	return (char *)node->item;
}
