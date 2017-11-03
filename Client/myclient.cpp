/* myclient.cpp */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <termios.h>
#define BUF 1024

using namespace std;

// Function to print pretty figures
void PrintHorrorzontal();
void HideStdinKeystrokes();
void ShowStdinKeystrokes();

int main (int argc, char **argv)
{
	int create_socket;
	char buffer[BUF], receiver[BUF];
	struct sockaddr_in address;
	int size;
	int menu;
	string temp;
	int number_of_messages;

	// On missing arguments display usage error message
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

	// Error / Confirmation message on (un-)successful connection
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
		// Print Menu
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

		// Send operation to server
		fgets(buffer, BUF, stdin);
		send(create_socket, buffer, strlen(buffer), 0);

		// Receive confirmation from server
		size = recv(create_socket, receiver, BUF-1, 0);

		// Czech if received message contains anything
		if( size > 0)
		{
            // Convert the received character array to integer
			receiver[size] = '\0';
			menu = atoi(receiver);

			// Perform respective case
			switch(menu)
			{
				case 1:{
                    /*--------------------------*/
                    /*      SEND OPERATION      */
                    /*--------------------------*/

					PrintHorrorzontal();

					// Send receivername to server
					// Repeat until server check returns OK
					do
					{
						cout << "Please enter receiver name: ";

						// Send receivername
						fgets(buffer, BUF, stdin);
						send(create_socket, buffer, strlen(buffer), 0);

						// Receive check result from server
						size = recv(create_socket, receiver, BUF-1, 0);
						receiver[size] = '\0';
						temp = receiver;

						if(temp == "ERR")
						{
							cout << "ERR: Max 8 Characters and no Special Characters" << endl; // ERROR MESSAGE
						}
					} while(temp == "ERR");

					// Send object to server
					// Repeat until server check returns OK
					do
					{
						cout << "Please enter object: ";

						// Send object
						fgets(buffer, BUF, stdin);
						send(create_socket, buffer, strlen(buffer), 0);

						// Receive check result from server
						size = recv(create_socket, receiver, BUF-1, 0);
						receiver[size] = '\0';
						temp = receiver;

						if(temp == "ERR")
						{
							cout << "ERR: Max 80 Characters" << endl; // ERROR MESSAGE
						}
					} while(temp == "ERR");

					cout << "Please enter your message:" << endl;

					// Send message in multiple lines
					do
					{
						fgets(buffer, BUF, stdin);
						send(create_socket, buffer, strlen(buffer), 0);
					} while((buffer[0] != '.') && (buffer[1] != '\n'));

					// Receive check result from server
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
                    /*--------------------------*/
                    /*      LIST OPERATION      */
                    /*--------------------------*/

                    // Signal server to continue sending
                    send(create_socket, buffer, strlen(buffer), 0);

                    // Receive check result from server
                    size = recv(create_socket, receiver, BUF-1, 0);
                    receiver[size] = '\0';
                    temp = receiver;

                    if(temp != "ERR")
                    {
                        // Signal server to continue sending
                        send(create_socket, buffer, strlen(buffer), 0);

                        // Extract number of messages from received string
                        number_of_messages = temp.length() > 0 ? stoi(temp) : 0;
                        PrintHorrorzontal();

                        // Loop and receive sender and object from the server
                        for(int i = 0; i < number_of_messages; ++i)
                        {
                            // Print messagenumber
                            cout << "#" << i + 1;

                            // Receive sender from server
                            size = recv(create_socket, receiver, BUF-1, 0);
                            receiver[size] = '\0';

                            // Signal server to continue sending
                            send(create_socket, buffer, strlen(buffer), 0);
                            temp = receiver;

                            // Print sender
                            cout << ": Sender: " << temp;

                            // Receive object from server
                            size = recv(create_socket, receiver, BUF-1, 0);
                            receiver[size] = '\0';

                            // Signal server to continue sending
                            send(create_socket, buffer, strlen(buffer), 0);
                            temp = receiver;

                            // Print object
                            cout << ", Object: " << temp << endl;
                        }
                    }
                    else
                    {
                        cout << "ERR: No messages for this user found.\n" << endl;
                    }

                    break;}
				case 3:{
                    /*--------------------------*/
                    /*      READ OPERATION      */
                    /*--------------------------*/

                    send(create_socket, buffer, strlen(buffer), 0);

                    // Receive check result from server
                    size = recv(create_socket, buffer, BUF-1, 0);
                    buffer[size] = '\0';
                    temp = buffer;

                    if(temp != "ERR")
                    {
                        PrintHorrorzontal();
                        cout << "Please enter a post number: ";

                        // Send post number to server
                        fgets(buffer, BUF, stdin);
                        send(create_socket, buffer, strlen(buffer), 0);
                        temp = "";

                        // Receive check result from server
                        size = recv(create_socket, buffer, BUF-1, 0);
                        buffer[size] = '\0';
                        temp = buffer;

                        if(temp != "ERR")
                        {
                            temp = "";

                            // Send OK to server to signal it to start sending
                            send(create_socket, buffer, strlen(buffer), 0);

                            // Receive an output string from the server
                            // coming in 1024 Bit chunks
                            // Repeat until ".\n" was received
                            do
                            {
                                // Receive one chunk of the message
                                size = recv(create_socket, buffer, BUF-1, 0);
                                buffer[size] = '\0';

                                if((buffer[0] != '.') && (buffer[1] != '\n'))
                                {
                                    // Append received chunks to a temporary string
                                    // if they are legit pieces of information
                                    temp += buffer;
                                }

                                // Signal the server to send the next chunk
                                send(create_socket, buffer, strlen(buffer), 0);
                            } while((buffer[0] != '.') && (buffer[1] != '\n') && (temp != "ERR"));

                            PrintHorrorzontal();

                            // Print out the complete string containing all information
                            cout << temp;

                        	PrintHorrorzontal();

                        	cout << "OK: Message read" << endl;
                        }
                        else
                        {
                        	PrintHorrorzontal();
                            cout << "ERR: No message with this number found." << endl;
                        }
                    }
                    else
                    {
                        PrintHorrorzontal();
                        cout << "ERR: No messages for this user found." << endl;
                    }

                    break;}
				case 4:{
                    /*--------------------------*/
                    /*       DEL OPERATION      */
                    /*--------------------------*/

                    send(create_socket, buffer, strlen(buffer), 0);

                    // Receive check result from server
                    size = recv(create_socket, buffer, BUF-1, 0);
                    buffer[size] = '\0';
                    temp = buffer;

                    if(temp != "ERR")
                    {
                        PrintHorrorzontal();
                        cout << "Please enter a post number: ";

                        // Send post number to server
                        fgets(buffer, BUF, stdin);
                        send(create_socket, buffer, strlen(buffer), 0);

						temp = "";

						// Receive check result from server
                        size = recv(create_socket, buffer, BUF-1, 0);
                        buffer[size] = '\0';
                        temp = buffer;

                        if(temp != "ERR")
                        {
                        	PrintHorrorzontal();
                            cout << "OK: Message deleted" << endl;
                        }
                        else
                        {
                        	PrintHorrorzontal();
                            cout << "ERR: No message with this number found." << endl;
                        }
                    }
                    else
                    {
                        PrintHorrorzontal();
                        cout << "ERR: No messages for this user found." << endl;
                    }

					break;}
				case 5:{
                    /*-------------------------*/
                    /*     LOGIN OPERATION     */
                    /*-------------------------*/

	                send(create_socket, buffer, strlen(buffer), 0);

				    // Receive response from Server
				    size = recv(create_socket, buffer, BUF-1, 0);
				    buffer[size] = '\0';
				    temp = buffer;

				    if(temp == "OK")
				    {
	                    PrintHorrorzontal();
						cout << "Please enter your user name: ";

						// Send username to Server
	                    fgets(buffer, BUF, stdin);
	                    send(create_socket, buffer, strlen(buffer), 0);

						cout << "Please enter your password: ";

						HideStdinKeystrokes();

						// Send password to Server
	                    fgets(buffer, BUF, stdin);
	                    send(create_socket, buffer, strlen(buffer), 0);

	                    ShowStdinKeystrokes();

						cout << endl;

					    // Receive response from Server
					    size = recv(create_socket, buffer, BUF-1, 0);
					    buffer[size] = '\0';
	                    PrintHorrorzontal();
					    cout << buffer << endl;
					}
					else
					{
						cout << buffer << endl;
					}

					break;}
				case 6:{
                    /*-------------------------*/
                    /* PLEASE LOGIN OPERATION  */
                    /*-------------------------*/

					cout << "Please 'Login' to use other Operations." << endl;
					break;}
				default:{
                    /*-------------------------*/
                    /*       NO OPERATION      */
                    /*-------------------------*/

					cout << "Invalid Operation" << endl;
					break;}
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

void HideStdinKeystrokes()
{
    termios tty;

    tcgetattr(STDIN_FILENO, &tty);

    /* we want to disable echo */
    tty.c_lflag &= ~ECHO;

    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

void ShowStdinKeystrokes()
{
   termios tty;

    tcgetattr(STDIN_FILENO, &tty);

    /* we want to reenable echo */
    tty.c_lflag |= ECHO;

    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}