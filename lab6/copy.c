#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv) {
	//parse arguments
	int arg;
	bool help, mapMode;
	do {
		arg = getopt(argc, argv, "+hm");
		help |= arg == 'h';
		mapMode |= arg == 'm';
	} while (arg != -1);
	
	//print help if needed
	if (help || argc == 1) {
		puts("Usage:");
		puts("\tcopy [-m] <file_name> <new_file_name>");
		puts("\tcopy [-h]");
		puts("\nThe first syntax copies one file to another. The -m argument "
			"tells the program to use memory mapping. The second syntax "
			"displays the help text.");
		return 0;
	}
	
	//handle insufficient arguments
	if (argc - optind < 2) {
		fputs("Insufficient arguments\n", stderr);
		return 1;
	}
}
