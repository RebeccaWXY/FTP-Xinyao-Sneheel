#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<dirent.h>

#include<sys/sendfile.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define MAX_LENGTH 200

int message_exchange(int srv_socket, char * buffer, char * input_total);

int main(int argc, char** argv)

{
	int srv_socket = socket(AF_INET,SOCK_STREAM,0);	
	int file_socket = socket(AF_INET, SOCK_STREAM,0);

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
		inet_aton(ip_addr, &server_address.sin_addr);
		bzero(&file_transfer_address,sizeof(file_transfer_address));
		file_transfer_address.sin_family = AF_INET;
		file_transfer_address.sin_port = htons(port);
		inet_aton(ip_addr, &file_transfer_address.sin_addr);
	} else if (see==1){
		bzero(&server_address,sizeof(server_address));
		server_address.sin_family = AF_INET;
		server_address.sin_port = htons(5000);
		server_address.sin_addr.s_addr = htonl(INADDR_ANY);	
		bzero(&file_transfer_address,sizeof(file_transfer_address));
		file_transfer_address.sin_family = AF_INET;
		file_transfer_address.sin_port = htons(5000);
		file_transfer_address.sin_addr.s_addr = htonl(INADDR_ANY);		
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
		//clear out input_command, and para
		strcpy(input_command,"");
		strcpy(input_parameters,"");

		memset(input_total, 0, MAX_LENGTH);
		fgets(input_total, MAX_LENGTH, stdin);
		//fgets a ftp command line from keyboard
		sscanf(input_total, "%s %s", input_command, input_parameters);
		printf("input_command:%s\n", input_command);
		printf("input_parameters:%s\n", input_parameters);

		if (strcmp(input_command, "user")==0 || strcmp(input_command, "pass")==0){
		//if the command is "user username”/"pass password"
			message_exchange(srv_socket, buffer, input_total);
		}
		
		else if (strcmp(input_command, "put")==0){
		//if the command is "put a existed file"
			send(srv_socket, input_total, MAX_LENGTH,0);
			//0 . send the command to the server
			int src = open(input_parameters, O_RDONLY);
			//1 . open the file
			if (src<0){
				printf("Error occurs when open file: %s\n", input_parameters);
			}
			//2 . read the file
			if(connect(file_socket,(struct sockaddr*)&file_transfer_address,sizeof(file_transfer_address))<0)
			{
				perror("connect error before sending file");
				return 0;
			}
			//3 . open a new TCP connection to server

			struct stat st;
			stat(input_parameters, &st);
			int filesize = st.st_size;
			//get the size of the file for sendfile()
			int sentsize = sendfile(file_socket, src, NULL, filesize);
			//error check:
			if (sentsize <= 0) {
				printf("ERROR: the file was not sent\n");
			}
				//(1) sentsize invalid, the file is not sent
			else if (sentsize < filesize){
				printf("WARNING: the file was not fully sent; there might be data missing\n");
			}
				//(2) the file is not fully sent
			else if (sentsize > filesize){
				printf("WARNING: the data sent was larger than the original file; there might be a mistake\n");
			}   //(3) the file is larger than original; there might be a mistake in between. 
			else {
				printf("Success! The file was sent. \n");
			}
			//4 . write the file to server

			//we use the function of ssize sendfile(int out_fd, int in_fd, off_t * offset, size_t count)
			//sendfile return: size of the file sent
			close(src);
			//5 . close the file
			close(file_socket);
			//6 . close the TCP connection
		}

		/*
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
		*/
		else if (strcmp(input_command, "ls")==0)
		{
			message_exchange(srv_socket, buffer, input_total);
		    bzero(&buffer,sizeof(buffer));
			do
			{
				valread = read(srv_socket,buffer,sizeof(buffer));
				if(strcmp(buffer,"bye")==0)
					break;
				if(valread!=0)
					printf("%s",buffer);
			}while(valread!=0);
		}

		
		else if (strcmp(input_command, "cd")==0 || strcmp(input_command, "pwd")==0){
		//if the command is "cd . . . ", "ls . . .", or "pwd"
			message_exchange(srv_socket, buffer, input_total);
		}
		
		
		else if (strcmp(input_command, "!ls")==0 || strcmp(input_command, "!LS")==0){
		//if the command is "!ls . . ."
			// DIR *d;
			// struct dirent *dir;
			// d = opendir(".");
			// if (d){
			// 	while((dir = readdir(d)) != NULL){
			// 		printf("%s\n", dir->d_name);
			// 	}
			// 	closedir(d);
			// }
			//1 . call system (command) locally
			char buff[MAX_LENGTH];
			strcpy(buff,"ls ");
			strcat(buff,input_parameters);
			system(buff);
		}

		else if (strcmp(input_command, "!pwd")==0 || strcmp(input_command, "!PWD")==0){
		//if the command is "!pwd"-]
			char cwd_client[200];
			if (getcwd(cwd_client, sizeof(cwd_client)) != NULL){
				printf("%s\n", cwd_client);
			} else {
				printf("error getting cwd\n");
			}
			//1 . call system (command) locally
		}
		
		else if (strcmp(input_command,"!cd")==0 || strcmp(input_command,"!CD")==0)
		{
			char new_dir[200];
			strcpy(new_dir,input_parameters);
			int f;
			if (argc<3){
				f = chdir("..");
			} else {
				f = chdir(new_dir);
			}
			if(f==0)
			{
				printf("successfully changed\n");
				printf("current directory: %s\n", new_dir);
			} 
			else
			{
				printf("Directory change unsuccesful");
			}
		}
		//if the command is "!cd directory"
			//1 . call chdir (directory) locally . Note that system ( ) cannot execute "cd . . . "
		
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

int message_exchange(int srv_socket, char * buffer, char * input_total){
	memset(buffer,0,1024); //fill buffer with \0
	if(send(srv_socket, input_total, MAX_LENGTH,0) < 0)
		perror("ERROR: failure during sending message to server.\n");//write the whole command to socket- very similar to send
	//0. send the command to the server

	int valread = read(srv_socket, buffer, 1024);
	//printf("valread: %d\n", valread);
	if ( valread < 0 ) {
		close(srv_socket);
		perror("ERROR: failure during receiving message from server\n");
	}
		printf("%s\n",buffer);
	//1 . fgets a reply line from the socket to see if the command
			//is successfully executed and display it to the user
}
