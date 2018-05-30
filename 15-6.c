#include <sys/stat.h>
#include <fcntl.h>
#include "lib/tlpi_hdr.h"

int
main(int argc, char *argv[])
{
	struct stat sb;
	int i;
	mode_t mode;

	for(i = 1; i < argc; i++)
	{
		if (stat(argv[i], &sb) == -1)
            errExit("stat");

        mode = sb.st_mode|S_IRUSR|S_IRGRP|S_IROTH;
        if(chmod(argv[i], mode) == -1)
            errExit("chmod");

        if((sb.st_mode&(S_IXUSR|S_IXGRP|S_IXOTH))||(S_ISDIR(sb.st_mode)))
        {
        	mode = mode|S_IXUSR|S_IXGRP|S_IXOTH;
        	if(chmod(argv[i], mode) == -1)
            	errExit("chmod");
        }
	}
}