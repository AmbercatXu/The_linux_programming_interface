#include <sys/stat.h>
#include <fcntl.h>
#include <sys/uio.h> 
#include <stdlib.h>
#include "tlpi_hdr.h"

#define STR_SIZE 10
#define MAX_READ 100
ssize_t rdvrep(int fd, const struct iovec *iov, int iovcnt)
{
	int i;
	ssize_t totalsize = 0;
	ssize_t tempsize = 0;

	for(i = 0; i<iovcnt; i++)
	{
		tempsize = read(fd, iov[i].iov_base, iov[i].iov_len);
		printf("tempsize %d \n", tempsize);
		if(tempsize > 0)
			totalsize += tempsize;
		else
			return tempsize;
	} 
	return totalsize;
}

ssize_t wrvrep(int fd, const struct iovec *iov, int iovcnt)
{
	int i;
	ssize_t totalsize = 0;
	ssize_t tempsize = 0;

	for(i = 0; i<iovcnt; i++)
	{
		tempsize = write(fd, iov[i].iov_base, iov[i].iov_len);
		printf("tempsize %d \n", tempsize);
		if(tempsize > 0)
			totalsize += tempsize;
		else
			return tempsize;
	} 
	return totalsize;
}

int main(int argc, char *argv[])
{
	int fd;
	struct iovec iov[3];
	int x;
	char str[STR_SIZE];
	int y;
	ssize_t numRead, totRequired;

	if(argc != 2 || strcmp(argv[1], "--help")== 0)
		usageErr("%s pathname \n", argv[0]);

	fd = open(argv[1], O_RDWR);
	if(fd == -1)
		errExit("open");

	totRequired = 0;

	iov[0].iov_base = &x;
	iov[0].iov_len = sizeof(x);
	totRequired += iov[0].iov_len;

	iov[1].iov_base = &str;
	iov[1].iov_len = STR_SIZE;
	totRequired += iov[1].iov_len;

	iov[2].iov_base = &y;
	iov[2].iov_len = sizeof(y);
	totRequired += iov[2].iov_len;

	numRead = rdvrep(fd, iov, 3);
	if(numRead == -1)
		errExit("rdvrep");

	if(numRead < totRequired)
		printf("Read fewer bytes than requested \n");

	printf("total bytes requested: %ld; bytes read: %ld\n", (long)totRequired, (long)numRead);
	
	x = 3434;
	strcpy(str, "ztztztztz");
	y = 3535;
	
	wrvrep(fd, iov, 3);
	close(fd);
	exit(EXIT_SUCCESS);
}

