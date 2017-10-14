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
	int number_of_messages;
	
	/* on missing arguments display usage error message */
	if(argc < 3){
		cout << "Usage: " << argv[0] << " ServerAdresse + Port Number" << endl;
		exit(EXIT_FAILURE);
	}

	if((create_socket = socket (AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Socket error");
		return EXIT_FAILURE;
	}

	memset(&address,0,sizeof(address));
	address.sin_family = AF_INET;
	int user_port = atoi(argv[2]);
	address.sin_port = htons (user_port);
	inet_aton (argv[1], &address.sin_addr);
	
	/* Error / Confirmation message on (un-)successful connection */
	if(connect (create_socket, (struct sockaddr *) &address, sizeof(address)) == 0)
	{
		cout << "Connection with server " << inet_ntoa(address.sin_addr) << " established" << endl;
		size = recv(create_socket,buffer,BUF-1, 0);

		if (size > 0)
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
		/* Menu */
		cout << "Choose operation: " << endl;
		cout << "SEND: Send a message" << endl;
		cout << "LIST: List all messages from a user" << endl;
		cout << "READ: Find a user's message by its number" << endl;
		cout << "DEL : Delete one message" << endl;
		cout << "QUIT: Logout\n" << endl;
		
		/* take input and send it to server */
		fgets(buffer, BUF, stdin);
		send(create_socket, buffer, strlen(buffer), 0);

		size = recv(create_socket, receiver, BUF-1, 0);

		if( size > 0)
		{
			receiver[size] = '\0';
			menu = atoi(receiver);

			switch(menu)
			{
				case 1:{
					do
					{
						cout << "Please enter sender name: ";
						fgets(buffer, BUF, stdin);
						send(create_socket, buffer, strlen(buffer), 0);
						size = recv(create_socket, receiver, BUF-1, 0);
						receiver[size] = '\0';
						temp = receiver;

						if(temp == "ERR")
						{
							cout << "ERR: Max 8 Characters and no Special Characters" << endl; // ERROR MESSAGE
						}
					} while(temp == "ERR");
					do
					{
						cout << "Please enter receiver name: ";
						fgets(buffer, BUF, stdin);
						send(create_socket, buffer, strlen(buffer), 0);
						size = recv(create_socket, receiver, BUF-1, 0);
						receiver[size] = '\0';
						temp = receiver;

						if(temp == "ERR")
						{
							cout << "ERR: Max 8 Characters and no Special Characters" << endl; // ERROR MESSAGE
						}
					} while(temp == "ERR");
					do
					{
						cout << "Please enter object: ";
						fgets(buffer, BUF, stdin);
						send(create_socket, buffer, strlen(buffer), 0);
						size = recv(create_socket, receiver, BUF-1, 0);
						receiver[size] = '\0';
						temp = receiver;

						if(temp == "ERR")
						{
							cout << "ERR: Max 80 Characters" << endl; // ERROR MESSAGE
						}
					} while(temp == "ERR");
					cout << "Please enter your message:" << endl;;
					do
					{
						fgets(buffer, BUF, stdin);
						send(create_socket, buffer, strlen(buffer), 0);
					} while((buffer[0] != '.') && (buffer[1] != '\n'));

					size = recv(create_socket, receiver, BUF-1, 0);
					receiver[size] = '\0';
					temp = receiver;

					if(temp == "ERR")
					{
						cout << "ERR: Could not send\n" << endl; // ERROR MESSAGE
					}
					else
					{
						cout << "OK: Message sent\n" << endl; // SUCCESS
					}
					break;}
				case 2:{
                    cout << "Please enter user name: ";
                    fgets(buffer, BUF, stdin);
                    send(create_socket, buffer, strlen(buffer), 0);

                    size = recv(create_socket, receiver, BUF-1, 0);
                    receiver[size] = '\0';
                    send(create_socket, buffer, strlen(buffer), 0);
                    temp = receiver;

                    if(temp != "ERR")
                    {
                        number_of_messages = temp.length() > 0 ? stoi(temp) : 0;

                        for(int i = 0; i < number_of_messages; ++i)
                        {
                            cout << "#" << i + 1;

                            size = recv(create_socket, receiver, BUF-1, 0);
                            receiver[size] = '\0';
                            send(create_socket, buffer, strlen(buffer), 0);
                            temp = receiver;

                            cout << ": Sender: " << temp;

                            size = recv(create_socket, receiver, BUF-1, 0);
                            receiver[size] = '\0';
                            send(create_socket, buffer, strlen(buffer), 0);
                            temp = receiver;

                            cout << ", Object: " << temp << endl;
                        }
                    }
                    else
                    {
                        cout << "ERR: No messages for this user found.\n" << endl;
                    }

                    break;}
				case 3:{
                    cout << "Please enter a user name: ";
                    fgets(buffer, BUF, stdin);
                    send(create_socket, buffer, strlen(buffer), 0);

                    cout << "Please enter a post number: ";
                    fgets(buffer, BUF, stdin);
                    send(create_socket, buffer, strlen(buffer), 0);
					temp = "";
					char dump[2] = { 'O', 'K' };

                    do
                    {
                        size = recv(create_socket, buffer, BUF-1, 0);
                        buffer[size] = '\0';
                    	send(create_socket, dump, strlen(dump), 0);

                        if((buffer[0] != '.') && (buffer[1] != '\n'))
                        {
                            temp += buffer;
                        }
                    } while((buffer[0] != '.') && (buffer[1] != '\n') && (temp != "ERR"));

                    cout << "\n------------------\n" << endl;
                    cout << temp << endl;
                    cout << "------------------\n" << endl;

                    break;}
				case 4:{
					cout << "Please enter a user name: ";
                    fgets(buffer, BUF, stdin);
                    send(create_socket, buffer, strlen(buffer), 0);

                    cout << "Please enter a post number: ";
                    fgets(buffer, BUF, stdin);
                    send(create_socket, buffer, strlen(buffer), 0);
					temp = "";
					
					
					break;}
				default: 
					cout << "default case" << endl;
					break;
			}
		}
	}while (strcmp (buffer, "quit\n") != 0);

	close (create_socket);
	return EXIT_SUCCESS;
}

