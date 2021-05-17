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
	uint32_t dir_id;
	uint32_t obj_id;
	char * name;
	Type type;
};

void copy_wrapper(const CallbackInfo &info){
	if (info[0].IsString() && info[1].IsString()
		&& info[2].IsArray() && info[3].IsNumber()){

		Array it_w = info[2].As<Array>();
		struct item_wrapper * i_w = 
			(struct item_wrapper*) malloc(
				sizeof(struct item_wrapper) * (it_w.Length())
			);
		for (int i = 0; i < it_w.Length(); ++i){
			Object tmp = ((Value)it_w[i]).ToObject();
			i_w[i] = {
				(std::uint32_t)tmp.Get("dir_id").ToNumber(),
				(std::uint32_t)tmp.Get("obj_id").ToNumber(),
			};
			i_w[i].name = (char *) calloc(((std::string)tmp.Get("namee").ToString()).size(), 1);
			memcpy(i_w[i].name,
				&((std::string)tmp.Get("namee").ToString())[0],
				((std::string)tmp.Get("namee").ToString()).size());
		}

		uint32_t inum = (uint32_t) info[3].ToNumber();

		string src_s = (string)info[0].ToString();
		char * src = (char *) calloc(src_s.size(), 1);
		memcpy(src, &(src_s)[0], src_s.size());

		string dst_s = (string)info[1].ToString();
		char * dst = (char *) calloc(dst_s.size(), 1);
		memcpy(dst, &(dst_s)[0], dst_s.size());
		copy(src, dst, i_w, inum);
	}
}

String get_file_by_name_wrapper(const CallbackInfo &info) {
	Env env = info.Env();
	if (info[0].IsString() && info[1].IsArray() && info[2].IsNumber()) {
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
			};
			i_w[i].name = (char *) calloc(((std::string)tmp.Get("namee").ToString()).size(), 1);
			memcpy(i_w[i].name,
				&((std::string)tmp.Get("namee").ToString())[0],
				((std::string)tmp.Get("namee").ToString()).size());
		}

		uint32_t inum = (uint32_t) info[2].ToNumber();

		string src_s = (string)info[0].ToString();
		char * src = (char *) calloc(src_s.size(), 1);
		memcpy(src, &(src_s)[0], src_s.size());

		return String::New(env, get_file_by_name(src, i_w, inum));
	}
	return String::New(env, "Oh sht I'm sorry");
}

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
			};
			i_w[i].name = (char *) calloc(((std::string)tmp.Get("namee").ToString()).size(), 1);
			memcpy(i_w[i].name,
				&((std::string)tmp.Get("namee").ToString())[0],
				((std::string)tmp.Get("namee").ToString()).size());
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


Napi::Object Init(Napi::Env env, Napi::Object exports) {
	exports.Set(Napi::String::New(env, "checkMagic"), Napi::Function::New(env, checkMagic));
	exports.Set(Napi::String::New(env, "printMeta"), Napi::Function::New(env, printMeta));
	exports.Set(Napi::String::New(env, "initLycoris"), Napi::Function::New(env, initLycoris));
	exports.Set(Napi::String::New(env, "listDevises"), Napi::Function::New(env, listDevises));
	exports.Set(Napi::String::New(env, "getDir"), Napi::Function::New(env, get_dir_wrapper));
	exports.Set(Napi::String::New(env, "printWorkingDir"), Napi::Function::New(env, print_working_dir_wrapper));
	exports.Set(Napi::String::New(env, "changeDir"), Napi::Function::New(env, change_dir_wrapper));
	exports.Set(Napi::String::New(env, "copy"), Napi::Function::New(env, copy_wrapper));
	exports.Set(Napi::String::New(env, "getFileByName"), Napi::Function::New(env, get_file_by_name_wrapper));
	return exports;
}

NODE_API_MODULE(hello_world, Init);