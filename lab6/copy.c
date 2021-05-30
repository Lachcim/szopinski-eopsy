#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int copyReadWrite(const char*, const char*);
int copyMmap(const char*, const char*);

const char* const ERROR_MESSAGES[] = {
	0,
	"Insufficient arguments\n",
	"Failed to open the source file\n",
	"Failed to open the destination file\n",
	"Failed to read from the source file\n",
	"Failed to write to the destination file\n"
};

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
		fputs(ERROR_MESSAGES[1], stderr);
		return 1;
	}
	
	//call the proper copying subroutine
	int error = 0;
	if (mapMode)
		error = copyMmap(argv[optind], argv[optind + 1]);
	else
		error = copyReadWrite(argv[optind], argv[optind + 1]);
	
	//print error message and return error code
	if (error) fputs(ERROR_MESSAGES[error], stderr);
	return error;
}

int copyReadWrite(const char* src, const char* dest) {
	//open source file
	int srcFd = open(src, O_RDONLY);
	if (srcFd == -1) return 2;
	
	//open destination file
	int destFd = open(dest, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
	if (destFd == -1) return 3;
	
	//copy data
	char buf[1024];
	while (true) {
		//read bytes from source
		ssize_t byteCount = read(srcFd, buf, 1024);
		if (byteCount == -1) return 4;
		
		//write bytes to destination
		byteCount = write(destFd, buf, byteCount);
		if (byteCount == -1) return 5;
		
		//break when done
		if (byteCount != 1024)
			break;
	}
	
	//close file handles and return
	close(srcFd);
	close(destFd);
	return 0;
}

int copyMmap(const char* src, const char* dest) {
	return 0;
}
