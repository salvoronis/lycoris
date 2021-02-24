#include "../inc/functions.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void list_devises() {
	DIR *devices = opendir("/sys/block");
	struct dirent *entry;
	if (!devices) {
		perror("Can not open /sys/block");
		exit(1);
	}
	puts("list of devices:");
	while ( (entry = readdir(devices)) != NULL ) {
		if (strcmp(entry->d_name,".") == 0 || strcmp(entry->d_name,"..") == 0)
			continue;
		printf("\t->%s\n",
				entry->d_name);
	}
	closedir(devices);
}
