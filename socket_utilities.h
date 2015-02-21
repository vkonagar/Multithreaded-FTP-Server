#include "common.h"


int read_request(int client_sock, char** command, char** arg)
{
	char* cmd = (char*)malloc(sizeof(char)*REQ_COMMAND_LENGTH);
	char* argument = (char*)malloc(sizeof(char)*REQ_ARG_LENGTH);
	int cmd_pointer = 0;
	int arg_pointer = 0;
	uint8_t flag = 0;
	char c;
	int err;
	while( ( err = Read(client_sock,&c,1 ) ) > 0 )
	{
		if( c == ' ' )
		{
			// Command is done, go with the argument
			flag = 1;
			continue;
		}
		else if( c == '\r' )
		{
			read(client_sock,&c,1);
			if( c == '\n' )
			{
				cmd[cmd_pointer] = '\0';
				argument[arg_pointer] = '\0';
				if( command != NULL )
					*command = cmd;

				if( arg != NULL )
				{
					// Check if the argument is present
					if( arg_pointer != 0)
						*arg = argument;
					else
						*arg = NULL;
				}
				return;
			}
			else
			{
				break;
			}
		}

		if( flag == 0 )
		{
			// COMMAND
			cmd[cmd_pointer++] = c;
		}
		else if( flag == 1 )
		{
			// ARGUMENT
			argument[arg_pointer++] = c;
		}
	}
}

int skip_client_str(int client_sock)
{
	char c;
	int flag = 0;
	while( read(client_sock,&c,1) != -1 )
	{
		if( c == '\r' )
		{	
			read(client_sock,&c,1);
			if( c == '\n' )
			{		
				flag = 1;
				break;
			}
		}
	}
	if( flag == 0 )
	{
		printf("Client error\n");
		exit(0);
	}
}

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
	while( char_count > 0 )
	{
		if( ( chars_read = read(clientfd, buffer + chars_read , char_count ) ) > 0 )
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
			if( errno == EINTR)
				continue;
			perror("Error in reading the line in readLine function : handle_client.h\n");
			return -1;
		}
		else if( chars_read == 0 )
		{
			printf("Client's connection is terminated\n");
			pthread_exit(0);
			return 0;
		}
	}
	return size;
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




