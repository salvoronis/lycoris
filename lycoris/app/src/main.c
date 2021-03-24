#include <getopt.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "../../core/inc/functions.h"
#include "../../core/inc/reiser.h"

#define OPTIONS_COUNT 2

static void disassembly_args(int, char**);
static void display_usage(void);

int main(int argc, char * argv[]) {
	disassembly_args(argc, argv);
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
				puts("OK");
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
