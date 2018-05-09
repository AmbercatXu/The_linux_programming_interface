#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <stdlib.h>

#define PATH_NAME_LENGTH 8
#define RANDOM_NAME_LENGTH 6	
#define MAX_LINE 255
#define CREATFILE_BY_RAND 0
#define CREATFILE_BY_SEQUENC 1
#define RAMDOM_MAX 1000000

void getrandname(char* name)
{
	struct timeval tpstart;
	int randname;
	char filename[RANDOM_NAME_LENGTH + 1];

	memset(filename, 0, RANDOM_NAME_LENGTH);

	gettimeofday(&tpstart, NULL);
	srand(tpstart.tv_usec);
	randname = rand()%RAMDOM_MAX;
	sprintf(filename, "%06d", randname);
	filename[RANDOM_NAME_LENGTH] = '\0';
	strcat(name, filename);
}

void createfilebyrand(int filenum, char* filepath)
{
	char pathname[MAX_LINE];
	char filename[PATH_NAME_LENGTH];
	int  fd;

	while(filenum != 0)
	{
		memset(filename, 0, PATH_NAME_LENGTH);
		getrandname(filename);

		memset(pathname, 0, MAX_LINE);
		strcat(pathname, filepath);
		strcat(pathname, "/");
		strcat(pathname, "x");
		strcat(pathname, filename);

		errno = 0;
		fd = open(pathname, O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
		if(fd == -1 && errno != EEXIST)
			errExit("open");
	 	if(errno != EEXIST)
		{
			filenum--;
			write(fd, pathname, 1);
			if(close(fd) == -1)
				errExit("close");
		}
	}
}

void createfilebysequec(int filenum, char* filepath)
{
	char pathname[MAX_LINE];
	char filename[RANDOM_NAME_LENGTH];
	int  fd;
	int  filecount = filenum;

	while(filenum != 0)
	{
		memset(filename, 0, RANDOM_NAME_LENGTH);
		sprintf(filename, "%06d", (filecount - filenum + 1));
		filenum--;

		memset(pathname, 0, MAX_LINE);
		strcat(pathname, filepath);
		strcat(pathname, "/");
		strcat(pathname, "x");
		strcat(pathname, filename);
		errno = 0;
		fd = open(pathname, O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
		if(fd == -1 && errno != EEXIST)
			errExit("open");
	 	if(errno != EEXIST)
		{
			write(fd, pathname, 1);
			if(close(fd) == -1)
				errExit("close");
		}
	}
}

int main(int argc, char *argv[])
{
	int  filenum = 0;
	int  randorseq = 0;
	char filepath[MAX_LINE];

	if(argc != 4 && strcmp(argv[1],"--help"))
		usageErr("%s filenum filepath randreq\n",argv[0]);

	filenum = atol(argv[1]);
	randorseq = atol(argv[3]);

	memset(filepath, 0, MAX_LINE);
	strcat(filepath, argv[2]);  
	if(memcmp(filepath + sizeof(filepath) - 1, "/", 1) == 0)
		filepath[sizeof(filepath) - 1] = 0;

	if(randorseq == CREATFILE_BY_RAND)
		createfilebyrand(filenum, filepath);
	else
		createfilebysequec(filenum, filepath);


}