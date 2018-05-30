#include <sys/stat.h>
#include <fcntl.h>
#include "lib/tlpi_hdr.h"
#include <unistd.h>
#include <string.h>

int
main(int argc, char *argv[])
{
	struct stat sb;
	int mode = 0;
	int tarmode = 0;
	Boolean statLink;  
	uid_t usrid = 0;
	gid_t grpid = 0;
	int fname; 


	statLink = (argc > 1) && strcmp(argv[1], "-l") == 0;
    fname = statLink ? 2 : 1;
    if (fname >= (argc - 1) || (argc > 1 && strcmp(argv[1], "--help") == 0))
    usageErr("%s [-l] file mode\n"
            "        -l = use lstat() instead of stat()\n", argv[0]);

    if (statLink) {
        if (lstat(argv[fname], &sb) == -1)
        {
        	printf("fail \n");
        	return -1;
        }      	
    } else {
        if (stat(argv[fname], &sb) == -1)
    	{
    		printf("fail \n");
    		return -1;
    	}
    }

    if(strcmp(argv[fname + 1], "F_OK") == 0)
    {
    	printf("success\n");
		return 0;
    }

	usrid = geteuid();
	grpid = getegid();


	if(usrid == 0)
	{
		printf("success\n");
		return 0;
	}

    if(usrid == sb.st_uid)
    {
		mode = sb.st_mode & (S_IRUSR|S_IWUSR|S_IXUSR);
		mode = mode >> 6;
    }
    else if(grpid == sb.st_gid)
    {
    	mode = sb.st_mode & (S_IRGRP|S_IWGRP|S_IXGRP);
    	mode = mode >> 3;
    }
    else
    {
    	mode = sb.st_mode & (S_IROTH|S_IWOTH|S_IXOTH);
    }

    if(strstr(argv[fname + 1], "R_OK"))
    	tarmode = tarmode|S_IROTH;
    if(strstr(argv[fname + 1], "W_OK"))
    	tarmode = tarmode|S_IWOTH;
    if(strstr(argv[fname + 1], "X_OK"))
    	tarmode = tarmode|S_IXOTH;  

    if(mode != (tarmode | mode))
    {
    	printf("fail \n");
    	return -1;
    }
    printf("success\n");
    return 0;
}