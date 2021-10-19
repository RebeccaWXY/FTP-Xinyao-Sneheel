Description: 
The pack is an application protocol built upon socket, focusing on file transferral. During the execution process, one need to start server first before starting client. One socket, srv_socket, will open as soon as client starts, used to communicate execution message between server and client. When doing file transferral(calling get/put command), a new temporary socket, file_socket, will open exclusively for file transmission, and close immediately after the transmission. 

Executinng: 
Use the makefile to build two separate packs FTPServer and FTPClient. 

File explantions: 
1. client.c:
Code on the client side. It takes in two arguments, ip_addr and port, to create socket for communications between server and client, and for file transferral between the two. After processing the basic information, it will enter an infinite loop until the user chooses to leave, typing in "quit".  

Client works primarily according to the pseudocode given, with message_exchange() separated as a function. 
GET: after exchanging messages with server, start a new TCP connection to server and close the connection after receiving all bytes and writing them into buffer. 

PUT: after sending the request to put file, start a new ICP connection to server and close the connection after sending all bytes to server. 

ls: read the continuous message from server and output until the last file is given. 

pwd: get message from server and output.

cd: get message from server and output. 

!ls: get file list via system call ls. 

!pwd: get the working directory through getcwd(). 

!cd: Operation on client side only. The command asks for new directory path and change path using chdir(). If the new path is not given, it will change to the root directory(cd ..). After the operation, it will notify the user with the success/failure state. 







2. server.c:

Server works primarily according to the pseudocode given.

Server uses a structure called Client to store data abotu an individual client, and also uses an array to check if a particular client has finished the authentication prcess or not.

GET: In the GET method, the server accepts a connection to the new socket, and then serves to the client connected to that socket. The server first receives the name of the file whcih needs to be transferred from the server to the client. 

PUT: In the PUT method, the server accepts a connection to the new socket, and then serves to the client connected to that socket. The server first receives the name of the file whcih needs to be transferred from the client to the server. Then we open a new file to write, and copy data 1500 bytes at a time from the stream to the file. The loop breaks when the number of bytes transferred is 0, ie. all data has been transferred.

ls: calls the popen function which writes the results of the system call to the stream. 

pwd:copies working directory from the client structure.

cd:	changes working director using the chddir call.

!ls, !pwd, !cd are operated only in client locally and does not involve server. 

The server also takes the help of three functions finderu, finderp, and finderfd. The first two help in authorization while finderfd() helps us to identify what username adn passowrd a given given client has for all subsequent operations.
