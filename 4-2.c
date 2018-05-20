/*TEST 4-2 : cp-replace*/




#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "tlpi_hdr.h"

#define MAX_READ 1024

int main(int argc, char const *argv[])
{
	size_t	numRead;
	char read_buf[MAX_READ];
	int	src_fd, dest_fd, have_holes, index, write_point = 0, write_parse = 0;
	struct stat st;

	if(argc != 3 || strcmp(argv[1], "--help") == 0)
	{
		usageErr("%s SOURCE DEST \n", argv[0]);
	}

	src_fd = open(argv[1], O_RDONLY);
	if(src_fd == -1)
	{
		errExit("opening source file");
	}

	dest_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC);
	if(dest_fd == -1)
	{
		errExit("opening dest file");
	}

	if(fstat(src_fd, &st) != 0)
	{
		errExit("fstat");
	}
	else
	{
		if(S_ISREG(st.st_mode)&&st.st_size > st.st_blksize * st.st_blocks)
		{
			have_holes = 1;
			printf("%s is a sparse-block file!\n", argv[1]);
		}
		else
		{
			have_holes = 0;
			printf("%s is not a sparse-block file!\n", argv[1]);
		}
	}
	if( have_holes == 0 )
	{
		while((numRead = read(src_fd, read_buf, st.st_blksize)) > 0)
		{
			if(write(dest_fd, read_buf, numRead) != numRead)
			{
				fatal("couldn't write whole buffer \n");
			}
		}
		if( numRead == -1)
			errExit("read");
	}
	else
	{
		while((numRead = read(src_fd, read_buf, st.st_blksize)) > 0)
		{
			for(index = 0; index < numRead; index++)
			{
				if(read_buf[index] ！= 0)
					break;
			}
			if(index != numRead)
			{
				if(write(dest_fd, read_buf, numRead) != numRead)
				{
					fatal("couldn't write whole buffer \n");
				}				
			}
			else
			{
				lseek(dest_fd, numRead, SEEK_CUR);
			}
		}

	}
	if(close(src_fd) == -1)
		errExit("close file \n");
	if(close(dest_fd) == -1)
	errExit("close file \n");

	exit(EXIT_SUCCESS);
}