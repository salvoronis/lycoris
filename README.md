# lycoris
lycoris is a program for basic serving of reiser file system (ver 3.5 and 3.6) like read files and copy them, unfortunatelly lycoris can't create or uptade files. Moreover lycoris can get all connected devices and they's path. **lycoris works only on linux.**
There are 2 types of lycoris: **C** and **Node js** versions. And in C there are 2 parts: **cli app** and **core**. `./lycoris/app` stands for **C cli app**, `./lycoris/core` stands for **core** and `./lycoris/js` stands for **node js application and C++ wrapper**. 

## Build and Run
lycoris has a makefile, so building project is an easy task.
```sh
make subdirs
make
```
For running application you have to choose application mode, you can choose it by flags:

1. `--list` or `-l` shows list of devices and partitions.
2. `--help` or `-h` shows all avialable flags.
3. `--device [path_to_fs]` or `-d [path_to_fs]` start interactive mode.
4. `--meta [path_to_fs]` or `-m [path_to_fs]` print superblock.

## Project structure

```text
.
├── lycoris
│   ├── app
│   │   ├── inc
│   │   │   └── colors.h
│   │   └── src
│   │       └── main.c
│   ├── core
│   │   ├── inc
│   │   │   ├── btree.h
│   │   │   ├── directory.h
│   │   │   ├── file.h
│   │   │   ├── functions.h
│   │   │   ├── linked_list.h
│   │   │   ├── reiser.h
│   │   │   ├── reiser_structures.h
│   │   │   └── util.h
│   │   └── src
│   │       ├── btree.c
│   │       ├── directory.c
│   │       ├── file.c
│   │       ├── linked_list.c
│   │       ├── list.c
│   │       ├── reiser.c
│   │       └── util.c
│   └── js
│       ├── binding.gyp
│       ├── cppbindings
│       │   └── bindings.cpp
│       ├── lycoris.js
│       ├── package.json
│       └── package-lock.json
├── Makefile
└── README.md

```

---
I use arch btw