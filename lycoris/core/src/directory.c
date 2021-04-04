#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "../inc/reiser.h"
#include "../inc/directory.h"
#include "../inc/linked_list.h"
#include "../inc/util.h"
#include "../inc/leaf.h"
#include <string.h>

extern struct superblock * meta;

/**
 * receive dir and obj id and array of dir's headers
 * change arg items_in_dir and return number of dirs in item
 * */
unsigned int get_dir(int32_t dir_id, int32_t obj_id, struct item_wrapper ** items_in_dir) {
	struct LinkedList * head = parseLeafNode(block_addr(meta->root_block,meta->blocksize));
	unsigned int items_count = 0;
	while (head != NULL) {
		enum Type key_type;
		if (head->header.version == 0) {
			key_type = get_keyv1_type(head->header.key[3]);
		} else
			key_type = get_keyv2_type(head->header.key[3]);

		if (key_type == Directory &&
		head->header.key[0] == dir_id &&
		head->header.key[1] == obj_id) {
			items_count = head->header.count;
			struct dir_header * dirs =
				malloc(head->header.count * sizeof(struct dir_header));
			dirs = (struct dir_header*) head->item;
			*items_in_dir = realloc(*items_in_dir, 
					head->header.count*sizeof(struct item_wrapper));
			for (int i = 0; i < head->header.count; i++) {
				(*items_in_dir)[i].dir_id = dirs[i].dir_id;
				(*items_in_dir)[i].obj_id = dirs[i].object_id;
				(*items_in_dir)[i].name = (char*)(head->item+dirs[i].location);
			}

		}
		head = head->next;
	}
	return items_count;
}

void list_directory(struct item_wrapper * items, unsigned int inum) {
	for (int i = 0; i < inum; i++) {
		printf("%s\n", items[i].name);
	}
}

unsigned int change_dir(
		struct item_wrapper ** cur,
		unsigned int * inum,
		char * new_dir,
		int32_t * dir_id,
		int32_t * obj_id) {

	for (int i = 0; i < *inum; i++){
		if (strcmp((*cur)[i].name, new_dir) == 0) {
			*dir_id = (*cur)[i].dir_id;
			*obj_id = (*cur)[i].obj_id;
		}
	}
	if (*dir_id != 0) {
		*inum = get_dir(*dir_id, *obj_id, cur);
		return 0;
	}
	return 1;
}

/**
 * receive current dir's dir and obj id
 * */
void print_working_dir(int32_t dir_id, int32_t obj_id){
	struct item_wrapper * current = malloc(1);
	unsigned int inum = get_dir(dir_id, obj_id, &current);
	char * path = calloc(0,0);
	int i = 0;
	int32_t old_dir = 0;
	int32_t old_obj = 0;

	while (dir_id != 0) {
		i++;
		old_dir = dir_id;
		old_obj = obj_id;
		
		change_dir(&current, &inum, "..", &dir_id, &obj_id);
		for (int i = 0; i < inum; i++) {
			if (current[i].dir_id == old_dir && current[i].obj_id == old_obj) {
				path = realloc(path, strlen(path)+strlen(current[i].name)+2);
				strcat(path, current[i].name);
				strcat(path, "->");
			}
		}
	}
	puts(path);
	free(current);
	free(path);
}
