#include "common.h"

int Socket(int domain, int type, int protocol)
{
	int ret = socket(domain,type,protocol);
	if( ret == -1 )
	{
		perror("Error in creating TCP socket!");
		exit(0);
	}
	return ret;
}

int Bind(int sockfd, const struct sockaddr* myaddr, socklen_t addrlen)
{
	int ret = bind(sockfd, myaddr, addrlen);
	if( ret == -1 )
	{
		perror("Error in binding the address at the server");
		exit(0);
	}
	return ret;
}

int Listen(int sockfd, int backlog)
{
	int ret;
	ret = listen(sockfd, backlog);
	if( ret == -1 )
	{
		perror("TCP server could not listen\n");
		exit(0);
	}
	return ret;
}

int Accept(int sockfd, struct sockaddr* cliaddr, socklen_t* addrlen)
{
	int ret;

	ret = accept(sockfd, cliaddr, addrlen);

	if( ret == -1 )
	{
		perror("TCP server cannot accept incoming clients \n");
		exit(0);
	}
	return ret;
}

int Read(int clientfd, char* buffer, int size)
{
	int char_count = size;
	int chars_read = 0;
	while( ( chars_read = read(clientfd, buffer + chars_read , char_count ) ) > 0 )
	{
		char_count = char_count - chars_read;
		if( char_count == 0 )
		{
			// All chars are read, break out
			break;
		}
	}
	if( chars_read == -1 )
	{
		perror("Error in reading the line in readLine function : handle_client.h\n");
		return -1;
	}
	else if( chars_read == 0 )
	{
		printf("Client's connection is terminated\n");
		return 0;
	}
}

/*
	This writes the specfied no of bytes in the buffer to the clientfd.
	If the client creashed, then write system call which is called inside this function will return -1, 
	then the client's node is invalidated by setting reachable=0 and will be ultimately deleted by the monitor thread.
*/

int Write(int clientfd, char* buff, int len)
{
	int left_chars = len;
	int written_chars = 0;

	int temp;

	// This will write untill the no of characters specified have been written
	while( left_chars > 0 )
	{
		if( (temp = write(clientfd, buff+written_chars, left_chars)) <= 0 )
		{
			// Error
			if( temp < 0 && errno == EINTR)
			{
				continue;
			}
			else
			{
				perror("Error with writing\n");
				exit(0);
			}
		}
		written_chars += temp;
		left_chars -= left_chars;
	}
}
