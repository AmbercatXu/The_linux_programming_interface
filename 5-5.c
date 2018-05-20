#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
	int oldfd = 0;
	int newfd = 0;
	int flags;
	int accessMode;
	int curr;

	if(argc != 2 || strcmp(argv[1], "--help")== 0)
		usageErr("%s pathname \n", argv[0]);

	oldfd = open(argv[1], O_RDWR);
	if(oldfd == -1)
		errExit("open");

	newfd = dup(oldfd);

	flags = fcntl(newfd, F_GETFL);
	accessMode = flags & O_ACCMODE;
	if(accessMode == O_RDWR)
		printf("newfd has the same access mode with oldfd \n");

	lseek(newfd, 10, SEEK_SET);
	lseek(oldfd, 0, SEEK_SET);

	curr = lseek(newfd, 0, SEEK_CUR);
	if(curr == 0)
		printf("newfd has the same seek offset with oldfd \n");
	if(close(oldfd) == -1)
		errExit("close");
}


