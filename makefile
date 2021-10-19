all: build

build: clean
	gcc -c client.c -o FTPclient.o
	gcc -c server.c -o FTPserver.o -lpthread
	gcc FTPclient.o -o FTPclient
	gcc FTPserver.o -o FTPserver -lpthread


clean: 
	rm -rf FTPserver FTPclient

