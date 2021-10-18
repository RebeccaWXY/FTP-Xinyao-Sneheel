#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/select.h>
#include<arpa/inet.h>
#include<sys/time.h>
#include<sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h> 

#define MAX_PATH 600
#define NUM_USERS 100
#define MAX_CLIENTS 1000



struct Client{
	char *users;
	char *pass;
	char current_path[MAX_PATH];
	int fd;
	bool is_logged;
};

struct arg_struct{
	int ffd;
	char *fname;
};

int finderu(char* u,struct Client clients[100]);
int finderp(char* p,struct Client clients[100]);
int finder_fd(int fd, struct Client clients[100]);
int serve_client(int client_fd, int file_transfer_fd, int *auth, struct Client clients[100]);
void* get_client(void* arguments);
void* put_client(void* arguments);


//main function

int main(int argc, char** argv)

{

	//array to keep a check on if a given file descrptor has been authorized
	int clients_auth[1000];
	//to store a user's data
	struct Client clients[100];
	//initialising all authorized users
	clients[0].users="user";
	clients[0].pass="user";
	clients[0].is_logged=0;
	clients[0].fd=0;
	getcwd(clients[0].current_path,MAX_PATH);
	printf("%s \n",clients[0].current_path);

	for(int i=0;i<1000;i++)
		clients_auth[i]=0;

	for(int i=1;i<100;i++)
	{
		clients[i].is_logged=0;
		clients[i].users="";
		clients[i].pass="";
		clients[i].fd=0;
		getcwd(clients[i].current_path,MAX_PATH);
	}

	//takes in ip address and port if they are given
	int see=1;
	char* ip_addr;
	int port;
	if(argc==3)
		see=0;

	// Reads in port and ip
	if(see==0)
	{
		ip_addr = argv[1];
		port = atoi(argv[2]);
	}


	//1. socket();

	//main socket
	int server_fd = socket(AF_INET,SOCK_STREAM,0);
	
	if(server_fd<0)
	{
		perror("socket");
		return -1;
	}
	if(setsockopt(server_fd,SOL_SOCKET,SO_REUSEADDR,&(int){1},sizeof(int))<0)
	{
		perror("setsock");
		return -1;
	}
	//socket for file transfer

	int file_transfer_fd = socket(AF_INET,SOCK_STREAM,0);
	if(file_transfer_fd<0)
	{
		perror("socket");
		return -1;
	}
	if(setsockopt(file_transfer_fd,SOL_SOCKET,SO_REUSEADDR,&(int){1},sizeof(int))<0)
	{
		perror("setsock");
		return -1;
	}

	//2. bind ();

	//main bind
	struct sockaddr_in server_address;
	bzero(&server_address,sizeof(server_address));
	server_address.sin_family = AF_INET;
	if(see==0)
	{	
		server_address.sin_port = htons(port);
		server_address.sin_addr.s_addr = inet_addr(ip_addr);
	}
	else
	{
		server_address.sin_port = htons(5000);
		server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	if(bind(server_fd,(struct sockaddr*)&server_address,sizeof(server_address))<0)
	{
		perror("bind");
		return -1;
	}

	//bind for file transfer

	struct sockaddr_in file_address;
	bzero(&file_address,sizeof(file_address));
	file_address.sin_family = AF_INET;
	file_address.sin_port = htons(5500);
	file_address.sin_addr.s_addr = server_address.sin_addr.s_addr;

	if(bind(file_transfer_fd,(struct sockaddr*)&file_address,sizeof(file_address))<0)
	{
		perror("bind");
		return -1;
	}


	//3. listen()

	if(listen(server_fd,5)<0)
	{
		perror("listen");
		return -1;
	}

	//select variables for main connection
	fd_set full_fdset,ready_fdset;
	FD_ZERO(&full_fdset);
	FD_SET(server_fd,&full_fdset);
	int max_fd = server_fd;

	//select variables for file transfer connection
	// fd_set file_full_fdset, file_ready_fdset;
	// FD_ZERO(&full_fdset);
	// FD_SET(file_transfer_fd,&file_full_fdset);
	// int file_max_fd= file_transfer_fd;


	//4. accept()
	while(1)
	{	
		// printf("max_fd=%d\n",max_fd);
		ready_fdset = full_fdset;
		if(select(max_fd+1,&ready_fdset,NULL,NULL,NULL)<0)
		{
			perror("select");
			return -1;
		}

		for(int fd = 0; fd<=max_fd; fd++)
		{
			if(FD_ISSET(fd,&ready_fdset))
			{
				if(fd==server_fd)
				{
					int new_fd = accept(server_fd,NULL,NULL);
					// printf("client fd = %d \n",new_fd);
					FD_SET(new_fd,&full_fdset);
					if(new_fd>max_fd) max_fd=new_fd;
				}
				else
				{
					if(serve_client(fd,file_transfer_fd,clients_auth,clients)==-1)
					{
						FD_CLR(fd,&full_fdset);
						if(max_fd==fd)
						{
							for(int i=max_fd-1; i>=0; i--)
								if(FD_ISSET(i,&full_fdset))
								{
									max_fd = i;
									break;
								}
						}
					}
				}
			}

		}
		//checking for connections to file transfer server
		// file_ready_fdset = file_full_fdset;
		// if(select(file_max_fd+1,&file_ready_fdset,NULL,NULL,NULL)<0)
		// {
		// 	perror("select");
		// 	return -1;
		// }

		// for(int fd = 0; fd<=file_max_fd; fd++)
		// {
		// 	if(FD_ISSET(fd,&file_ready_fdset))
		// 	{
		// 		if(fd==file_transfer_fd)
		// 		{
		// 			int file_new_fd = accept(file_transfer_fd,NULL,NULL);
		// 			// printf("client fd = %d \n",new_fd);
		// 			FD_SET(file_new_fd,&file_full_fdset);
		// 			if(file_new_fd>file_max_fd) file_max_fd=file_new_fd;
		// 		}
		// 		else
		// 		{
		// 			if(serve_client_files(fd,clients_auth,clients)==-1)
		// 			{
		// 				FD_CLR(fd,&full_fdset);
		// 				if(max_fd==fd)
		// 				{
		// 					for(int i=max_fd-1; i>=0; i--)
		// 						if(FD_ISSET(i,&full_fdset))
		// 						{
		// 							max_fd = i;
		// 							break;
		// 						}
		// 				}
		// 			}
		// 		}
		// 	}

		// }

		
	}

	//6. close());
	close(server_fd);

	return 0;
}

int serve_client(int client_fd, int file_transfer_fd, int *auth, struct Client clients[100])
{
	char message[100];
	char msgx[100];
	bzero(&msgx,sizeof(msgx));
	bzero(&message,sizeof(message));
	if(recv(client_fd,message,sizeof(message),0)<0)
		{
			perror("recv");
			return 0;
		}
	if(strcmp(message,"quit")==0)
		{
			printf("Client disconnected \n");
			int find = finder_fd(client_fd,clients);
			if (find>=0)
				clients[find].is_logged=0;
			close(client_fd);
			return -1;
		}
	if(auth[client_fd]==0)
	{	
		char comm[100];
		char para[100];
		bzero(&comm,sizeof(comm));
		bzero(&para,sizeof(para));
		sscanf(message,"%s %s", comm , para);
		if(strcmp(comm,"user")==0)
		{
			int check = finderu(para,clients);
			if(check>=0)
			{
				if(clients[check].is_logged==0)
				{
					strcpy(msgx,"Username OK, password required ");
					//printf("%s",msgx);
					send(client_fd,msgx,sizeof(msgx),0);
					auth[client_fd]=1;
					clients[check].fd=client_fd;
				}
				else
				{
					printf("User is already logged in\n");
					strcpy(msgx,"User logged in already, enter different details");
					send(client_fd,msgx,sizeof(msgx),0);
					return 0;
				}
			}
			else
			{
				strcpy(msgx,"Username not found");
				send(client_fd,msgx,sizeof(msgx),0);
			}
		}
		else if(strcmp(comm,"pass")==0)
		{
			strcpy(msgx,"Set username first");
			send(client_fd,msgx,sizeof(msgx),0);
		}
		else
		{
			strcpy(msgx,"Authenticate first");
			send(client_fd,msgx,sizeof(msgx),0);
		}
		return 0;
	}


	else if(auth[client_fd]==1)
	{
		char comm[100];
		char para[100];
		bzero(&comm,sizeof(comm));
		bzero(&para,sizeof(para));
		sscanf(message,"%s %s", comm , para);
		if(strcmp(comm,"pass")==0)
		{
			int check=finder_fd(client_fd,clients);
			if(check>=0 && strcmp(clients[check].pass,para)==0)
			{	
				strcpy(msgx,"Authentication Complete");
				auth[client_fd]=2;
				clients[check].is_logged=1;
				printf("User Authenticated \n");
				send(client_fd,msgx,sizeof(msgx),0);
			}
			else
			{
				strcpy(msgx,"Wrong password");
				send(client_fd,msgx,sizeof(msgx),0);
			}
		}
		else
		{
			strcpy(msgx,"Set password");
			send(client_fd,msgx,sizeof(msgx),0);
		}
		return 0;
	}
	else
	{
		//user has authenticated
		int mark = finder_fd(client_fd,clients);
		//setting current directory to current working directory in server for the client being served
		int m=chdir(clients[mark].current_path);

		//reading the instruction sent
		char comm[100];
		char para[100];
		bzero(&comm,sizeof(comm));
		bzero(&para,sizeof(para));
		sscanf(message,"%s %s", comm , para);

		//cases
		if(strcmp(comm,"user")==0)
		{
			strcpy(msgx,"User has already logged in");
			send(client_fd,msgx,sizeof(msgx),0);
		}
		else if (strcmp(comm,"pass")==0)
		{
			strcpy(msgx,"User has already logged in");
			send(client_fd,msgx,sizeof(msgx),0);
		}
		else if (strcmp(comm,"get")==0 || strcmp(comm,"GET")==0)
		{
			FILE* fptr = fopen(para,"r");
			if(!fptr)
			{
				perror("Cant open the file");
				strcpy(msgx,"nonexisted");
				send(client_fd,msgx,sizeof(msgx),0);
				return 0;
			}
			else
			{
				strcpy(msgx,"existed");
				send(client_fd,msgx,sizeof(msgx),0);
				fclose(fptr);
				int client_sd = accept(file_transfer_fd,NULL,NULL);
				if(client_sd<0)
				{
					perror("accept ");
					fclose(fptr);
				}
				else
				{
					fclose(fptr);
					struct arg_struct args;
					args.ffd=client_sd;
					strcpy(args.fname,para);
					pthread_t thread_id;
					pthread_create(&thread_id,NULL,get_client,&args);
				}
			}
			return 0;
		}
		//implementing the put command
		else if (strcmp(comm,"put")==0 || strcmp(comm,"PUT")==0)
		{

			FILE* fptr = fopen(para,"w");
			if(!fptr)
			{
				perror("File creation error");
			}
			else
			{	
				fclose(fptr);
				//code to enable file transfer
				int client_sd = accept(file_transfer_fd,NULL,NULL);
				if(client_sd<0)
				{
					perror("accept ");
				}
				else
				{
					struct arg_struct args;
					args.ffd=client_sd;
					strcpy(args.fname,para);
					pthread_t thread_id;
					pthread_create(&thread_id,NULL,put_client,&args);
				}
			}

			return 0;
		}

		//implementing the ls command
		else if (strcmp(comm,"ls")==0 || strcmp(comm,"LS")==0)
		{
			FILE* fp = popen(message,"r");
			char path[1024];
			bzero(&path,sizeof(path));
			if(fp==NULL)
			{
				strcpy(msgx,"wrong command usage");
				send(client_fd,msgx,sizeof(msgx),0);
			}
			else
			{
				strcpy(msgx,"succesfully executed!");
				send(client_fd,msgx,sizeof(msgx),0);
				while(fgets(path, 1024, fp) != NULL)
				{
					send(client_fd,path,1024,0);
					printf("%s", path);
					// bzero(&path,sizeof(path));
				}	

			}
			int status = pclose(fp);
			if(status==-1)
				printf("Error- pclose");
			send(client_fd,"bye",sizeof("bye"),0);
			return 0;
		}

		//implementing the pwd command
		else if (strcmp(comm,"pwd")==0 || strcmp(comm,"PWD")==0)
		{
			strcpy(msgx,clients[mark].current_path);
			send(client_fd,msgx,sizeof(msgx),0);
			return 0;
		}

		//implementing the cd command

		else if (strcmp(comm,"cd")==0 || strcmp(comm,"CD")==0)
		{
			int f = chdir(para);
			if(f==0)
			{
				strcpy(msgx,"Changed directory succesfully");
				send(client_fd,msgx,sizeof(msgx),0);
				getcwd(clients[mark].current_path,MAX_PATH);

			} 
			else
			{
				strcpy(msgx,"Directory change unsuccesful");
				send(client_fd,msgx,sizeof(msgx),0);
			}
			return 0;

		}
		else
		{
			strcpy(msgx,"Invalid Command");
			send(client_fd,msgx,sizeof(msgx),0);
		}
		return 0;
	}
}

//finds username if it exists
int finderu(char* u,struct Client clients[100])
{
	for(int i =0; i<100; i++)
	{
		if(strcmp(clients[i].users,u)==0)
			return i;
	}
	return -1;
}

void* get_client(void* arguments)
{
	char buffer[1500];
	int bytes=0;
	struct arg_struct args = *(struct arg_struct*)arguments;
	int client_sd= args.ffd;
	FILE* fptr = fopen(args.fname,"w");
	do
	{
		int i;
		for(i=0; i<1500; i++)
		{
			char ch = fgetc(fptr);
			if(feof(fptr)) break;
			buffer[i] = ch;
		}
		bytes+=i;
		send(client_sd,buffer,i,0);
	}while(!feof(fptr));
	printf("Bytes sent %d \n",bytes);
	close(client_sd);
	fclose(fptr);
}

void* put_client(void* arguments)
{
	char buffer[1500];
	int bytes;
	struct arg_struct args = *(struct arg_struct*)arguments;
	int client_sd= args.ffd;
	FILE* fptr = fopen(args.fname,"w");
	do
	{
		bytes = recv(client_sd,buffer,sizeof(buffer),0);
		if(bytes>0)
			fwrite(buffer,bytes,1,fptr);
	}while(bytes>0);
	printf("Bytes received %d \n",bytes);
	close(client_sd);
	fclose(fptr);
}
//finds password if it exists
int finderp(char* p,struct Client clients[100])
{
	for(int i =0; i<100; i++)
	{
		if(strcmp(clients[i].pass,p)==0)
			return i;
	}
	return -1;
}

int finder_fd(int fd, struct Client clients[100])
{
	for(int i =0; i<100; i++)
	{
		if(fd==clients[i].fd)
			return i;
	}
	return -1;
}

