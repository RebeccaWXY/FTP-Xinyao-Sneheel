#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdlib.h>

#define MAX_LENGTH 200


int main(int argc, char** argv)
{
	int srv_socket = socket(AF_INET,SOCK_STREAM,0);	
	//sd=socket(AF_INET, SOCK_STREAM, 0)
	char* ip_addr;
	int port;
	int see;
	int valread;
	char buffer[1024];
	struct sockaddr_in server_address, file_transfer_address;
	char input_total[MAX_LENGTH];
	char input_command[MAX_LENGTH];
	char input_parameters[MAX_LENGTH];
	/*create variable for storing input commands*/

	if(argc==3)
	{
		see = 0;
		ip_addr = argv[1];
		port = atoi(argv[2]);
	} else {
		see = 1;
		//no input of IP and port num: use default later
	}
	//get server host name from argv[1] and server port number from argv[2]
	if (see==0){
		bzero(&server_address,sizeof(server_address));
		server_address.sin_family = AF_INET;
		server_address.sin_port = htons(port);
		server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	} else if (see==1){
		bzero(&server_address,sizeof(server_address));
		server_address.sin_family = AF_INET;
		server_address.sin_port = htons(5000);
		server_address.sin_addr.s_addr = htonl(INADDR_ANY);		
	}


	if(connect(srv_socket,(struct sockaddr*)&server_address,sizeof(server_address))<0)
	{
		perror("connect");
		return 0;
	}
	//connect(sd, &server −socket−address, . . . )

	while (1){
	//While ( 1 ) {
		printf("ftp>");
		//show ftp>
		fgets(input_total, MAX_LENGTH, stdin);
		//fgets a ftp command line from keyboard
		sscanf(input_total, "%s %s", input_command, input_parameters);
		
		if (strcmp(input_command, "user")==0){
		//if the command is "user username ”
			send(srv_socket, input_total, MAX_LENGTH,0);
			//0. send the command to the server
		    valread = read(srv_socket, buffer, 1024);
		    printf("%s\n", buffer);
			//1. fgets a reply line from the socket to see if the command
			//is successfully executed and display it to the user
		}
		
		else if (strcmp(input_command, "pass")==0){
		//if the command is "pass password"
			send(srv_socket, input_total, MAX_LENGTH,0);
			//0 . send the command to the server
		    valread = read(srv_socket, buffer, 1024);
		    printf("%s\n", buffer);	
			//1 . fgets a reply line from the socket to see if the command
			//is successfully executed and display it to the user
		}
		
		
		
		
		/*codes to be filled: put, get, cd, ls, pwd, ...!cd*/
		
		
		
		
		else if (strcmp(input_command, "!ls")==0){
		//if the command is "!ls . . ."
			DIR *d;
			struct dirent *dir;
			d = opendir(".");
			if (d){
				while((dir = readdir(d)) != NULL){
					printf("%s\n", dir->d_name);
				}
				closedir(d);
			}
			//1 . call system (command) locally
		}

		else if (strcmp(input_command, "!pwd")==0){
		//if the command is "!pwd"-]
			char cwd_client[200];
			if (getcwd(cwd_client, sizeof(cwd_client)) != NULL){
				printf("%s\n", cwd_client);
			} else {
				printf("error getting cwd\n");
			}
			//1 . call system (command) locally
		}		
		else if (strcmp(input_command, "quit")==0){
		//if the command is "quit"
			close(srv_socket);
			//1 . close the socket
			break;
			//2 . break or exit
		}
		else {
			printf("An invalid ftp command.\n");
			//otherwise : show "An invalid ftp command."		
		}
		
	}
	return 0;
}

