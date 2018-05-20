#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
	int fd1;
	int fd2;
	int fd3;

	if(argc != 2 || strcmp(argv[1], "--help")== 0)
		usageErr("%s pathname \n", argv[0]);

	fd1 = open(argv[1], O_RDWR|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
	fd2 = dup(fd1);
	fd3 = open(argv[1], O_RDWR);

	write(fd1, "Hello,", 6);
	write(fd2, "world", 6);
	lseek(fd2, 0, SEEK_SET);
	write(fd1, "HELLO,", 6);
	write(fd3, "Gidday", 6);

	close(fd1);
	close(fd3);
}
