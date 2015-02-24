#ifndef COMMON_HEADERS
#define COMMON_HEADERS

#include <sys/resource.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <assert.h>
#define TRUE 1
#define FALSE 0

// Networking
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#define BACKLOG 100
#define MAX_OPEN_DESC 1024

typedef struct arguments
{
	int cli_sock;
}arguments_t;

void clean_all_fds(int fd[],int count)
{
	int i;
	for(i=0;i<count;i++)
	{
		close(fd[i]);
	}
}

#define REQ_COMMAND_LENGTH 20
#define REQ_ARG_LENGTH 40
#define BUFF_SIZE 1024

#endif
