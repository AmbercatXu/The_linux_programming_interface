#include <fcntl.h>
#include "tlpi_hdr.h"
#include <dirent.h>
#include <pwd.h>
#include <stdlib.h>
#include <stdio.h>

#define _GNU_SOURCE
#define MAX_LINE 100

uid_t userIdFromName(const char *name)
{
	struct passwd *pwd;
	uid_t u;
	char *endptr;

	if(name == NULL || *name == '\0')
		return -1;
	u = strtol(name, &endptr, 10);
	if(*endptr == '\0')
		return u;
	pwd = getpwnam(name);
	if(pwd == NULL)
		return -1;
	return pwd->pw_uid;
}



int main(int argc, char *argv[])
{
	FILE *fd;
	struct dirent *dp;
	DIR *dirp;
	char *endptr;
	char procpath[] = "/proc/";
	char statuspath[] = "/status";
	char statusname[] = "Name:	";
	char statusuid[]  = "Uid:	";
	char procname[MAX_LINE];
	char path[MAX_LINE];
	char line[MAX_LINE];
	uid_t uid;

	if(argc != 2 || strcmp(argv[1], "--help") == 0)
		usageErr("%s usrname \n", argv[0]);

	uid = userIdFromName(argv[1]);
	if(uid == -1)
	{
		errMsg("get user id failed");
		return;
	}
		
	dirp = opendir("/proc");
	if(dirp == NULL) 
	{
		errMsg("opendir failed on /proc/");
		return;
	}

	for(;;) {
		errno = 0;
		dp = readdir(dirp);
		if(dp == NULL)
			break;
		if(errno != 0)
			errExit("readdir");
		
		if(strcmp(dp->d_name, ".") == 0 ||strcmp(dp->d_name, "..")== 0)
			continue;

		strtol(dp->d_name, &endptr, 10);
		if(*endptr != '\0')
			continue;

		memset(path, 0, MAX_LINE);
		strcat(path, procpath);
		//printf("path is %s \n", path);
		strcat(path, dp->d_name);
		//printf("path is %s \n", path);
		strcat(path, statuspath);
		//printf("path is %s \n", path);
		fd = fopen(path, "r");

		memset(line, 0, MAX_LINE);
		while(fgets(line, MAX_LINE, fd) != NULL)
		{
			//printf("line is %s\n", line);
			if(memcmp(line, statusname, strlen(statusname)) == 0)
			{
				memset(procname, 0, MAX_LINE);
				strcpy(procname, (line + strlen(statusname)));
				procname[strlen(procname) - 1] = '\0';
				memset(line, 0, MAX_LINE);
				continue;
			}
			if(memcmp(line, statusuid, strlen(statusuid)) == 0)
			{
				if(uid == atol(line + strlen(statusuid)))
				{
					printf("name: %s, procID: %s \n", procname, dp->d_name);				
				}
				break;
			}
		}
		fclose(fd);
	}
	if(closedir(dirp) == -1)
		errMsg("closedir");
}
