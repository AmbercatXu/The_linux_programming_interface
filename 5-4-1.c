#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"



int main(int argc, char *argv[])
{
	int newfd = 0;
	int oldfd = atoi(argv[1]);

	if(argc != 2 || strcmp(argv[1], "--help")== 0)
		usageErr("%s oldfd \n", argv[0]);

	if(fcntl(oldfd, F_GETFL) == -1)
	{
		printf("EBADF \n");
		errno = EBADF;
		return -1;
	}
	newfd = fcntl(oldfd, F_DUPFD, 0);
	if(newfd == -1)
		errExit("fcntl");
	printf("newfd is %d \n", newfd);

}


