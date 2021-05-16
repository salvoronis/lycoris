#include <cstddef>
#include <cstdio>
#include <iostream>
#include <cstring>
#include <vector>
#include <napi.h>
#include "../../core/inc/reiser_structures.h"
#include "../../core/inc/util.h"
#include "bindings.h"

using namespace Napi;
using namespace std;

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

Object change_dir_wrapper(const CallbackInfo &info) {
	if (info[0].IsArray() && info[1].IsNumber()
		&& info[2].IsString() && info[3].IsObject()){

		Array it_w = info[0].As<Array>();
		struct item_wrapper * i_w = 
			(struct item_wrapper*) malloc(
				sizeof(struct item_wrapper) * (it_w.Length())
			);
		for (int i = 0; i < it_w.Length(); ++i){
			Object tmp = ((Value)it_w[i]).ToObject();
			i_w[i] = {
				(std::uint32_t)tmp.Get("dir_id").ToNumber(),
				(std::uint32_t)tmp.Get("obj_id").ToNumber(),
				&((std::string)tmp.Get("namee").ToString())[0]
			};
			//cout << &((std::string)tmp.Get("namee").ToString())[0] << endl;
		}

		uint32_t inum = (uint32_t)info[1].ToNumber();
		char * name = &((string)info[2].ToString())[0];

		Object skey_o = info[3].ToObject();
		struct reiser_key skeys = {
			(std::uint32_t)skey_o.Get("dir_id").ToNumber(),
			(std::uint32_t)skey_o.Get("obj_id").ToNumber()
		};

		if (!(change_dir(&i_w, &inum, name, &skeys) == 0)){
			return Object();
		}

		Env env = info.Env();
		Object ret_tmp = Object::New(env);
		Array ret_items = Array::New(env);
		for (int i = 0; i < inum; ++i){
			Object obj = Object::New(env);
			obj.Set("dir_id", i_w[i].dir_id);
			obj.Set("obj_id", i_w[i].obj_id);
			obj.Set("namee", i_w[i].name);
			ret_items[i] = obj;
		}

		Object skey_ro = Object::New(env);
		skey_ro.Set("dir_id", skeys.dir_id);
		skey_ro.Set("obj_id", skeys.obj_id);
		ret_tmp.Set("skey", skey_ro);

		ret_tmp.Set("item_wrapper", ret_items);

		ret_tmp.Set("inum", inum);

		return ret_tmp;
	}
	return Object();
}

Object get_dir_wrapper(const CallbackInfo &info) {
	if (info[0].IsObject() && info[1].IsArray()) {

		Object skey_o = info[0].ToObject();
		struct reiser_key skeys = {
			(std::uint32_t)skey_o.Get("dir_id").ToNumber(),
			(std::uint32_t)skey_o.Get("obj_id").ToNumber()
		};

		Array it_w = info[1].As<Array>();
		struct item_wrapper * i_w = 
			(struct item_wrapper*) malloc(
				sizeof(struct item_wrapper) * (it_w.Length())
			);
		for (int i = 0; i < it_w.Length(); ++i){
			Object tmp = ((Value)it_w[i]).ToObject();
			i_w[i] = {
				(std::uint32_t)tmp.Get("dir_id").ToNumber(),
				(std::uint32_t)tmp.Get("obj_id").ToNumber(),
				&((std::string)tmp.Get("namee").ToString())[0]
				//(Type)((std::uint32_t)tmp.Get("type").ToNumber())
			};
		}


		uint32_t inum = get_dir(skeys, &i_w);


		Env env = info.Env();
		Object ret_tmp = Object::New(env);
		Array ret_items = Array::New(env);
		for (int i = 0; i < inum; ++i){
			Object obj = Object::New(env);
			obj.Set("dir_id", i_w[i].dir_id);
			obj.Set("obj_id", i_w[i].obj_id);
			obj.Set("namee", i_w[i].name);
			//obj.Set("type", (uint32_t)i_w[i].type);
			//cout << "shit > " << i_w[i].type << endl;
			ret_items[i] = obj;
		}
		ret_tmp.Set("item_wrapper", ret_items);
		ret_tmp.Set("inum", inum);


		return ret_tmp;
	}
	return Object();
}

String print_working_dir_wrapper(const CallbackInfo &info) {
	Env env = info.Env();
	if (info[0].IsObject()){
		Object skey_o = info[0].ToObject();
		struct reiser_key skey = {
			(std::uint32_t)skey_o.Get("dir_id").ToNumber(),
			(std::uint32_t)skey_o.Get("obj_id").ToNumber()
		};
		return String::New(env, print_working_dir(skey));
	}
	return String::New(env, "oops");
}

static void fucking_test_shit(const CallbackInfo &info) {
	Value val = info[0];
	//std::cout << val.IsObject() <<std::endl;
	if (val.IsArray()) {
		std::cout << "array" << std::endl;
		Array fucking_shit = val.As<Array>();//Napi::Array(val.Env());
		std::uint32_t cho = 1;
		Object obj = ((Value)fucking_shit[cho]).ToObject();
		std::uint32_t shit = obj.Get("dir_id").ToNumber();
		std::cout << shit << std::endl;
	} else if (val.IsObject()) {
		Object fucking_item = val.ToObject();
		std::uint32_t shit = fucking_item.Get("dir_id").ToNumber();
		std::cout << shit << std::endl;
	}
}

static void list_directory(struct item_wrapper * items, std::uint32_t inum) {
	for (unsigned int i = 0; i < inum; i++) {
		std::cout << items[i].name << std::endl;
	}
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


Napi::Object Init(Napi::Env env, Napi::Object exports) {
	exports.Set(Napi::String::New(env, "checkMagic"), Napi::Function::New(env, checkMagic));
	exports.Set(Napi::String::New(env, "printMeta"), Napi::Function::New(env, printMeta));
	exports.Set(Napi::String::New(env, "initLycoris"), Napi::Function::New(env, initLycoris));
	exports.Set(Napi::String::New(env, "listDevises"), Napi::Function::New(env, listDevises));
	exports.Set(Napi::String::New(env, "get_some_struct"), Napi::Function::New(env, get_some_struct));
	exports.Set(Napi::String::New(env, "fucking_test_shit"), Napi::Function::New(env, fucking_test_shit));
	exports.Set(Napi::String::New(env, "getDir"), Napi::Function::New(env, get_dir_wrapper));
	exports.Set(Napi::String::New(env, "printWorkingDir"), Napi::Function::New(env, print_working_dir_wrapper));
	exports.Set(Napi::String::New(env, "changeDir"), Napi::Function::New(env, change_dir_wrapper));
	return exports;
}

NODE_API_MODULE(hello_world, Init);








/*struct item_wrapper * curnode;

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
}*/

/*Napi::Object get_curnode(const Napi::CallbackInfo &info) {
	struct item_wrapper tmp = {12,12,"anime",(Type)1};
	Napi::Env env = info.Env();
	Napi::Object ret_tmp = Napi::Object::New(env);
	
	
	ret_tmp.Set("dir_id", Napi::Number::New(env, (tmp.dir_id)));
	ret_tmp.Set("obj_id", Napi::Number::New(env, (tmp.obj_id)));
	ret_tmp.Set("name", Napi::String::New(env, (tmp.name)));
	ret_tmp.Set("type", Napi::Number::New(env, (tmp.type)));
	return (Napi::Object)ret_tmp;
}*/

//std::uint32_t inum = get_dir(skey, &current_dir);
/* Napi::Object get_dir_wrapper(const Napi::CallbackInfo &info) {

} */

/*static Napi::String print_working_dir_wrapper(const Napi::CallbackInfo &info){
	return print_working_dir();
}*/

/*static void run_interactive() {
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
}*/