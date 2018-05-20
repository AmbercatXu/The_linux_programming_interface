#include <pwd.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
	struct passwd *pwd1;
	struct passwd *pwd2;
	char *name;
	//long namelen;

	//namelen = sysconf(_SC_LOGIN_NAME_MAX);
	//if(namelen == -1)
	//	namelen = 256;

	if((argc != 2)||strcmp(argv[1], "--help") == 0)
	{
		printf("getnametest name \n");
	}
	name = argv[1];

	pwd1 = getpwnam(name);
	if(pwd1 == NULL)
	{
		exit(EXIT_FAILURE);
	}
	else
		printf("getpwnam uid is %ld \n",(long)pwd1->pw_uid);

	while((pwd2 = getpwent()) != NULL)
	{
		if(strcmp(pwd2->pw_name,name) == 0)
		{
			printf("getpwent uid is %ld \n",(long)pwd2->pw_uid);
			break;
		}
	}
	endpwent();

	exit(EXIT_SUCCESS);
}

