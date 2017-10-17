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

void PrintHorrorzontal();

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
		cout << "\e(0\x6c\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B";
		cout << "\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B";
		cout << "\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B";
		cout << "\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B";
		cout << "\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B";
		cout << "\e(0\x71\e(B\e(0\x71\e(B\e(0\x6b\e(B" << endl;
		cout << "\e(0\x78\e(BChoose operation:                        \e(0\x78\e(B" << endl;
		cout << "\e(0\x78\e(BSEND: Send a message                     \e(0\x78\e(B" << endl;
		cout << "\e(0\x78\e(BLIST: List all messages from a user      \e(0\x78\e(B" << endl;
		cout << "\e(0\x78\e(BREAD: Find a user's message by its number\e(0\x78\e(B" << endl;
		cout << "\e(0\x78\e(BDEL : Delete one message                 \e(0\x78\e(B" << endl;
		cout << "\e(0\x78\e(BQUIT: Logout                             \e(0\x78\e(B" << endl;
		cout << "\e(0\x6d\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B";
		cout << "\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B";
		cout << "\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B";
		cout << "\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B";
		cout << "\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B";
		cout << "\e(0\x71\e(B\e(0\x71\e(B\e(0\x6a\e(B" << endl;

		memset(buffer, 0, sizeof(buffer));

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
					/* CASE SEND */
					PrintHorrorzontal();
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

					PrintHorrorzontal();

					if(temp == "ERR")
					{
						cout << "ERR: Could not send" << endl; // ERROR MESSAGE
					}
					else
					{
						cout << "OK: Message sent" << endl; // SUCCESS
					}

					break;}
				case 2:{
					/* CASE LIST */
			PrintHorrorzontal();
                    cout << "Please enter user name: ";
                    fgets(buffer, BUF, stdin);
                    send(create_socket, buffer, strlen(buffer), 0);

                    size = recv(create_socket, receiver, BUF-1, 0);
                    receiver[size] = '\0';
                    temp = receiver;

                    if(temp != "ERR")
                    {
                        send(create_socket, buffer, strlen(buffer), 0);
                        number_of_messages = temp.length() > 0 ? stoi(temp) : 0;
			PrintHorrorzontal();

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
					/* CASE READ */
			PrintHorrorzontal();
                    cout << "Please enter a user name: ";
                    fgets(buffer, BUF, stdin);
                    send(create_socket, buffer, strlen(buffer), 0);

                    size = recv(create_socket, buffer, BUF-1, 0);
                    buffer[size] = '\0';
                    temp = buffer;

                    if(temp != "ERR")
                    {
                        cout << "Please enter a post number: ";
                        fgets(buffer, BUF, stdin);
                        send(create_socket, buffer, strlen(buffer), 0);
                        temp = "";

                        size = recv(create_socket, buffer, BUF-1, 0);
                        buffer[size] = '\0';
                        temp = buffer;

                        if(temp != "ERR")
                        {

                            temp = "";

                            send(create_socket, buffer, strlen(buffer), 0);
                            do
                            {
                                size = recv(create_socket, buffer, BUF-1, 0);
                                buffer[size] = '\0';
                                send(create_socket, buffer, strlen(buffer), 0);

                                if((buffer[0] != '.') && (buffer[1] != '\n'))
                                {
                                    temp += buffer;
                                }
                            } while((buffer[0] != '.') && (buffer[1] != '\n') && (temp != "ERR"));

				PrintHorrorzontal();
                            cout << temp << endl;
                        }
                        else
                        {
                            cout << "ERR: No message under this number found.\n" << endl;
                        }
                    }
                    else
                    {
                        cout << "ERR: No messages for this user found.\n" << endl;
                    }

                    break;}
				case 4:{
					/* CASE DEL */
			PrintHorrorzontal();
					cout << "Please enter a user name: ";
                    fgets(buffer, BUF, stdin);
                    send(create_socket, buffer, strlen(buffer), 0);


                    size = recv(create_socket, buffer, BUF-1, 0);
                    buffer[size] = '\0';
                    temp = buffer;

                    if(temp != "ERR")
                    {
                        cout << "Please enter a post number: ";
                        fgets(buffer, BUF, stdin);
                        send(create_socket, buffer, strlen(buffer), 0);
						temp = "";
                        size = recv(create_socket, buffer, BUF-1, 0);
                        buffer[size] = '\0';
                        temp = buffer;

                        if(temp != "ERR")
                        {
                            cout << "\nMessage deleted\n" << endl;
                        }
                        else
                        {
                            cout << "ERR: No messages under this number.\n" << endl;
                        }
                    }
                    else
                    {
                        cout << "ERR: No messages for this user found.\n" << endl;
                    }

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

void PrintHorrorzontal()
{
	cout << "\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B";
	cout << "\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B";
	cout << "\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B";
	cout << "\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B";
	cout << "\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B";
	cout << "\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B" << endl;
}

