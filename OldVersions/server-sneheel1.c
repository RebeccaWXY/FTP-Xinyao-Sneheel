#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/select.h>
#include<arpa/inet.h>
#include<sys/time/h>
#include<sys/types.h>
#include <unistd.h>
#include <stdlib.h>


int main(int argc, char** argv)

{
	//initialise list of users and their passwords (temporary)
	string users[100];
	string pass[100];
	int fds[100];
	users[0]="user";
	pass[0]="pass";

	// Reads in port and ip
	string ip_addr = argv[1];
	int port = atoi(argv[2]);


	//1. socket();
	int server_fd = socket(AF_INET,SOCK_STREAM,0);
	printf("server_fd = %d \n",server_fd);

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

	//2. bind ();
	struct sockaddr_in server_address;
	bzero(&server_address,sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(5000);
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(server_fd,(struct sockaddr*)&server_address,sizeof(server_address))<0)
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
	fd_set full_fdset,ready_fdset;
	FD_ZERO(&full_fdset);
	FD_SET(server_fd,&full_fdset);
	int max_fd = server_fd;


	//4. accept()
	while(1)
	{	
		printf("max_fd=%d\n",max_fd);
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
					printf("client fd = %d \n",new_fd);
					FD_SET(new_fd,&full_fdset);
					if(new_fd>max_fd) max_fd=new_fd;
				}
				else
				{
					if(serve_client(fd)==-1)
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
		
	}

	//6. close());
	close(server_fd);

	return 0;
}

int serve_client(int client_fd, int & auth, )
{
	char message[100];
	char msgx[100];	
	bzero(&message,sizeof(msgx));
	bzero(&message,sizeof(message));
	if(recv(client_fd,message,sizeof(message),0)<0)
		{
			perror("recv");
			return 0;
			//break;
		}
	if(strcmp(message,"bye")==0)
		{
			printf("Client disconnected \n");
			close(client_fd);
			return -1;
		}
	if(auth==0)
	{	
		char* auth[2];
		strcpy(auth[0],parseauth(message)[0]);
		strcpy(auth[1],parseauth(message)[1])
		int check = finderu(parseauth(message)[1]);
		string msgx="";
		
		if(check>=0)
		{
			msgx = "Username OK, password required ";
			send(client_fd,msgx,sizeof(msgx),0);
			auth=1;
			fds[check]=client_fd;	
		}
		else
		{
			msgx = "Username not found";
		}
		return 0;
	}
	else if(auth==1)
	{
		if(fds[finderp(parseauth(message))]==client_fd)
		{	
			msgx="Authentication Complete";
			auth=2;
			send(client_fd,msgx,sizeof(msgx),0);
		}
		else
		{
			msgx="Wrong password";
			send(client_fd,msgx,sizeof(msgx),0);
		}
		return 0;
	}
	else if(auth==2)
	{
		//user has authenticated
	}
	else
	{
		msgx = "Authenticate first";
		send(client_fd,msgx,sizeof(msgx),0);
		return 0;

	}
}

char* parseauth(string k)
{
	
}