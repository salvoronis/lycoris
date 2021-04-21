#include <cstddef>
#include <cstdio>
#include <iostream>
#include <cstring>
#include <vector>
#include <napi.h>
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
	char * list_devises(void);
	int check_fs(void);
	char * print_meta(void);
}

struct item_wrapper {
	std::uint32_t dir_id;
	std::uint32_t obj_id;
	char * name;
	Type type;
};

static void list_directory(struct item_wrapper * items, std::uint32_t inum) {
	for (unsigned int i = 0; i < inum; i++) {
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

void runInteractive(const Napi::CallbackInfo &info) {
	run_interactive();
}

void initLycoris(const Napi::CallbackInfo &info) {
	std::string fsStr = (std::string) info[0].ToString();
	char * fs;
	fs = &fsStr[0];
	read_meta(fs);
}

Napi::Number checkMagic(const Napi::CallbackInfo &info) {
	return Napi::Number::New(info.Env(), check_fs());
}

Napi::String printMeta(const Napi::CallbackInfo &info) {
	Napi::String meta = Napi::String::New(info.Env(), print_meta());
	return meta;
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
	exports.Set(Napi::String::New(env, "checkMagic"), Napi::Function::New(env, checkMagic));
	exports.Set(Napi::String::New(env, "printMeta"), Napi::Function::New(env, printMeta));
	exports.Set(Napi::String::New(env, "initLycoris"), Napi::Function::New(env, initLycoris));
	exports.Set(Napi::String::New(env, "runInteractive"), Napi::Function::New(env, runInteractive));
	return exports;
}

NODE_API_MODULE(hello_world, Init);