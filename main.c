#include "common.h"
#ifndef PROTO
#define PROTO
#include "protocol.h"
#endif
#include "socket_utilities.h"

void sig_term_handler()
{
	// Kill all threads
	printf("FTP Server exiting\n");
	exit(0);
}

void sig_pipe_handler()
{
	printf("Client terminated because of Pipe termination\n");
}

// Runs the server socket on port 21
void client_function(void* var)
{
	increment_thread_count();
	
	struct sched_param sp;
	int policy;
	
	if( pthread_getschedparam(pthread_self(), &policy, &sp) < 0 )
	{
		printf("THREAD: Error getting the sched params\n");
		exit(0);
	}
	// Check the default sheduling policy.
	if( policy == SCHED_RR )
	{
		//printf("THREAD: Its SCHED_RR\n");
	}
//	printf("THREAD: My priority is %d\n",sp.sched_priority);
	// All open descriptors are stored here for the client
	int open_desc[MAX_OPEN_DESC];
	int open_desc_count = 0;
	// get the client socket
	// Copy the args to local variables
	int client_sock = (int)var;
	// Add fd to the list
	open_desc[open_desc_count++] = client_sock;
	assert(client_sock);

	//printf("Client connected\n");
	
	// This structure is for data connection
	struct sockaddr_in active_client_addr;
	// This is for the requests
	ftp_request_t request;
	// Clear the contents
	bzero((void*)&active_client_addr,sizeof(active_client_addr));
	bzero((void*)&request,sizeof(request));
	// Now send the greeting to the client.
	Write(client_sock, greeting, strlen(greeting), open_desc, open_desc_count);
	// Command and arg to be recieved from the client
	
	char* command;
	char* arg;
	for( ;; )
	{
		// Start serving the requests
		// This method will overwrite the existing command and arg
		if( read_request(client_sock, &request, open_desc, open_desc_count) ==  0 )
		{
			printf("Bad Command\n");
			Write(client_sock, error, strlen(error), open_desc, open_desc_count);
			break;
		}
		// get the command and arg from the structure
		command = request.command;
		arg = request.arg;
		//printf("%s : %s\n",command,arg);
		
		// Process the command
		if( strcmp(command,"USER") == 0 )
		{
			// USER REQUEST
			Write(client_sock, allow_user, strlen(allow_user), open_desc, open_desc_count);
		}
		else if( strcmp(command,"SYST") == 0 )
		{
			// SYSTEM REQUEST
			Write(client_sock, system_str, strlen(system_str), open_desc, open_desc_count);
		}
		else if( strcmp(command,"PORT") == 0 )
		{
			// PORT COMMAND
			// Send reply
			Write(client_sock, port_reply, strlen(port_reply), open_desc, open_desc_count);
			// Argument is of form h1,h2,h3,h4,p1,p2
			store_ip_port_active(arg,&active_client_addr);
		}
		else if( strcmp(command,"TYPE") == 0 )
		{
			Write(client_sock, type_ok, strlen(type_ok), open_desc, open_desc_count);
		}
		else if( strcmp(command,"QUIT") == 0 )
		{
			// Child exited
			// break and free the client_sock
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
				Write(client_sock, file_error, strlen(file_error), open_desc, open_desc_count);
				// If the file open has error, quit the connection.
				break;
			}
			// FILE OK
			open_desc[open_desc_count++] = file;

			Write(client_sock, file_ok, strlen(file_ok), open_desc, open_desc_count);
			
			// Now transfer the file to the client
			int data_sock = Socket(AF_INET, SOCK_STREAM, 0, open_desc, open_desc_count);
			open_desc[open_desc_count++] = data_sock;	
			
			if( connect(data_sock, (struct sockaddr*)&active_client_addr, sizeof(active_client_addr))  == -1 )
			{
				printf("Cant establish data connection to %d\n", ntohs(active_client_addr.sin_port));
				// Close existing fd's related to this command
				Write(client_sock, data_open_error, strlen(data_open_error), open_desc, open_desc_count);
			}
			// Now transfer the file.
			int n;
			char data_buff[BUFF_SIZE];
			while( ( n = Read(file, data_buff,BUFF_SIZE, open_desc, open_desc_count) ) > 0 )
			{
				Write( data_sock, data_buff, n, open_desc, open_desc_count);
			}
			// File transferred succesfully
			// Send reply now
			Write( client_sock, file_done, strlen(file_done), open_desc, open_desc_count);
			break;
		}
		else
		{
			Write( client_sock, error, strlen(error), open_desc, open_desc_count);
			break;
		}
	}
	Write( client_sock, close_con, strlen(close_con), open_desc, open_desc_count);
	sleep(1);
	//printf("Closing client connection and killing THREAD\n");
	clean_all_fds(open_desc,open_desc_count);
	decrement_thread_count();
	pthread_exit(0);
}


void set_res_limits()
{
	// Set MAX FD's to 50000
	struct rlimit res;
	res.rlim_cur = 50000;
	res.rlim_max = 50000;
	if( setrlimit(RLIMIT_NOFILE, &res) == -1 )
	{
		perror("Resource FD limit");
		exit(0);
	}
	printf("FD limit set to 50000\n");	
	if( setrlimit(RLIMIT_RTPRIO, &res) == -1 )
	{
		perror("Resource Prioiry limit");
		exit(0);
	}
	printf("Prioirty limit set to 100\n");
}

int main()
{
	// Set resource limits
	set_res_limits();

	// Get the default thread priority
	struct sched_param sp;
	int policy;
	
	if( pthread_getschedparam(pthread_self(), &policy, &sp) < 0 )
	{
		printf("Error getting the sched params\n");
		exit(0);
	}

	// Check the default sheduling policy.
	if( policy == SCHED_OTHER )
	{
		printf("Its SCHED_OTHER by default and priority is 0\n");
	}
	// Assign the RR scheduler and a high priority
	sp.sched_priority = THREAD_PRIORITY_HIGH;
	if( pthread_setschedparam(pthread_self(), SCHED_RR, &sp) < 0 )
	{
		printf("Error in setting the thread priority in main\n");
	}
	printf("Thread priority set to high in main\n");

	int open_desc[1];
	int open_desc_count;
	sigignore(SIGPIPE);
	signal(SIGTERM, sig_term_handler);
	// Change the current working directory to the FILES folder.
	if( chdir("../FTP_FILES") == -1 )
	{
		perror("CWD");
		exit(0);
	}

	// This is the listen socket on 21
	int listen_sock = Socket(AF_INET, SOCK_STREAM, 0, open_desc, open_desc_count);
	struct sockaddr_in server_addr;
	
	server_addr.sin_port = htons(21);
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htons(INADDR_ANY);

	Bind(listen_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
	Listen(listen_sock, BACKLOG);
	printf("FTP SERVER STARTED\n");
	// thread id
	pthread_t pid;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	// Set stack size to 1 MB
 	if( pthread_attr_setstacksize(&attr,512*1024) < 0 )
	{
		printf("Stack size set error");
		exit(0);
	}
	printf("Stack size set to 512 KB\n");
	// Set the inherit sheduling attr
	if( pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED) < 0 )
	{
		printf("Error in set the inherit sheduling attr from main\n");
	}
	// Now assign the priority and type of scheduling to the attr
	if( pthread_attr_setschedpolicy(&attr, SCHED_RR) < 0 )
	{
		printf("Error in seting the scheduling type\n");
	}
	sp.sched_priority = THREAD_PRIORITY_LOW;
	if( pthread_attr_setschedparam(&attr, &sp) < 0 )
	{
		printf("Error in setting hte priority of the children\n");
	}

	// Create a client addr structure
	struct sockaddr_in client_addr;
	int client_addr_len = 0;
	int client_sock;
	// Monitoring thread
	if( pthread_create(&pid, &attr, (void*)monitoring_thread, NULL ) != 0 )
        {
        	perror("pthread create in main");
        	close(client_sock);
        }
	
	// Accept the connections
	while( TRUE )
	{
		//printf("LISTENING FOR CLIENTS\n");
		client_sock = Accept(listen_sock, (struct sockaddr*)&client_addr, &client_addr_len);
		if( client_sock == -1 )
		{
			continue;
		}
		// Now create a new thread for this client.	
		// Pass the client sock fd as the argument.
		if( pthread_create(&pid, &attr, (void*)client_function, (void*)client_sock ) != 0 )
		{
			perror("pthread create in main");
			close(client_sock);
		}
	}
}
