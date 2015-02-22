#include "common.h"
#include "socket_utilities.h"
#include "protocol.h"

void sig_term_handler()
{
	// Kill all threads
	exit(0);
}

void sig_pipe_handler()
{
	printf("Client terminated\n");
}

// Runs the server socket on port 21
void client_function(void* var)
{
	sigignore(SIGPIPE);
	signal(SIGTERM, sig_term_handler);
	// get the client socket
	struct arguments* args = (struct arguments*)var;	
	// Copy the args to local variables
	int client_sock = args->cli_sock;
	assert(client_sock);
	struct sockaddr_in* client_addr = args->cli_addr;
	assert(client_addr);
	// Print client details
	const char *inet_ntop(int af, const void *src,
	                             char *dst, socklen_t size);
	char ip[16];
	inet_ntop(AF_INET, &((*client_addr).sin_addr),(void*)ip,16);
	printf("CLIENT CONNECTED - IP:%s PORT:%d\n", ip, ntohs(client_addr->sin_port)); 
	
	struct sockaddr_in active_client_addr;
	bzero((void*)&active_client_addr,sizeof(active_client_addr));
	// Now send the greeting to the client.
	Write(client_sock, greeting, strlen(greeting));
	for( ;; )
	{
		// Start serving the requests
		char* command;
		char* arg;
		int err = read_request(client_sock, &command, &arg);
		printf("%s : %s\n",command,arg);
		if( strcmp(command,"USER") == 0 )
		{
			// USER REQUEST
			Write(client_sock, allow_user, strlen(allow_user));
		}
		else if( strcmp(command,"SYST") == 0 )
		{
			// SYSTEM REQUEST
			Write(client_sock, system_str, strlen(system_str));
		}
		else if( strcmp(command,"PORT") == 0 )
		{
			// PORT COMMAND
			// Send reply
			Write(client_sock, port_reply, strlen(port_reply));
			// Argument is of form h1,h2,h3,h4,p1,p2
			store_ip_port_active(arg,&active_client_addr);
		}
		else if( strcmp(command,"TYPE") == 0 )
		{
			Write(client_sock, type_ok, strlen(type_ok));
		}
		else if( strcmp(command,"QUIT") == 0 )
		{
			// Close socket
			printf("Client %s:%d exited\n", ip, client_addr->sin_port);
			break;
		}
		else if( (strcmp(command,"LIST") == 0 ) || (strcmp(command,"RETR") == 0 ))
		{
			// RETR
			// Argument will have the file name
			int file;
			if( strcmp(command,"LIST") ==  0)
			{
				// Execute the list command and store it in a file
				system("ls -l > .list");
				// Now open that file and send it to the client
				file = open(".list",O_RDONLY);
			}
			else
			{
				file = open(arg,O_RDONLY);
			}

			if( file == -1 )
			{
				perror("Open");
				Write(client_sock, file_error, strlen(file_error));
			}
			Write(client_sock, file_ok, strlen(file_ok));
			// Now transfer the file to the client
			int data_sock = Socket(AF_INET, SOCK_STREAM, 0 );
			// Connect to the port and IP given by client
		
			/*	
			struct sockaddr_in server_addr;
        		server_addr.sin_port = htons(20);
       	 		server_addr.sin_family = AF_INET;
       			server_addr.sin_addr.s_addr = htons(INADDR_ANY);	
		
			Bind(data_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
			*/
			if( connect(data_sock, (struct sockaddr*)&active_client_addr, sizeof(active_client_addr))  == -1 )
			{
				printf("Cant Connect to %d\n", ntohs(active_client_addr.sin_port));
				inet_ntop(AF_INET, &((*client_addr).sin_addr),(void*)ip,16);
				printf("Can't connect to IP: %s\n",ip);	
				break;
			}
			
			// Now transfer the file.
			int n;
			char data_buff[BUFF_SIZE];
			while( ( n = read(file, data_buff,BUFF_SIZE) ) > 0 )
			{
				Write( data_sock, data_buff, n);
			}
			// File transferred succesfully
			// Send reply now
			close(data_sock);
			Write( client_sock, file_done, strlen(file_done));
		}
		free(command);
		free(arg);
	}
	close(client_sock);
	pthread_exit(0);
}


int main()
{
	// Change the current working directory to the FILES folder.
	if( chdir("../FTP_FILES") == -1 )
	{
		perror("CWD");
		exit(0);
	}

	// This is the listen socket on 21
	int listen_sock = Socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server_addr;
	
	server_addr.sin_port = htons(21);
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htons(INADDR_ANY);

	Bind(listen_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
	Listen(listen_sock, BACKLOG);
	printf("FTP SERVER STARTED\n");
	// thread id
	pthread_t pid;
	// Create a client addr structure
	struct sockaddr_in client_addr;
	int client_addr_len = 0;
	int client_sock;
	// Accept the connections
	while( TRUE )
	{
		printf("LISTENING FOR CLIENTS\n");
		client_sock = Accept(listen_sock, (struct sockaddr*)&client_addr, &client_addr_len);
		// Pack the arguments for the client
		struct arguments* args = (struct arguments*)malloc(sizeof(struct arguments));
		args->cli_sock = client_sock;
		args->cli_addr = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
		memcpy(args->cli_addr, &client_addr, sizeof(client_addr));
		// Now create a new thread for this client.	
		if( pthread_create(&pid, NULL, (void*)client_function, (void*)args ) != 0 )
		{
			perror("pthread create in main");
		}
	}
}
