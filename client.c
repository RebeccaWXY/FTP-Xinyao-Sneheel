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
	struct sockaddr_in server_address, file_transfer_address;

	if(argc==3)
	{
		see = 0;
		ip_addr = argv[1];
		//port = atoi(argv[2]);
	} else {
		see = 1;
		//no input of IP and port num: use default later
	}
	//get server host name from argv[1] and server port number from argv[2]

	bzero(&server_address,sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(5000);
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);


	if(connect(srv_socket,(struct sockaddr*)&server_address,sizeof(server_address))<0)
	{
		perror("connect");
		return 0;
	}
	//connect(sd, &server −socket−address, . . . )

	char input_total[MAX_LENGTH];
	char input_command[MAX_LENGTH];
	char input_parameters[MAX_LENGTH];
	char buffer[1000];
	/*create variable for storing input commands*/


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
	
			//1 . fgets a reply line from the socket to see if the command
			//is successfully executed and display it to the user
		}
		if (){
		//if the command is "pass password"
	
			//0 . send the command to the server
	
			//1 . fgets a reply line from the socket to see if the command
			//is successfully executed and display it to the user
		}
		if (){
		//if the command is "put a existed file"
	
			//0 . send the command to the server
	
			//1 . open the file
	
			//2 . read the file
	
			//3 . open a new TCP connection to server
	
			//3 . write the file to server
	
			//4 . close the file
	
			//5 . close the TCP connection
		}
		if(){
		//if the command is "get a file "
	
			//1 . send the command to the server
	
			//2 . fgets first line from the server : existed or non−existed
	
			//3 . if existed
		
				//3. 1 open a new TCP connection to server
		
				//3. 1 read the file from the server
		
				//3. 2 write the file to the local directory
		
				//3. 3 close the new TCP connection
	
			//4 . if non−existed display "file name : no such file on server"
		}
		if (){
		//if the command is "cd . . . ", "ls . . .", or "pwd"
	
			//1 . send the command to the server
	
			//2 . fgets a reply line from the socket to see if the command is successfully executed
	
			//3 . read from the socket and display correspondingly.
		}
		if (){
		//if the command is "!ls . . ." or "!pwd"-]
	
			//1 . call system (command) locally
		}
		if (){
		//if the command is "!cd directory"
	
			//1 . call chdir (directory) locally . Note that system ( ) cannot execute "cd . . . "
		}
		if (strcmp(input_command, "user")==0){
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
