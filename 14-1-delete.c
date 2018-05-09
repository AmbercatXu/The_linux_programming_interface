#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <stdlib.h>

#define NAME_LENGTH 7	
#define MAX_LINE 255

typedef struct 
{
	char filename[NAME_LENGTH + 1];
}fileElem;

typedef struct CTNode
{
	fileElem data;
	struct CTNode *Next;
}CTBox;

CTBox *initNode;

CTBox *newNode(char* name)
{
	CTBox *tempNode = (CTBox*)malloc(sizeof(CTBox));

	if(tempNode == NULL)
		return NULL;

	memset(tempNode, 0, sizeof(tempNode));
	strcpy(tempNode->data.filename, name);

	return(tempNode);
};

void delNode(CTBox *pNode)
{
	free(pNode);
}

void InsertNode(CTBox *newNode)
{
	CTBox *tempNode = NULL;
	CTBox *cycNode  = NULL;

	if(initNode->Next == NULL)
		initNode->Next = newNode;
	else
	{
		cycNode = initNode;
		while(cycNode->Next != NULL)
		{
			if(strcmp(cycNode->Next->data.filename, newNode->data.filename) > 0)
			{
				tempNode = cycNode->Next;
				cycNode->Next = newNode;
				newNode->Next = tempNode;
				return;
			}
			cycNode = cycNode->Next;
		}
		cycNode->Next = newNode;
	}
}

void recordfile(char* filepath)
{
	DIR *dirp;
	struct dirent *dp;
	char *endptr;
	char filename[NAME_LENGTH + 1];
	CTBox *fileNode  = NULL;

	dirp = opendir(filepath);
	if(dirp == NULL) 
	{
		errMsg("opendir failed on filepath");
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
		if(memcmp(dp->d_name, "x", 1) != 0)
			continue;
		strtol(dp->d_name + 1, &endptr, 10);
		if(*endptr != '\0')
			continue;

		memset(filename, 0, NAME_LENGTH + 1);
		strcat(filename, dp->d_name);
		fileNode = newNode(filename);
		InsertNode(fileNode);
	}
}

void deletefilebyascend(char* filepath)
{
	CTBox *tempNode = NULL;
	CTBox *freeNode = NULL;
	char pathname[MAX_LINE];

	tempNode = initNode->Next;
	while(tempNode != NULL)
	{	
		memset(pathname, 0, MAX_LINE);
		strcat(pathname, filepath);
		strcat(pathname, "/");
		strcat(pathname, tempNode->data.filename);
		if(remove(pathname) == -1)
			errExit("remove");
		tempNode = tempNode->Next;
	}
	freeNode = initNode;
	while(freeNode != NULL)
	{	
		tempNode = freeNode->Next;
		delNode(freeNode);
		freeNode = tempNode;
	}
	initNode = NULL;
}

int main(int argc, char *argv[])
{
	char filepath[MAX_LINE];

	if(argc != 2 && strcmp(argv[1],"--help"))
		usageErr("%s filepath\n",argv[0]);

	memset(filepath, 0, MAX_LINE);
	strcat(filepath, argv[1]);
	if(memcmp(filepath + sizeof(filepath) - 1, "/", 1) == 0)
		filepath[sizeof(filepath) - 1] = 0;

	initNode = newNode("init");
	recordfile(filepath);
	deletefilebyascend(filepath);
}