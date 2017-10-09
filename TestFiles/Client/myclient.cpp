/* myclient.cpp */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
//#include <stdio.h>
#include <string.h>
#include <iostream>
#define BUF 1024
#define PORT 6543

using namespace std;

int main (int argc, char **argv) 
{
	int create_socket;
	char buffer[BUF], receiver[BUF];
	struct sockaddr_in address;
	int size;
	int menu;
	string temp;

	if( argc < 3 ){
		cout << "Usage: " << argv[0] << " ServerAdresse + Port Number" << endl;
		exit(EXIT_FAILURE);
	}

	if ((create_socket = socket (AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Socket error");
		return EXIT_FAILURE;
	}

	memset(&address,0,sizeof(address));
	address.sin_family = AF_INET;
	int user_port = atoi(argv[2]);
	address.sin_port = htons (user_port);
	inet_aton (argv[1], &address.sin_addr);

	if (connect ( create_socket, (struct sockaddr *) &address, sizeof (address)) == 0)
	{
		cout << "Connection with server " << inet_ntoa(address.sin_addr) << " established" << endl;
		size = recv(create_socket,buffer,BUF-1, 0);

		if (size>0)
		{
			buffer[size]= '\0';
			cout << buffer << endl;
		}
	}
	else
	{
		cout << "Connect error - no server available" << endl;
		return EXIT_FAILURE;
	}

	do 
	{
		cout << "Choose operation: " << endl;
		cout << "SEND: Send a message" << endl;
		cout << "LIST: List all messages from a user" << endl;
		cout << "READ: Find a user's message by its number" << endl;
		cout << "DEL: Delete one message" << endl;
		cout << "QUIT: Logout" << endl;
		fgets(buffer, BUF, stdin);
		send(create_socket, buffer, strlen(buffer), 0);

		size = recv(create_socket, receiver, BUF-1, 0);

		if( size > 0)
		{
			receiver[size] = '\0';
			menu = atoi(receiver);
			
			switch(menu)
			{
				case 1: 
					do
					{
						cout << "Please enter sender name: ";
						fgets(buffer, BUF, stdin);
						send(create_socket, buffer, strlen(buffer), 0);
						size = recv(create_socket, receiver, BUF-1, 0);
						receiver[size] = '\0';
						temp = receiver;
					} while(temp == "ERR");
					do
					{
						cout << "Please enter receiver name: ";
						fgets(buffer, BUF, stdin);
						send(create_socket, buffer, strlen(buffer), 0);
						size = recv(create_socket, receiver, BUF-1, 0);
						receiver[size] = '\0';
						temp = receiver;
					} while(temp == "ERR");
					do
					{
						cout << "Please enter object: ";
						fgets(buffer, BUF, stdin);
						send(create_socket, buffer, strlen(buffer), 0);
						size = recv(create_socket, receiver, BUF-1, 0);
						receiver[size] = '\0';
						temp = receiver;
					} while(temp == "ERR");
					cout << "Please enter your message:" << endl;;
					do
					{
						fgets(buffer, BUF, stdin);
						send(create_socket, buffer, strlen(buffer), 0);
					} while((buffer[0] != '.') && (buffer[1] != '\n'));
					break;
				default: break;
			}
		}
	}while (strcmp (buffer, "quit\n") != 0);

	close (create_socket);
	return EXIT_SUCCESS;
}
