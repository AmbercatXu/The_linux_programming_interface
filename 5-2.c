#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
	int fd;

	if(argc != 2||strcmp(argv[1], "--help")==0)
		usageErr("%s pathname \n", argv[0]);

	fd = open(argv[1], O_RDWR | O_CREAT|O_APPEND);
	if(fd == -1)
		errExit("open");
	
	if(lseek(fd, 0, SEEK_SET)== -1)
		errExit("lseek");
	
	if(write(fd, "test", 4) == -1)
		errExit("write");

	exit(EXIT_SUCCESS);
}
	
