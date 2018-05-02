#include <fcntl.h>
#include "tlpi_hdr.h"
#include <dirent.h>
#include <pwd.h>
#include <stdlib.h>
#include <stdio.h>

#define _GNU_SOURCE
#define MAX_LINE 100
#define MAX_NAME_LENGTH 16
#define INTERVAL_LENGTH 5

typedef struct 
{
	char procname[MAX_NAME_LENGTH];
	uid_t procid;
	
}procElem;

typedef struct CTNode
{
	procElem data;
	struct CTNode *firstchild;
	struct CTNode *nextsibling;
}CTBox;

CTBox *initNode;
int level = 0;
int location = 0;

CTBox *newNode(void)
{
	CTBox *tempNode = (CTBox*)malloc(sizeof(CTBox));

	if(tempNode == NULL)
		return NULL;

	memset(tempNode, 0, sizeof(tempNode));

	return(tempNode);
};

CTBox *InitNode(void)
{
	CTBox *tempNode = (CTBox*)malloc(sizeof(CTBox));
	
	if(tempNode == NULL)
		return NULL;
	strcpy(tempNode->data.procname, "init");
	tempNode->data.procid = 1;
	tempNode->firstchild = NULL;
	tempNode->nextsibling = NULL;

	return(tempNode);
};

void delNode(CTBox *pNode)
{
	free(pNode);
}

CTBox *MatchNode(CTBox *pNode, uid_t parentid)
{
	CTBox *tempNode = NULL;

	if(pNode == NULL)
		return NULL;

	if(pNode->data.procid == parentid)
		return pNode;

	tempNode = MatchNode(pNode->firstchild, parentid);
	if(tempNode != NULL)
		return tempNode;
	
	tempNode = MatchNode(pNode->nextsibling, parentid);

	return tempNode;
}

void InsertNode(CTBox *ChildNode, CTBox *parentNode)
{
	CTBox *tempNode = NULL;

	if(parentNode->firstchild == NULL)
		parentNode->firstchild = ChildNode;
	else
	{
		tempNode = parentNode->firstchild;
		while(tempNode->nextsibling!= NULL)
		{
			tempNode = tempNode->nextsibling;
		}
		tempNode->nextsibling = ChildNode;
	}
}

uid_t getprocnameandppid(CTBox *ChildNode, char* procid)
{
	char procpath[] = "/proc/";
	char statuspath[] = "/status";
	char statusname[] = "Name:	";
	char statusppid[] = "PPid:	";
	char path[MAX_LINE];
	char line[MAX_LINE];
	char proccmd[MAX_LINE];
	FILE *fd;
	uid_t parentid = 0;

	memset(path, 0, MAX_LINE);
	strcat(path, procpath);
	strcat(path, procid);
	strcat(path, statuspath);

	fd = fopen(path, "r");
	if(fd == NULL)
	{
		return -1;
	}

	memset(line, 0, MAX_LINE);
	while(fgets(line, MAX_LINE, fd) != NULL)
	{
		if(memcmp(line, statusname, strlen(statusname)) == 0)
		{
			memset(proccmd, 0, MAX_LINE);
			strcpy(proccmd, (line + strlen(statusname)));
			proccmd[strlen(proccmd) - 1] = '\0';
			strcpy(ChildNode->data.procname, proccmd);
			memset(line, 0, MAX_LINE);
			continue;
		}

		if(memcmp(line, statusppid, strlen(statusppid)) == 0)
		{
			parentid = atol(line + strlen(statusppid));
			break;
		}
		
	}
	fclose(fd);
	return parentid;
}

void printcount(int count)
{
	while(count)
	{
		printf("-");
		count--;
	}
}

int printinterval(CTBox *pNode)
{
	if(pNode->firstchild == NULL)
	{
		return 0;
	}	
	printf("─");
	if(pNode->firstchild->nextsibling != NULL)
	{
		printf("┬");
	}
	else
	{
		printf("─");
	}
	printf("─");
	return 2;
}

int printname(CTBox *pNode)
{
	int length = 0;
	char idstr[MAX_NAME_LENGTH] = {0};

	memset(idstr, 0, MAX_NAME_LENGTH);
	snprintf(idstr,(MAX_NAME_LENGTH - 1), "%d", pNode->data.procid);
	length = strlen(pNode->data.procname);
	length += strlen(idstr);
	length += 2;
	printf("%s(%d)", pNode->data.procname, pNode->data.procid);
	if(printinterval(pNode) != 0)
	{
		if(pNode == initNode)
		{
			return (length+2);
		}
		else
		{
			return (length+3);
		}
	}
	return 0;
}

void printlevel(CTBox *pNode)
{
	int count = location;

	if(pNode->nextsibling == NULL)
	{
		return;
	}
	printf("\n");
	while(count - 1)
	{
		printf(" ");
		count--;
	}

	if(pNode->nextsibling->nextsibling != NULL)
	{
		printf("├");
	}
	else
	{
		printf("└");
	}
	printf("─");
}

int showfig(CTBox *pNode)
{
	int templocal = 0;

	if(pNode == NULL)
		return -1;	

	level++;
	templocal = location;
	location += printname(pNode);
	
	showfig(pNode->firstchild);
	location = templocal;
	printlevel(pNode);
	showfig(pNode->nextsibling);
	level--;
	location = templocal;
}

int main(int argc, char *argv[])
{
	CTBox *tempNode;
	CTBox *parentNode;
	DIR *dirp;
	FILE *fd;
	struct dirent *dp;
	uid_t parentid;
	char *endptr;

	initNode = InitNode();
	if(initNode == NULL)
	{
		errMsg("init failed");
		return;
	}

	dirp = opendir("/proc");
	if(dirp == NULL) 
	{
		errMsg("opendir failed on /proc/");
		return;
	}

	for(;;) 
	{
		tempNode = newNode();
		if(tempNode == NULL)
		{
			errMsg("new node malloc failed");
			return;
		}

		errno = 0;
		dp = readdir(dirp);
		if(dp == NULL)
			break;
		if(errno != 0)
			errExit("readdir");
		
		if(strcmp(dp->d_name, ".") == 0 ||strcmp(dp->d_name, "..")== 0)
			continue;
		if(strcmp(dp->d_name, "1") == 0)
			continue;

		strtol(dp->d_name, &endptr, 10);
		if(*endptr != '\0')
			continue;

		tempNode->data.procid = atol(dp->d_name);
		parentid = getprocnameandppid(tempNode, dp->d_name);
		if( parentid == -1 || parentid == 0)
		{
			delNode(tempNode);
			continue;
		}

		parentNode = MatchNode(initNode, parentid);
		if(parentNode == NULL)
		{
			delNode(tempNode);
			continue;
		}
		else
		{
			InsertNode(tempNode, parentNode);
		}
	}
	showfig(initNode);

}
