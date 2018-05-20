/*TEST 4-1 : tee-replace*/




#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "tlpi_hdr.h"

#define MAX_READ 20

int main(int argc, char const *argv[])
{
	size_t	numRead;
	char read_buf[MAX_READ];
	int	fd;

	if(strcmp(argv[1], "--help") == 0)
	{
		usageErr("%s [-a] file \n", argv[0]);
	}

	if(strcmp(argv[1], "-a") == 0)
	{
		fd = open(argv[2], O_WRONLY | O_APPEND);
	}
	else
	{
		fd = open(argv[1], O_WRONLY | O_CREAT);
	}
	
	if(fd == -1)
	{
		errExit("opening file");
	}

	while((numRead = read(STDIN_FILENO, read_buf, MAX_READ)) > 0)
	{
		if(write(fd, read_buf, numRead) != numRead)
		{
			fatal("couldn't write whole buffer \n");
		}
		if(write(STDOUT_FILENO, read_buf, numRead) != numRead)
		{
			fatal("couldn't write whole buffer \n");
		}
	}
	if( numRead == -1)
		errExit("read");

	if(close(fd) == -1)
		errExit("close file \n");
	
	exit(EXIT_SUCCESS);
}
