#include <cstddef>
#include <cstdio>
#include <iostream>
#include <cstring>
#include <vector>
#include "../../core/inc/reiser_structures.h"
#include "../../core/inc/util.h"

extern "C" {
	void display_usage(void);
	std::uint32_t get_dir(struct reiser_key, struct item_wrapper **);
	void read_meta(char *path_to_fs);
	char * print_working_dir(struct reiser_key);
	std::uint32_t change_dir(struct item_wrapper **,
			unsigned int *, char *, struct reiser_key *);
	char * get_file_by_name(char *, struct item_wrapper *, std::uint32_t);
	void copy(char *, char *, struct item_wrapper *, std::uint32_t);
	void list_devises(void);
	void read_meta(char *);
	int check_fs(void);
	void print_meta(void);
}

struct item_wrapper {
	std::uint32_t dir_id;
	std::uint32_t obj_id;
	char * name;
	Type type;
};

static void list_directory(struct item_wrapper * items, std::uint32_t inum) {
	for (int i = 0; i < inum; i++) {
		std::cout << items[i].name << std::endl;
	}
}

static void run_interactive() {
	bool stop_flag = false;
	char cmd[100];
	struct item_wrapper * current_dir = (struct item_wrapper *)malloc(1);
	struct reiser_key skey;
	skey.dir_id = 1;
	skey.obj_id = 2;
	std::uint32_t inum = get_dir(skey, &current_dir);
	while (!stop_flag) {
		std::cout << "lycoris-> ";
		std::cin.getline(cmd, sizeof(cmd));
		char * command = strtok(cmd, " ");
		if (strcmp(command, "exit") == 0) {
			stop_flag = true;
		} else if (strcmp(command, "pwd") == 0) {
			char * path = print_working_dir(skey);
			puts(path);
		} else if (strcmp(command, "ls") == 0) {
			list_directory(current_dir, inum);
		} else if (strcmp(command, "cd") == 0) {
			char * arg = strtok(NULL, " ");
			if (arg == NULL) {
				puts("Missing argument");
				continue;
			}
			if (!(change_dir(&current_dir, &inum, arg, &skey) == 0)) {
				puts("Change directory error");
				continue;
			}
		} else if (strcmp(command, "cp") == 0) {
			char * arg = strtok(NULL, " ");
			if (arg == NULL) {
				puts("Missing first argument");
				continue;
			}
			char * arg2 = strtok(NULL, " ");
			if (arg2 == NULL) {
				puts("Missing second argument");
				continue;
			}
			copy(arg, arg2, current_dir, inum);
		} else if (strcmp(command, "cat") == 0) {
			char * arg = strtok(NULL, " ");
			if (arg == NULL) {
				puts("Missing argument");
				continue;
			}
			puts(get_file_by_name(arg, current_dir, inum));
		} else {
			puts("Unknown command");
		}
	}
}

void initLycoris(char * fs) {
	read_meta(fs);
}

void get_list_devices(void) {
	list_devises();
}

int checkMagic(void) {
	return check_fs();
}

void printMeta(void) {
	print_meta();
}

int main() {
	initLycoris((char*)"./van.fs");
	run_interactive();
	list_devises();
}
