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
#include <pthread.h>
#include <assert.h>
#define TRUE 1
#define FALSE 0

// Networking
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#define BACKLOG 100000
#define MAX_OPEN_DESC 1024

#define THREAD_PRIORITY_LOW 20
#define THREAD_PRIORITY_HIGH 50

typedef struct arguments
{
	int cli_sock;
}arguments_t;

int threads_active = 0;

pthread_mutex_t mutex;

void increment_thread_count()
{
        // Lock
        pthread_mutex_lock(&mutex);
        threads_active++;
        // Unlock
        pthread_mutex_unlock(&mutex);
}

void decrement_thread_count()
{

        // Lock
        pthread_mutex_lock(&mutex);
        threads_active--;
        // Unlock
        pthread_mutex_unlock(&mutex);
}

void clean_all_fds(int fd[],int count)
{
	int i;
	for(i=0;i<count;i++)
	{
		close(fd[i]);
	}
}





int monitoring_thread()
{
	while(1)
	{
		sleep(5);
		printf("Current threads: %d\n",threads_active);
	}
}



#define REQ_COMMAND_LENGTH 20
#define REQ_ARG_LENGTH 40
#define BUFF_SIZE 1024

#endif
