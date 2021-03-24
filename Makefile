FILEFS = van.fs
MOUNTDIR = /mnt/spo
SUBDIRS = lycoris/app lycoris/core lycoris/app/obj lycoris/app/inc lycoris/app/src lycoris/core/obj core/inc lycoris/core/src build
CC = gcc

all: core_prt app_prt
	gcc -o build/lycoris lycoris/core/obj/*.o lycoris/app/obj/app_prt.o

subdirs: $(SUBDIRS)

.PHONY: $(SUBDIRS) ./lycoris/core/src/main.c ./lycoris/app/src/main.c

.IGNORE: $(SUBDIRS)

$(SUBDIRS):
	mkdir $@

mountfs:
	sudo mount -t reiserfs -o loop $(FILEFS) $(MOUNTDIR)

core_prt: list.o reiser.o

list.o: ./lycoris/core/src/list.c
	$(CC) -o ./lycoris/core/obj/$@ -c $^

reiser.o: ./lycoris/core/src/reiser.c
	$(CC) -o ./lycoris/core/obj/$@ -c $^

app_prt: ./lycoris/app/src/main.c
	$(CC) -o ./lycoris/app/obj/$@.o -c $^

create_fs: $(FILEFS)
	touch $(FILEFS)
	sudo dd if=/dev/zero of=$(FILEFS) bs=1M count=64
	make mountfs
