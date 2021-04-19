#include "../inc/btree.h"
#include <string.h>

extern struct superblock * meta;

/**
 * receive dir and obj id and array of dir's headers
 * change arg items_in_dir and return number of dirs in item
 * */
unsigned int get_dir(struct reiser_key skey, struct item_wrapper ** items_in_dir) {
	struct LinkedList * head = get_leaf_block_by_key(skey);

	unsigned int items_count = 0;
	while (head != NULL) {
		enum Type key_type;
		if (head->header.version == 0) {
			key_type = get_keyv1_type(head->header.key.u.k_offset_v1.type);
		} else
			key_type = get_keyv2_type(head->header.key.u.k_offset_v2.offset);

		if (key_type == Directory &&
		head->header.key.dir_id == skey.dir_id &&
		head->header.key.obj_id == skey.obj_id) {
			items_count = head->header.count;
			struct dir_header * dirs =
				malloc(head->header.count * sizeof(struct dir_header));
			dirs = (struct dir_header*) head->item;
			*items_in_dir = realloc(*items_in_dir, 
					head->header.count*sizeof(struct item_wrapper));
			uint32_t floor = head->header.length;
			for (int i = 0; i < head->header.count; i++) {
				(*items_in_dir)[i].dir_id = dirs[i].dir_id;
				(*items_in_dir)[i].obj_id = dirs[i].object_id;
				(*items_in_dir)[i].name = 
					(char*) calloc(floor-dirs[i].location+1,1);
				memcpy(
					(*items_in_dir)[i].name,
					head->item+dirs[i].location,
					floor-dirs[i].location);
				floor = dirs[i].location;
			}

		}
		head = head->next;
	}
	return items_count;
}

/**
 * change current item_wrapper, key and inum to new directory,
 * pointed by new_dir name
 * */
unsigned int change_dir(
		struct item_wrapper ** cur,
		unsigned int * inum,
		char * new_dir,
		struct reiser_key * skey) {

	for (int i = 0; i < *inum; i++){
		if (strcmp((*cur)[i].name, new_dir) == 0) {
			skey->dir_id = (*cur)[i].dir_id;
			skey->obj_id = (*cur)[i].obj_id;
		}
	}
	if (skey->dir_id != 0) {
		*inum = get_dir(*skey, cur);
		return 0;
	}
	return 1;
}

/**
 * receive current dir's key
 * produce rath to received key
 * */
char * print_working_dir(struct reiser_key skey){
	struct item_wrapper * current = malloc(1);
	unsigned int inum = get_dir(skey, &current);
	char * path = calloc(0,0);
	int i = 0;
	int32_t old_dir = 0;
	int32_t old_obj = 0;

	while (skey.dir_id != 0) {
		i++;
		old_dir = skey.dir_id;
		old_obj = skey.obj_id;
		
		change_dir(&current, &inum, "..", &skey);
		for (int i = 0; i < inum; i++) {
			if (current[i].dir_id == old_dir && current[i].obj_id == old_obj) {
				path = realloc(path, strlen(path)+strlen(current[i].name)+2);
				strcat(path, current[i].name);
				strcat(path, "->");
			}
		}
	}
	return path;
	free(current);
	free(path);
}
