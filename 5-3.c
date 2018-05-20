#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
	int i, fd;
	int num;

	num = atoi(argv[2]);
	if(argc == 4 && (strcmp(argv[3], "x") == 0))
	{
		fd = open(argv[1], O_RDWR | O_CREAT);
		if(fd == -1)
			errExit("open1");
		for(i=0;i<num;i++)
		{
			if(lseek(fd, 0, SEEK_END)== -1)
				errExit("lseek");
			if(write(fd, "1", 1) == -1)
				errExit("write1");
		}
	}
	else if(argc == 3)
	{
		//printf("enter \n");
		fd = open(argv[1], O_RDWR | O_CREAT|O_APPEND);
		if(fd == -1)
			errExit("open2");
		for(i=0;i<num;i++)
		{
			//printf("write \n");
			if(write(fd, "1", 1) == -1)
				errExit("write2");
		}
	}
	else
	{
		if(strcmp(argv[1], "--help")==0)
			usageErr("%s filename num-bytes [x]\n", argv[0]);
	}

	exit(EXIT_SUCCESS);
}
	
