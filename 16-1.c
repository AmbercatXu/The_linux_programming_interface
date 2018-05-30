#include <sys/xattr.h>
#include "lib/tlpi_hdr.h"

#define XATTR_SIZE 100



int
main(int argc, char *argv[])
{
	char tarvalue[XATTR_SIZE];
	char pathname[XATTR_SIZE], name[XATTR_SIZE];
	ssize_t valuelen;
    Boolean statLink;           
    int fname;  

	statLink = (argc > 1) && strcmp(argv[1], "-l") == 0;
	fname = statLink ? 2 : 1;

	if ( argc < (fname + 3 ) || (strcmp(argv[1], "--help") == 0))
        usageErr("%s [-l] filename EAname EAvalue \n", argv[0]);

    memset(pathname, 0, XATTR_SIZE);
    memset(name, 0, XATTR_SIZE);
    memset(tarvalue, 0, XATTR_SIZE);
    if (statLink) 
    {
    	strcpy(pathname, argv[2]);
    	strcpy(name, argv[3]);
    	strcpy(tarvalue, argv[4]);
	}
	else
	{
    	strcpy(pathname, argv[1]);
    	strcpy(name, argv[2]);
    	strcpy(tarvalue, argv[3]);
	}


	if (statLink) 
		lsetxattr(pathname, name, tarvalue, strlen(tarvalue), 0);
	else
		setxattr(pathname, name, tarvalue, strlen(tarvalue), 0);

    printf("#file: %s\n", pathname);
    printf("%s = \"%s\"\n", name, tarvalue);
}