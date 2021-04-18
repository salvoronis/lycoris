#include "../inc/file.h"
#include "../inc/reiser.h"
#include "../inc/reiser_structures.h"
#include "../inc/util.h"
#include "../inc/linked_list.h"
#include "../inc/btree.h"
#include "../inc/directory.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

extern struct superblock * meta;

void copy(char * from, char * to, struct item_wrapper * cur, uint32_t inum) {
	for (uint32_t i = 0; i < inum; i++) {
		if (strcmp(cur[i].name, from) == 0) {
			struct reiser_key skey;
			skey.dir_id = cur[i].dir_id;
			skey.obj_id = cur[i].obj_id;
			struct item_header * tmp = get_item_by_ids(skey);
			enum Type type;
			if (tmp->version == 0) {
				type = get_keyv1_type(tmp->key.u.k_offset_v1.type);
			} else
				type = get_keyv2_type(tmp->key.u.k_offset_v2.offset);
			if (type == Directory) {
				char * resDir = calloc(strlen(to) + 2, 1);
				strcat(resDir, to);
				strcat(resDir, "/");
				struct stat st = {0};
				struct reiser_key dkey;
				dkey.dir_id = 1;
				dkey.obj_id = 2;

				struct item_wrapper * tmp = malloc(sizeof(struct item_wrapper) *
						inum);
				memcpy(tmp, cur, sizeof(struct item_wrapper) * inum);
				uint32_t inumtmp = inum;
				if (stat(resDir, &st) == -1){
					mkdir(resDir, 0700);
					if ((change_dir(&tmp, &inumtmp, 
							from, &dkey) == 0)) {
						for (uint32_t i = 0; i < inumtmp; i++)
							if (strcmp(tmp[i].name, ".") != 0 &&
							strcmp(tmp[i].name, "..") != 0){
								char * newFile = calloc(
									strlen(tmp[i].name) +
									strlen(resDir) + 2, 1
								);
								strcat(newFile, resDir);
								strcat(newFile, tmp[i].name);
								copy(
									tmp[i].name,
									newFile,
									tmp,
									inumtmp
								);
							}
					}
				}
			} else if (type == Direct) {
				FILE * cpto = fopen(to, "w+");
				struct reiser_key kes = tmp->key;
				kes.u.k_offset_v2.offset = 0;
				char * data = get_file(kes);

				fwrite(data, strlen(data), 1, cpto);
				fclose(cpto);
				return;
			}
		}
	}
}

char * get_file_by_name(char * name, struct item_wrapper * cur, uint32_t inum) {
	for (uint32_t i = 0; i < inum; i++) {
		if (strcmp(cur[i].name, name) == 0) {
			struct reiser_key skey;
			skey.dir_id = cur[i].dir_id;
			skey.obj_id = cur[i].obj_id;
			struct item_header * tmp = get_item_by_ids(skey);
			if (tmp == NULL) {
				return "lol it's null";
			}
			enum Type type;
			if (tmp->version == 0) {
				type = get_keyv1_type(tmp->key.u.k_offset_v1.type);
			} else {
				type = get_keyv2_type(tmp->key.u.k_offset_v2.offset);
			}

			skey.u.k_offset_v2.offset = 0;
			return get_file(skey);
		}
	}
	return NULL;
}

static uint16_t get_file_chunk(struct reiser_key skey, char * data) {
	struct LinkedList * head = get_leaf_block_by_key(skey);
	while (head != NULL) {
		enum Type type;
		if (head->header.version == 0) {
			type = get_keyv1_type(head->header.key.u.k_offset_v1.type);
		} else
			type = get_keyv2_type(head->header.key.u.k_offset_v2.offset);

		if (head->header.key.dir_id == skey.dir_id &&
				head->header.key.obj_id == skey.obj_id &&
				type == Direct) {
			strcat(data, head->item);
			return head->header.length;
		}
		head = head->next;
	}
	return 0;
}

char * get_file(struct reiser_key skey) {
	struct LinkedList * head = get_leaf_block_by_key(skey);
	struct LinkedList * tmp = head;
	enum Type type;
	while (tmp != NULL) {
		if (tmp->header.version == 0) {
			type = get_keyv1_type(tmp->header.key.u.k_offset_v1.type);
		} else
			type = get_keyv2_type(tmp->header.key.u.k_offset_v2.offset);


		if (tmp->header.key.dir_id == skey.dir_id &&
				tmp->header.key.obj_id == skey.obj_id &&
				type == Stat) {
			uint64_t size = 0;
			char * data;
			if (tmp->header.version == 0){
				size = ((struct stat_item_v1*)tmp->item)->size;
			} else 
				size = ((struct stat_item_v2*)tmp->item)->size;

			size = align_to_8(size);
			data = calloc(size, 1);

			skey.u.k_offset_v2.offset = 1;

			while (size != 0) {
				uint64_t chunk_size = get_file_chunk(skey, data);
				skey.u.k_offset_v2.offset =
					skey.u.k_offset_v2.offset + chunk_size;
				size -= chunk_size;
			}

			return data;
		}
		tmp = tmp->next;
	}
	return "hehehe";
}
