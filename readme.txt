Client:
Server works primarily according to the pseudocode given, with message_exchange() separated as a function. 
GET:

PUT: 

ls: read the continuous message from server and output until the last file is given. 

pwd: get message from server and output.

cd: get message from server and output. 

!ls: get file list via system call ls. 

!pwd: get the working directory through getcwd(). 

!cd: Operation on client side only. The command asks for new directory path and change path using chdir(). If the new path is not given, it will change to the root directory(cd ..). After the operation, it will notify the user with the success/failure state. 













Server:

Server works primarily according to the pseudocode given.
GET: In the GET method, the server accepts a connection to the new socket, and then serves to the client connected to that socket. The server first receives the name of the file whcih needs to be transferred from the server to the client. 

PUT: In the PUT method, the server accepts a connection to the new socket, and then serves to the client connected to that socket. The server first receives the name of the file whcih needs to be transferred from the client to the server. Then we open a new file to write, and copy data 1500 bytes at a time from the stream to the file. The loop breaks when the number of bytes transferred is 0, ie. all data has been transferred.

ls: calls the popen function which writes the results of the system call to the stream. 

pwd:copies working directory from the client structure.

cd:	changes working director using the chddir call.
