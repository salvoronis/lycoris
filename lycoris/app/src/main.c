#include <getopt.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../core/inc/functions.h"
#include "../../core/inc/reiser.h"
#include "../../core/inc/directory.h"

#define OPTIONS_COUNT 2

static void disassembly_args(int, char**);
static void display_usage(void);
static void run_interactive(void);

typedef enum {false, true} Bool;

int main(int argc, char * argv[]) {
	disassembly_args(argc, argv);
}

static void run_interactive(void) {
	Bool stop_flag = false;
	char cmd[100];
	struct item_wrapper * current_dir = malloc(1);
	int32_t cur_dir_id = 1;
	int32_t cur_obj_id = 2;
	unsigned int inum = get_dir(cur_dir_id, cur_obj_id, &current_dir);
	while (!stop_flag) {
		printf("lycoris-> ");
		fgets(cmd, sizeof(cmd), stdin);
		for (int i = 0; i < sizeof(cmd); i++) {
			if (cmd[i] == '\n'){
				cmd[i] = '\0';
				break;
			}
		}
		char *command = strtok(cmd, " ");
		if (strcmp(command, "exit") == 0 || strcmp(command, "q") == 0){
			stop_flag = true;
		} else if (strcmp(command, "pwd") == 0) {
			print_working_dir(cur_dir_id, cur_obj_id);
		} else if (strcmp(command, "ls") == 0) {
			list_directory(current_dir, inum);
		} else if (strcmp(command, "cd") == 0) {
			char * arg = strtok(NULL, " ");
			if (arg == NULL) {
				puts("missing argument");
				continue;
			}
			if (!(change_dir(&current_dir,
					&inum,
					arg,
					&cur_dir_id,
					&cur_obj_id) == 0))
				puts("change directory error");
		} else if (strcmp(command, "cp") == 0) {
			puts("cp");
		} else if (strcmp(command, "cat") == 0) {
			puts("print");
		} else {
			puts("unknown command");
		}
	}
}

/**
 * receive current dir's dir and obj id
 * */
/*static void print_working_dir(int32_t dir_id, int32_t obj_id){
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
}*/

/*static unsigned int change_dir(
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
}*/

/*static void list_directory(struct item_wrapper * items, unsigned int inum) {
	for (int i = 0; i < inum; i++) {
		printf("%s\n", items[i].name);
	}
}*/

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
