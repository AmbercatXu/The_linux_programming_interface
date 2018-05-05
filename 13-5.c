#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <memory.h>

#define MAX_LINE 100
int linefeed = 0;

off_t checkitembyblock(int fd, int linenum)
{
	off_t curr   = 0;
	int index = 0;
	ssize_t readsize = 0;
	char buf[MAX_LINE];

	memset(buf, 0, MAX_LINE);

	curr = lseek(fd, -MAX_LINE, SEEK_CUR);
	if(curr == -1)
		return -1;
	readsize = read(fd, buf, MAX_LINE);
	if( readsize == -1)
		errExit("read");
	curr = lseek(fd, -MAX_LINE, SEEK_CUR);

	for(index = readsize; index > 0; index--)
	{
		if(memcmp(&buf[index - 1],"\n", 1)==0)
			linefeed++;
		if(linefeed == linenum)
		{
			curr += index - 1; 
			break;
		}
	}

	return curr;
}

off_t checkitembybytes(int fd, int linenum)
{
	off_t curr   = 0;
	char  buf[2];
	off_t start  = 0;
	off_t end  = 0;

	//end   = lseek(fd, 0, SEEK_END);

	while(linefeed < linenum)
	{
		memset(buf, 0, 2);

		curr = lseek(fd, -1, SEEK_CUR);
		if(curr == -1)
			return curr;

		if(read(fd, buf, 1) == -1)
			errExit("read");
		if(strcmp(buf,"\n")==0)
			linefeed++;

		curr = lseek(fd, -1, SEEK_CUR);
		if(curr == -1)
			return curr;
	}

	return curr;
}

int
main(int argc, char *argv[])
{
	int linenum  = 10;
	off_t curr   = 0;
	off_t start  = 0;
	int fd;
	char* file;

	char buf[MAX_LINE];
	ssize_t size = 0;
	char *bufout;
	int index = 0;
	ssize_t readsize = 0;

	memset(buf, 0, MAX_LINE);

	if(argc == 4 && strcmp(argv[1],"--n"))
	{
		linenum = atol(argv[2]);
		file = argv[3];
	}
	else if(argc == 2)
	{
		file = argv[1];
	}
	else
		usageErr("%s [-n num] file\n",argv[0]);

	fd = open(file, O_RDONLY);
	if(fd == -1)
		errExit("open");
	
	curr  = lseek(fd, -1, SEEK_END);
	bufout = malloc(1);
	if(read(fd, bufout, 1) == -1)
		errExit("read");
	if(memcmp(bufout,"\n", 1)==0)
		linenum++;

	curr  = lseek(fd, 0, SEEK_END);
	size  = curr;

	while(linefeed < linenum)
	{	
		curr = checkitembyblock(fd, linenum);
		if( curr == -1)
			curr = checkitembybytes(fd, linenum);

		if(curr == -1)
			break;
	}

	if(curr != -1)
		size = size - curr;
	curr = lseek(fd, curr+1, SEEK_SET);
	bufout = malloc(size);
	if(read(fd, bufout, size) == -1)
		errExit("read");
	write(STDOUT_FILENO, bufout, size);

	free(bufout);
	if(close(fd) == -1)
		errExit("close");
}