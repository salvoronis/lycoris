#include <getopt.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../core/inc/functions.h"
#include "../../core/inc/reiser.h"
#include "../../core/inc/directory.h"
#include "../../core/inc/file.h"

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
			char * arg = strtok(NULL, " ");
			if (arg == NULL) {
				puts("missing first argument");
				continue;
			}
			char * arg2 = strtok(NULL, " ");
			if (arg == NULL) {
				puts("missing second argument");
				continue;
			}
			FILE * cpto = fopen(arg2, "w+");
			char * data = get_file_by_name(arg, current_dir, inum);
			fwrite(data, strlen(data), 1, cpto);
			fclose(cpto);
		} else if (strcmp(command, "cat") == 0) {
			char * arg = strtok(NULL, " ");
			if (arg == NULL) {
				puts("missing argument");
				continue;
			}
			puts(get_file_by_name(arg, current_dir, inum));
		} else {
			puts("unknown command");
		}
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
