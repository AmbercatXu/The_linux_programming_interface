#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
	int rusfd = 0;
	int oldfd = atoi(argv[1]);
	int newfd = atoi(argv[2]);

	if(argc != 3 || strcmp(argv[1], "--help")== 0)
		usageErr("%s oldfd newfd\n", argv[0]);

	if(fcntl(oldfd, F_GETFL) == -1)
	{
		printf("EBADF \n");
		errno = EBADF;
		return -1;
	}

	if(oldfd == newfd)
	{
		return newfd;
	}
	close(newfd);
	rusfd = fcntl(oldfd, F_DUPFD, newfd);
	if(rusfd == -1)
		errExit("fcntl");
	printf("newfd is %d \n", rusfd);

}
