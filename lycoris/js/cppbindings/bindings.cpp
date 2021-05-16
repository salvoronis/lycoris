#include <cstddef>
#include <cstdio>
#include <iostream>
#include <cstring>
#include <vector>
#include <napi.h>
#include "../../core/inc/reiser_structures.h"
#include "../../core/inc/util.h"
#include "bindings.h"

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

struct item_wrapper * curnode;

static void initWrapper(const Napi::CallbackInfo &info) {
	std::uint32_t times = (std::uint32_t)info[0].ToNumber();
	curnode = (struct item_wrapper*) malloc(sizeof(struct item_wrapper) * times);
}

static void toItemWrapper(const Napi::CallbackInfo &info) {
	std::uint32_t iter = (std::uint32_t)info[4].ToNumber();
	curnode[iter].dir_id = (std::uint32_t)info[0].ToNumber();
	curnode[iter].obj_id = (std::uint32_t)info[1].ToNumber();
	curnode[iter].name = &((std::string)info[2].ToString())[0];
	curnode[iter].type = (Type)(std::uint32_t)info[3].ToNumber();
}

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

Napi::String listDevises(const Napi::CallbackInfo &info) {
	Napi::String list = Napi::String::New(info.Env(), list_devises());
}

//std::uint32_t inum = get_dir(skey, &current_dir);
/* Napi::Object get_dir_wrapper(const Napi::CallbackInfo &info) {

} */

Napi::Object get_some_struct(const Napi::CallbackInfo &info) {
	struct item_wrapper tmp = {12,12,"anime",(Type)1};
	Napi::Env env = info.Env();
	Napi::Object ret_tmp = Napi::Object::New(env);
	
	
	ret_tmp.Set("dir_id", Napi::Number::New(env, (tmp.dir_id)));
	ret_tmp.Set("obj_id", Napi::Number::New(env, (tmp.obj_id)));
	ret_tmp.Set("name", Napi::String::New(env, (tmp.name)));
	ret_tmp.Set("type", Napi::Number::New(env, (tmp.type)));
	return (Napi::Object)ret_tmp;
}






/*ItemWrapWrapper::ItemWrapWrapper(const Napi::CallbackInfo &info) : ObjectWrap(info) {
	Napi::Env env = info.Env();

	if (info.Length() < 1){
		Napi::TypeError::New(env, "Wrong number of arguments")
          .ThrowAsJavaScriptException();
        return;
	}

	if (!info[0].IsString()) {
        Napi::TypeError::New(env, "You need to name yourself")
          .ThrowAsJavaScriptException();
        return;
    }

	this->_greeterName = info[0].As<Napi::String>().Utf8Value();
}

ItemWrapWrapper::ItemWrapWrapper(Napi::Object){
	std::cout << "fuckfuckfuck" << std::endl;
	return Null;
}

Napi::Value ItemWrapWrapper::Greet(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();

	if (info.Length() < 1) {
        Napi::TypeError::New(env, "Wrong number of arguments")
          .ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "You need to introduce yourself to greet")
          .ThrowAsJavaScriptException();
        return env.Null();
    }

	Napi::String name = info[0].As<Napi::String>();

    printf("Hello %s\n", name.Utf8Value().c_str());
    printf("I am %s\n", this->_greeterName.c_str());

    return Napi::String::New(env, this->_greeterName);
}

std::string ItemWrapWrapper::getName(){
	return this->_greeterName;
}

void getSomeShit(const Napi::CallbackInfo &info){
	ItemWrapWrapper item = (ItemWrapWrapper)info[0].ToObject();//.As<ItemWrapWrapper>();
	std::cout << item.getName() << std::endl;
}

Napi::Function ItemWrapWrapper::GetClass(Napi::Env env) {
	return DefineClass(env, "ItemWrapWrapper", {
		ItemWrapWrapper::InstanceMethod("greet", &ItemWrapWrapper::Greet),
	});
}*/










Napi::Object Init(Napi::Env env, Napi::Object exports) {
	exports.Set(Napi::String::New(env, "checkMagic"), Napi::Function::New(env, checkMagic));
	exports.Set(Napi::String::New(env, "printMeta"), Napi::Function::New(env, printMeta));
	exports.Set(Napi::String::New(env, "initLycoris"), Napi::Function::New(env, initLycoris));
	exports.Set(Napi::String::New(env, "runInteractive"), Napi::Function::New(env, runInteractive));
	exports.Set(Napi::String::New(env, "listDevises"), Napi::Function::New(env, listDevises));
	exports.Set(Napi::String::New(env, "get_some_struct"), Napi::Function::New(env, get_some_struct));
	exports.Set(Napi::String::New(env, "initWrapper"), Napi::Function::New(env, initWrapper));
	exports.Set(Napi::String::New(env, "toItemWrapper"), Napi::Function::New(env, toItemWrapper));
	//exports.Set(Napi::String::New(env, "ItemWrapWrapper"), ItemWrapWrapper::GetClass(env));
	return exports;
}

NODE_API_MODULE(hello_world, Init);