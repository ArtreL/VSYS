/* myclient.cpp */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <termios.h>
#include <fstream>
#include <dirent.h> // Make Directory
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
    ifstream Attachment;
    ofstream Attoutment;
    bool check = false;
    int att_length;
    int att_loop;
    int att_index;


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

					do
					{
						cout << "Would you like to add an attachment? [Y/N] ";

						// Send selection
						fgets(buffer, BUF, stdin);
						send(create_socket, buffer, strlen(buffer), 0);
						temp = buffer;

						if((temp == "y\n") || (temp == "Y\n"))
						{
							cout << "Please enter the path to the file you'd like to send: ";

							// Send filepath
							fgets(buffer, BUF, stdin);
							send(create_socket, buffer, strlen(buffer),0);
							recv(create_socket, receiver, BUF-1, 0);
							temp = buffer;
							temp[temp.length() - 1] = '\0';

							Attachment.open(temp, ios_base::binary);

							if(Attachment.is_open())
							{

							    Attachment.seekg (0, Attachment.end);
							    int length = Attachment.tellg();
							    Attachment.seekg (0, Attachment.beg);

							    char* att_content = new char[length];

							    Attachment.read(att_content, length);

								int att_length = (length / BUF) + 1;

                        		strncpy(buffer, to_string(length).c_str(), sizeof(buffer));
								send(create_socket, buffer, strlen(buffer), 0);

								recv(create_socket, receiver, BUF-1, 0);

								int att_index = 0;
								int att_end = 0;

							    // Send master string in 1024 Bit blocks until the master string is empty
			                    for(int i = 0; i < att_length; ++i)
			                    {	
                        			cout << i << endl;
			                    	att_end = (att_index + BUF) < length ? BUF : (length - att_index);
			                    	copy(att_content + att_index, att_content + att_index + att_end, buffer + 0);
			                    	att_index = att_index + BUF;

			                        send(create_socket, buffer, att_end, 0);

									recv(create_socket, receiver, BUF-1, 0);
			                    }

								delete att_content;
							}

							Attachment.close();

							check = false;
						}
						else if (temp == "n\n" || temp == "N\n")
						{
							cout << "No attachment will be added." << endl;
							check = false;
						}
						else
						{
							cout << "dafuq?" << endl;
							check = true;
						}

					}while(check);

					// Send subject to server
					// Repeat until server check returns OK
					do
					{
						cout << "Please enter subject: ";

						// Send subject
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

                        // Loop and receive sender and subject from the server
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

                            // Receive subject from server
                            size = recv(create_socket, receiver, BUF-1, 0);
                            receiver[size] = '\0';

                            // Signal server to continue sending
                            send(create_socket, buffer, strlen(buffer), 0);
                            temp = receiver;

                            // Print subject
                            cout << ", Subject: " << temp << endl;
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
                            string read_content = "";

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
                                    read_content += buffer;
                                }

                                // Signal the server to send the next chunk
                                send(create_socket, buffer, strlen(buffer), 0);
                            } while((buffer[0] != '.') && (buffer[1] != '\n') && (read_content != "ERR"));

                            if(read_content.find("Attachment: ") != string::npos)
                            {
	                            string sender = read_content.substr(8, read_content.find('\n') - 8);
	                            string att_name = read_content.substr(read_content.find("Attachment: ") + 12, read_content.find("\nMessage:") - (read_content.find("Attachment: ") + 12));


	                            temp = sender + "/";
			                    DIR* dir = opendir(temp.c_str());

			                    // Check if msdirectory exists; if yes, good, if not, create it
			                    if (dir)
			                    {
			                        /* Directory exists. */
			                        closedir(dir);
			                    }
			                    else if (ENOENT == errno)
			                    {
			                        /* Directory does not exist. */
			                        mkdir(temp.c_str(), 0777);
			                    }

			                    size = recv(create_socket, buffer, BUF-1, 0);
			                    buffer[size] = '\0';
			                    temp = buffer;

			                    if(temp != "ERR")
			                    {
				                    att_length = stoi(temp);
				                    att_loop = (att_length / BUF) + 1;

				                    strncpy(buffer,"1", sizeof(buffer));
				                    send(create_socket, buffer, strlen(buffer),0);

				                    char* att_out = new char[att_length];
				                    att_index = 0;
				                    cout << att_loop << endl;

				                    for(int i = 0; i < att_loop; ++i)
				                    {
                                		if(i % 100 == 0) cout << i << endl;
				                        size = recv(create_socket, buffer, BUF, 0);

				                        copy(buffer, buffer + size, att_out + att_index);
				                        att_index = (att_index + size) < att_length ? (att_index + size) : (att_length - att_index);

				                        strncpy(buffer,"1", sizeof(buffer));
				                        send(create_socket, buffer, strlen(buffer), 0);
				                    }

				                    Attoutment.open(sender + "/" + att_name);

				                    Attoutment.write(att_out, att_length);

				                    Attoutment.close();

				                    delete att_out;

				                	cout << "OK: Attachment has been saved to your inbox." << endl;
				                }
				                else
				                {
				                	cout << "ERR: Could not open file." << endl;
				                }
				            }

                            PrintHorrorzontal();

                            // Print out the complete string containing all information
                            cout << read_content;

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

// just some styling
void PrintHorrorzontal()
{
	cout << "\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B";
	cout << "\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B";
	cout << "\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B";
	cout << "\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B";
	cout << "\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B";
	cout << "\e(0\x71\e(B\e(0\x71\e(B\e(0\x71\e(B" << endl;
}

// hide entered input, gets called before password is being typed
void HideStdinKeystrokes()
{
    termios tty;

    tcgetattr(STDIN_FILENO, &tty);

    /* we want to disable echo */
    tty.c_lflag &= ~ECHO;

    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

// show entered input again after password has been entered
void ShowStdinKeystrokes()
{
   termios tty;

    tcgetattr(STDIN_FILENO, &tty);

    /* we want to reenable echo */
    tty.c_lflag |= ECHO;

    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}