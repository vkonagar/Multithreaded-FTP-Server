#ifndef COMMON_HEADERS
#define COMMON_HEADERS

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#define TRUE 1
#define FALSE 0

// Networking
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#define BACKLOG 5

typedef struct arguments
{
	int cli_sock;
	struct sockaddr_in* cli_addr;
}arguments_t;

#endif
