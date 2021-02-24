#include "../inc/reiser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SUPERBLOCK_START 0x10000

int check_fs(char * path_to_fs) {
	FILE * fs = fopen(path_to_fs, "rb");
	struct superblock * meta = malloc(sizeof(struct superblock));
	fseek(fs, SUPERBLOCK_START, SEEK_CUR);
	fread(meta, sizeof(struct superblock), 1, fs);
	if (strcmp("ReIsEr2Fs", (char*)meta->magic_string) == 0) {
		return 1;
	}
	return 0;
}
