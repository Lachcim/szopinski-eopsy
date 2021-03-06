#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

int copyReadWrite(int, int);
int copyMmap(int, int);

const char* const ERROR_MESSAGES[] = {
	0,
	"Insufficient arguments\n",
	"Failed to open the source file\n",
	"Failed to open the destination file\n",
	"Failed to read from the source file\n",
	"Failed to write to the destination file\n",
	"Error while seeking source file\n",
	"Failed to map source file\n",
	"Failed to map destination file\n"
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
	
	//open source and destination files
	int srcFd = open(argv[optind], O_RDONLY);
	if (srcFd == -1) return 2;
	int destFd = open(argv[optind + 1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	if (destFd == -1) return 3;
	
	//call the proper copying subroutine
	int error = 0;
	if (mapMode) error = copyMmap(srcFd, destFd);
	else error = copyReadWrite(srcFd, destFd);
	
	//close file handles
	close(srcFd);
	close(destFd);
	
	//print error message and return error code
	if (error) fputs(ERROR_MESSAGES[error], stderr);
	return error;
}

int copyReadWrite(int src, int dest) {
	//copy data
	char buf[1024];
	while (true) {
		//read bytes from source
		ssize_t byteCount = read(src, buf, 1024);
		if (byteCount == -1) return 4;
		
		//write bytes to destination
		byteCount = write(dest, buf, byteCount);
		if (byteCount == -1) return 5;
		
		//break when done
		if (byteCount != 1024)
			break;
	}
	
	return 0;
}

int copyMmap(int src, int dest) {
	//obtain source file size
	off_t size = lseek(src, 0, SEEK_END);
	if (size == -1) return 6;
	lseek(src, 0, SEEK_SET);
	
	//map source file onto memory
	char* srcMap = mmap(0, size, PROT_READ, MAP_PRIVATE, src, 0);
	if (srcMap == MAP_FAILED) return 7;
	
	//map destination file, ensure the file is long enough to store source
	ftruncate(dest, size);
	char* dstMap = mmap(0, size, PROT_WRITE, MAP_SHARED, dest, 0);
	if (dstMap == MAP_FAILED) return 8;
	
	//copy memory
	memcpy(dstMap, srcMap, size);
	
	//unmap memory
	munmap(srcMap, size);
	munmap(dstMap, size);
	
	return 0;
}
