#include "../inc/functions.h"
#include <dirent.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void partitions_list(char * device_name);

void list_devises() {
	DIR *devices = opendir("/sys/block");
	struct dirent *entry;
	if (!devices) {
		perror("Can not open /sys/block.");
		exit(1);
	}
	puts("list of devices:\n.");
	while ( (entry = readdir(devices)) != NULL ) {
		if (strcmp(entry->d_name,".") == 0 || strcmp(entry->d_name,"..") == 0)
			continue;
		printf("├── %s\n", entry->d_name);

		partitions_list(entry->d_name);
	}
	closedir(devices);
}

void partitions_list(char * device_name) {
	DIR *partition_dir = opendir(concat("/sys/block/", device_name));
	struct dirent *entry;
	if (!partition_dir) {
		perror("Can not open device directory.");
		exit(1);
	}
	while ( (entry = readdir(partition_dir)) != NULL ) {
		if (strstr(entry->d_name, device_name) != NULL) {
			printf("│   ├── %s\n", entry->d_name);
		}
	}
	closedir(partition_dir);
}

char * concat(char *s1, char *s2) {
	char *result = malloc(strlen(s1) + strlen(s2) + 1);
	strcpy(result, s1);
	strcat(result, s2);
	return result;
}
