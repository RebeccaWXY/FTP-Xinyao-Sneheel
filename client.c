#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<unistd.h>

int main()
{
	//1. socket();
	int server_fd = socket(AF_INET,SOCK_STREAM,0);
	printf("server_fd = %d \n",server_fd);
	if(server_fd<0)
	{
		perror("socket");
		return 0;
	}

	//2. connect ();
	struct sockaddr_in server_address;
	bzero(&server_address,sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(5000);
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);

	if(connect(server_fd,(struct sockaddr*)&server_address,sizeof(server_address))<0)
	{
		perror("connect");
		return 0;
	}
	//3. send/recv
	char filename[50];
	//rcv
	if(argc<2)
	{
		printf("Enter the file name: ");
		scanf("%s",filename);
	}

	else
		strcpy(filename,argv[1]);
	
	send(srv_socket,filename,strlen(filename),0);
	printf("Requesting file %s \n",filename);
	char buffer[1500];
	int bytes;
	FILE *fptr;
	if(!(fptr = fopen(filename,"w")))
		perror("Cant create file");
	else
	{
		do
		{
			bytes = recv(srv_socket,buffer,sizeof(buffer),0);
			if(bytes>0)
				fwrite(buffer,bytes,1,fptr);
		}while(bytes>0);
		fclose(fptr);
	}

	while(1)
	{
		char user_input[100];
		fgets(user_input,100,stdin);
		user_input[strcspn(user_input,"\n")]=0;   //remove new line char from string by replacing it with '\0'

		send(server_fd,user_input,strlen(user_input),0); 
		
		if(strcmp(user_input,"bye")==0)
			break;
	}


	//4. close
	close(server_fd);

	return 0;
