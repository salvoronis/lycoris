#include <getopt.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../core/inc/functions.h"
#include "../../core/inc/reiser.h"

#define OPTIONS_COUNT 2

static void disassembly_args(int, char**);
static void display_usage(void);
static void run_interactive(void);
static void list_directory(struct item_wrapper*, unsigned int);

typedef enum {false, true} Bool;

int main(int argc, char * argv[]) {
	disassembly_args(argc, argv);
}

static void run_interactive(void) {
	Bool stop_flag = false;
	char command[50];
	struct item_wrapper * current_dir = malloc(1);
	unsigned int cur_dir_id = 1;
	unsigned int inum = get_dir(cur_dir_id, &current_dir);
	/*for (int i = 0; i < inum; i++) {
		printf("item %d %s %u %u\n",
			i,
			current_dir[i].name,
			current_dir[i].dir_id,
			current_dir[i].obj_id
		);
	}*/
	while (!stop_flag) {
		printf("lycoris-> ");
		scanf("%s", command);
		if (strcmp(command, "exit") == 0 || strcmp(command, "q") == 0){
			stop_flag = true;
		} else if (strcmp(command, "pwd") == 0) {
			puts("/");
		} else if (strcmp(command, "ls") == 0) {
			list_directory(current_dir, inum);
		} else if (strcmp(command, "cd") == 0) {
			puts("cd");
		} else if (strcmp(command, "cp") == 0) {
			puts("cp");
		} else if (strcmp(command, "print_file") == 0) {
			puts("print");
		} else {
			puts("unknown command");
		}
	}
}

static void list_directory(struct item_wrapper * items, unsigned int inum) {
	for (int i = 0; i < inum; i++) {
		printf("%s\n", items[i].name);
	}
}

void disassembly_args(int argc, char * argv[]) {
	const char * shortFlags = "lhd:m:";
	const struct option longFlags[] = {
		{"list", no_argument, NULL, 'l'},
		{"help", no_argument, NULL, 'h'},
		{"device", required_argument, NULL, 'd'},
		{"meta", required_argument, NULL, 'm'},
	};
	int32_t rez;
	int32_t longid;
	while ((rez = getopt_long(argc, argv, shortFlags, longFlags, &longid)) != -1) {
	switch (rez) {
		case 'l':
			list_devises();
			break;
		case 'h':
			display_usage();
			break;
		case 'd':
			read_meta(optarg);
			if (check_fs() == 1){
				run_interactive();
			} else {
				puts("Not reiser");
			}
			break;
		case 'm':
			read_meta(optarg);
			print_meta();
			break;
	}
	}
}

struct help {
	int8_t require;
	char shortF;
	char *longF;
	char *description;
};

void display_usage() {
	struct help help_list[] = {
		{0, 'l', "list", "shows list of devices and partitions."},
		{0, 'h', "help", "show this message."},
	};
	for (int16_t i; i < OPTIONS_COUNT; i++) {
		printf("%s:\n\trequired: %d\n\tshort name: %c\n\tlong name: %s\n\tdescription: %s\n",
				help_list[i].longF,
				help_list[i].require,
				help_list[i].shortF,
				help_list[i].longF,
				help_list[i].description
				);
	}
}
