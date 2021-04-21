#include "../inc/functions.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void partitions_list(char * device_name, char * output);
static char * get_loc(char * device);

char * list_devises() {
	char * output = calloc(2000, 1);
	DIR *devices = opendir("/sys/block");
	struct dirent *entry;
	char * where;
	if (!devices) {
		perror("Can not open /sys/block.");
		exit(1);
	}
	//puts("list of devices:\n.");
	strcat(output, "List of devices:\n.");
	while ( (entry = readdir(devices)) != NULL ) {
		if (strcmp(entry->d_name,".") == 0 || strcmp(entry->d_name,"..") == 0)
			continue;

		strcat(output, "├── ");
		strcat(output, entry->d_name);

		if ( (where = get_loc(entry->d_name)) != NULL) {
			strcat(output, "\t\t");
			strcat(output, where);
		}

		strcat(output, "\n");

		partitions_list(entry->d_name, output);
	}
	closedir(devices);
	return output;
}

void partitions_list(char * device_name, char * output) {
	DIR *partition_dir = opendir(concat("/sys/block/", device_name));
	struct dirent *entry;
	char * where;
	if (!partition_dir) {
		perror("Can not open device directory.");
		exit(1);
	}
	while ( (entry = readdir(partition_dir)) != NULL ) {
		if (strstr(entry->d_name, device_name) != NULL) {
			strcat(output, "│   ├── ");
			strcat(output, entry->d_name);
			if ( (where = get_loc(entry->d_name)) != NULL) {
				strcat(output, "\t");
				strcat(output, where);
			}
			strcat(output, "\n");
		}
	}
	closedir(partition_dir);
}

char * concat(char *s1, char *s2) {
	char *result = calloc(strlen(s1) + strlen(s2) + 1, 1);
	strcpy(result, s1);
	strcat(result, s2);
	return result;
}

void better_concat(char * s1, char * s2) {
	s1 = realloc(s1, strlen(s1) + strlen(s2) + 1);
	strcat(s1, s2);
}

static char * get_loc(char * device) {
	FILE * mounts = fopen("/proc/mounts", "r");
	char * buf = malloc(250);
	char * buf_cp;
	char * result;
	while ( (buf_cp = fgets(buf, 250, mounts)) != NULL) {
		if (strstr(buf, device) != NULL) {
			result = strtok(buf, " ");
			if (strcmp((result+5), device) != 0) //5 = /dev/
				continue;
			result = strtok(NULL, " ");
			return result;
			//printf("str - %s\n",result);
		}
	}
	return NULL;
}
