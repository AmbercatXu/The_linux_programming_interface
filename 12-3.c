#include <fcntl.h>
#include "tlpi_hdr.h"
#include <dirent.h>
#include <pwd.h>
#include <stdlib.h>
#include <stdio.h>

#define _GNU_SOURCE
#define MAX_LINE 255
#define MAX_NAME_LENGTH 16
#define INTERVAL_LENGTH 5

void getfdlink(char *pidpath, char *targetpath)
{
	char procpath[] = "/proc/";
	char fdpath[] = "/fd";
	struct dirent *dp;
	char path[MAX_LINE];
	char buf[MAX_LINE];
	DIR *dirp;
	char *endptr;

	memset(path, 0, MAX_LINE);
	strcat(path, procpath);
	strcat(path, pidpath);
	strcat(path, fdpath);

	dirp = opendir(path);
	if(dirp == NULL) 
	{
		errMsg("opendir failed on /proc/PID/fd/");
		return;
	}

	for(;;) 
	{
		memset(path, 0, MAX_LINE);
		strcat(path, procpath);
		strcat(path, pidpath);
		strcat(path, fdpath);

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
		strcat(path, "/");
		strcat(path, dp->d_name);

		memset(buf, 0, MAX_LINE);
		if(readlink(path, buf, MAX_LINE) == -1)
			continue;
		if(strcmp(buf, targetpath) == 0)
		{
			printf("PID: %s\n", path);
			//printf("%s\n", targetpath);
		}

	}
}

int main(int argc, char *argv[])
{
	DIR *dirp;
	FILE *fd;
	struct dirent *dp;
	char *endptr;

	if(argc != 2 || strcmp(argv[1],"--help")==0)
		usageErr("%s pathfile\n",argv[0]);

	dirp = opendir("/proc");
	if(dirp == NULL) 
	{
		errMsg("opendir failed on /proc/");
		return;
	}

	for(;;) 
	{
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

		getfdlink(dp->d_name, argv[1]);
	}

}