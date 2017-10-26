/* myserver.cpp */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <regex>
#include <cstdio>
#include <dirent.h> // Make Directory
#define BUF 1024
#define PORT 6543

using namespace std;

// Some functions used in the program
string StringToLower(char value[]);
string CutFromString(string input, string pattern);
int StringNewlineOnly(string input);

int main (int argc, char **argv)
{
	int create_socket, new_socket;
	socklen_t addrlen;
	char buffer[BUF];
	int size;
	struct sockaddr_in address, cliaddress;

    // Regex for the serverstart input
	string dir_regex = "^[a-zA-Z0-9_+.,]{1,200}/$";
    // Regex for the username input
	string user_regex = "^[a-zA-Z0-9]{1,8}$";
	// Temporary variable used every now and then
	string temp;

	create_socket = socket (AF_INET, SOCK_STREAM, 0);

    // If not enough arguments are delivered, the program exits
	if(argc < 3)
	{
		cout << "Missing Arguments: " << argv[0] << " Port Number + Mailspookdirectory" << endl;
		cout << "Maybe try 'data/' for a directory :)" << endl;
		exit(EXIT_FAILURE);
	}

    // Path to mailspooldirectory
	string m_path = argv[2]; //"data/";
	string combo_path = "./" + m_path;
	DIR* dir = opendir(combo_path.c_str());
	int serv_port = atoi(argv[1]);

	if(!((1024 < serv_port) && (serv_port < 65536)) || (!regex_match(m_path, regex(dir_regex))))
	{
		cout << "ERROR: Port number not within 1024-65535 range or directory schreibweis foisch" << endl;
		cout << "Maybe try 'my_directory/' for a directory :)" << endl;
		exit(EXIT_FAILURE);
	}

    // Check if msdirectory exists; if yes, good, if not, create it
	if (dir)
	{
	    /* Directory exists. */
	    closedir(dir);
	}
	else if (ENOENT == errno)
	{
	    /* Directory does not exist. */
		mkdir(combo_path.c_str(), 0777);
	}

	memset(&address,0,sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons (serv_port);

	// Arbitrary variable needed for setsockopt
	int x = 1;
	// To prevent the port from locking up, we tell the program to use it again
	setsockopt(create_socket, SOL_SOCKET, SO_REUSEADDR, &x, sizeof(int*));

	if (bind ( create_socket, (struct sockaddr *) &address, sizeof (address)) != 0)
	{
		perror("bind error");
		return EXIT_FAILURE;
	}

	listen (create_socket, 5);

	addrlen = sizeof (struct sockaddr_in);

	while (1)
	{
		printf("Waiting for connections...\n");
		new_socket = accept ( create_socket, (struct sockaddr *) &cliaddress, &addrlen );

		if (new_socket > 0)
		{
            // Send a warm welcome to the client
			temp = "Welcome to myserver, please enter your command:\n";
			cout << "Client connected from " << inet_ntoa(cliaddress.sin_addr) << ":" << ntohs(cliaddress.sin_port)<< "..." << endl;
			strncpy(buffer, temp.c_str(), sizeof(buffer));
			send(new_socket, temp.c_str(), strlen(buffer),0);
		}

		do
		{
            // Reset buffer
            memset(buffer, 0, sizeof(buffer));
			size = recv(new_socket, buffer, BUF-1, 0);

			if( size > 0)
			{
				buffer[size] = '\0';
				temp = StringToLower(buffer);
				cout << "Message received: " << temp << endl;

				// Determine switch case
				int input_type = temp == "send" ? 1 : temp == "list" ? 2 : temp == "read" ? 3 : temp == "del" ? 4 : temp == "quit" ? 5 : 6;

				// Variables for SEND case
				string send_information[4];
				bool check = false;
				ifstream MessageIn;
				ofstream MessageOut;
				string file_content = "";
				string file_substr = "";
				string file_output = "";
				int message_number = 0;

				// Variables for LIST case
				string list_user = "";
				int number_of_messages = 0;
				string list_sender = "";
				string list_subject = "";
				char dump_array[1] = {'a'};

				// Variables for READ case
				string read_user = "";
				int read_postnumber = 0;
				string read_sender = "";
				string read_subject = "";
				string read_message = "";
				string read_result = "";
				int read_subend = 0;

				// Variables for DEL case
				string delete_user = "";
				int delete_postnumber = 0;
				string delete_output = "";
				int delete_index = 0;

				switch(input_type)
				{
					case 1:{
						/*--------------------------*/
						/*      SEND OPERATION      */
						/*--------------------------*/

						// Confirm SEND case to client
						strncpy(buffer,"1", sizeof(buffer));
						send(new_socket, buffer, strlen(buffer),0);

						// Get sender name
						// Repeat until input is valid (regex)
						do
						{
							size = recv(new_socket, buffer, BUF-1, 0);
							buffer[size - 1] = '\0';
							send_information[0] = buffer;

							check = !(regex_match(send_information[0], regex(user_regex)));

							if(check)
							{
								strncpy(buffer, "ERR", sizeof(buffer));
							}
							else
							{
								strncpy(buffer, "OK", sizeof(buffer));
								cout << "Sender: " << send_information[0] << endl;
							}

							// Send check result to client
							send(new_socket, buffer, strlen(buffer),0);
						} while(check && (size != 0));

						// Get receiver name
						// Repeat until input is valid (regex)
						do
						{
							size = recv(new_socket, buffer, BUF-1, 0);
							buffer[size - 1] = '\0';
							send_information[1] = buffer;

							check = !(regex_match(send_information[1], regex(user_regex)));

							if(check)
							{
								strncpy(buffer, "ERR", sizeof(buffer));
							}
							else
							{
								strncpy(buffer, "OK", sizeof(buffer));
								cout << "Receiver: " << send_information[1] << endl;
							}

							// Send check result to client
							send(new_socket, buffer, strlen(buffer),0);
						} while(check && (size != 0));

						// Get subject
						// Repeat until input is valid (char limit)
						do
						{
							size = recv(new_socket, buffer, BUF-1, 0);
							buffer[size - 1] = '\0';
							temp = buffer;

							// Remove file delimiters from input
							temp = CutFromString(temp, "#####");
							temp = CutFromString(temp, "##*##");
							temp = CutFromString(temp, "#***#");

							// If input was empty, set it to default
							temp = temp.length() == 0 ? "subject" : temp;

							send_information[2] = temp;
							if(size > 81)
							{
								strncpy(buffer, "ERR", sizeof(buffer));
							}
							else
							{
								strncpy(buffer, "OK", sizeof(buffer));
								cout << "subject: " << send_information[2] << endl;
							}

							// Send check result to client
							send(new_socket, buffer, strlen(buffer),0);
						} while((size > 81) && (size != 0));

						// Get message
						// Repeat until the whole message was received
						temp = "";
						do
						{
							size = recv(new_socket, buffer, BUF-1, 0);
							buffer[size] = '\0';

							if((buffer[0] != '.') && (buffer[1] != '\n'))
							{
								temp += buffer;
							}
						} while((buffer[0] != '.') && (buffer[1] != '\n'));

						// Remove file delimiters from input
						temp = CutFromString(temp, "#####");
						temp = CutFromString(temp, "##*##");
						temp = CutFromString(temp, "#***#");

						// If input was (basically) empty, set it to default
						temp = StringNewlineOnly(temp) == 1 ? "Empty Message\n" : temp;

						send_information[3] = temp;
						cout << "Message:\n" << send_information[3];

						// Write to file
						MessageIn.open(m_path + send_information[1] + ".txt");

						if(MessageIn.is_open())
						{
							file_content = "";

                            // Fetch all existing content from file
							while(getline(MessageIn, temp))
							{
								file_content += temp + "\n";
							}

							MessageIn.close();

							// Get number of last existing post
							file_substr = "";
							file_substr = file_content.substr(file_content.find("#***#") + 5, file_content.length());
							file_substr = file_substr.substr(0, file_substr.find("#####"));

							// Replace the "last post" delimiter with a standard one
							file_content.replace(file_content.find("#***#"), 5, "##*##");
						}

						// If any number was found, set it and increase by one
						message_number = file_substr.length() > 0 ? stoi(file_substr) : 0;
						++message_number;

						// Build string to write with inputs and delimiters
						file_output = "#***#" + to_string(message_number) + "#####" + send_information[0] + "#####" + send_information[2] + "#####\n" + send_information[3];

						// Append string to existing content
						file_content += file_output;

						// Open file and write
						MessageOut.open(m_path + send_information[1] + ".txt");

						if(MessageOut.is_open())
						{
							MessageOut << file_content;
							MessageOut.close();

							strncpy(buffer, "OK", sizeof(buffer));
						}
						else
						{
							strncpy(buffer, "ERR", sizeof(buffer));
						}

						send(new_socket, buffer, strlen(buffer),0);

						cout << "OPERATION FINISHED\nWaiting for new Input...\n" << endl;

						break;}
					case 2:{
						/*--------------------------*/
						/*      LIST OPERATION      */
						/*--------------------------*/

						// Confirm LIST case to client
						strncpy(buffer,"2", sizeof(buffer));
						send(new_socket, buffer, strlen(buffer),0);

						// Confirm LIST case to client
                        size = recv(new_socket, buffer, BUF-1, 0);
                        buffer[size - 1] = '\0';
                        list_user = buffer;

						// Open file for given user
                        MessageIn.open(m_path + list_user + ".txt");

                        if(MessageIn.is_open())
						{
							// Fetch content from file
							file_content = "";

							while(getline(MessageIn, temp))
							{
								file_content += temp + "\n";
							}

							MessageIn.close();

                            // Extract number of posts from content
							file_substr = "";
                            file_substr = file_content.substr(file_content.find("#***#") + 5, file_content.length());
                            file_substr = file_substr.substr(0, file_substr.find("#####"));
                            number_of_messages = file_substr.length() > 0 ? stoi(file_substr) : 0;

                            // Send number of posts to client
                            strncpy(buffer, file_substr.c_str(), sizeof(buffer));
							send(new_socket, buffer, strlen(buffer),0);

                            // Receive confirmation from client
                            recv(new_socket, dump_array, BUF-1, 0);

                            file_substr = file_content.substr(file_content.find("#####") + 5, file_content.length());

                            // Prepare content and send to client
							for(int i = 0; i < number_of_messages; ++i)
							{
                            	// Prepare and send the sender information
                                list_sender = file_substr.substr(0, file_substr.find("#####"));
                                file_substr = file_substr.substr(file_substr.find("#####") + 5, file_substr.length());

                                strncpy(buffer, list_sender.c_str(), sizeof(buffer));
                                send(new_socket, buffer, strlen(buffer),0);
                                recv(new_socket, dump_array, BUF-1, 0);

                            	// Prepare and send the subject information
                                list_subject = file_substr.substr(0, file_substr.find("#####"));
                                file_substr = file_substr.substr(file_substr.find("#####") + 5, file_substr.length());

                                strncpy(buffer, list_subject.c_str(), sizeof(buffer));
                                send(new_socket, buffer, strlen(buffer),0);
                                recv(new_socket, dump_array, BUF-1, 0);

                            	// Cut off unused parts of the content
                                file_substr = file_substr.substr(file_substr.find("#####") + 5, file_substr.length());
                            }

                            cout << "OPERATION FINISHED\nWaiting for new Input...\n" << endl;
						}
						else
						{
                            // Could not open file; Send err to client
                            strncpy(buffer, "ERR", sizeof(buffer));
							send(new_socket, buffer, strlen(buffer),0);
						}

						break;}
					case 3:{
						/*--------------------------*/
						/*      READ OPERATION      */
						/*--------------------------*/

						// Confirm READ case to client
						strncpy(buffer,"3", sizeof(buffer));
						send(new_socket, buffer, strlen(buffer),0);

                        // Receive username from client
                        size = recv(new_socket, buffer, BUF-1, 0);
                        buffer[size - 1] = '\0';
                        read_user = buffer;

                        // Open file for this user
                        MessageIn.open(m_path + read_user + ".txt");

                        if(MessageIn.is_open())
						{
                            // Send success message to client
                            temp = "OK";
                            strncpy(buffer,temp.c_str(), sizeof(buffer));
                            send(new_socket, buffer, strlen(buffer),0);

                            // Receive post number from client
                            size = recv(new_socket, buffer, BUF-1, 0);
                            buffer[size - 1] = '\0';
                            temp = buffer;
                            read_postnumber = temp.length() > 0 ? stoi(temp) : 0;

							// Fetch all content from file
							file_content = "";

							while(getline(MessageIn, temp))
							{
								file_content += temp + "\n";
							}

							MessageIn.close();

                            // Extract number of posts from content
							file_substr = "";

                            file_substr = file_content.substr(file_content.find("#***#") + 5, file_content.length());
                            file_substr = file_substr.substr(0, file_substr.find("#####"));
                            number_of_messages = file_substr.length() > 0 ? stoi(file_substr) : 0;

                            // Check if post number is present in current content
                            if((0 < read_postnumber) && (read_postnumber <= number_of_messages))
                            {
                                // Send success message to client
                                temp = "OK";
                                strncpy(buffer,temp.c_str(), sizeof(buffer));
                                send(new_socket, buffer, strlen(buffer),0);

                                // Cut off the string up to the first relevant content
                                file_substr = file_content.substr(file_content.find("#####") + 5, file_content.length());

                                // Loop through the content and write the parts specified by the post number
                                for(int i = 0; i < number_of_messages; ++i)
                                {
                                    // Check if current loop equals the given post number and write variable if it does
                                    read_sender = (i + 1) == read_postnumber ? file_substr.substr(0, file_substr.find("#####")) : read_sender;
                                    file_substr = file_substr.substr(file_substr.find("#####") + 5, file_substr.length());

                                    // Check if current loop equals the given post number and write variable if it does
                                    read_subject = (i + 1) == read_postnumber ? file_substr.substr(0, file_substr.find("#####")) : read_subject;
                                    file_substr = file_substr.substr(file_substr.find("#####") + 5, file_substr.length());

                                    // If the current loop represents the last message, set delimiter to the "last-post-delimiter"
                                    if((i + 2) != number_of_messages)
                                    {
                                        read_message = (i + 1) == read_postnumber ? file_substr.substr(0, file_substr.find("##*##")) : read_message;
                                    }
                                    else
                                    {
                                        read_message = (i + 1) == read_postnumber ? file_substr.substr(0, file_substr.find("#***#")) : read_message;
                                    }

                                    // Cut off irrelevant content
                                    file_substr = file_substr.substr(file_substr.find("#####") + 5, file_substr.length());
                                }

                                // Build a master string containing all values
                                read_result = "Sender: " + read_sender + "\nsubject: " + read_subject + "\nMessage:" + read_message;
                                cout << read_result << endl;

                                // Receive OK from client to start sending
                                recv(new_socket, buffer, BUF-1, 0);

                                // Send master string in 1024 Bit blocks until the master string is empty
                                while(read_result.length() > 1)
                                {
                                    // Check if remaining string is longer than 1024 characters and use 1024 if it is
                                    read_subend = read_result.length() > BUF ? BUF : read_result.length();

                                    // Write master string to temp string until char limit is reached
                                    temp = read_result.substr(0, read_subend);

                                    // Cut off content that has already been prepared to be sent
                                    read_result = read_result.substr(read_subend, read_result.length());

                                    // Send prepared string
                                    strncpy(buffer,temp.c_str(), sizeof(buffer));
                                    send(new_socket, buffer, strlen(buffer),0);

                                    // Wait for OK from client
                                    size = recv(new_socket, buffer, BUF-1, 0);
                                }

                                cout << "OPERATION FINISHED\nWaiting for new Input...\n" << endl;

                                // Send FINISH to client so they can stop receiving data
                                strncpy(buffer, ".\n", sizeof(buffer));
                                send(new_socket, buffer, strlen(buffer),0);

                                // Wait for OK from client
                                size = recv(new_socket, buffer, BUF-1, 0);
                            }
                            else
                            {
                                // Post number was not contained in current file
                                strncpy(buffer, "ERR", sizeof(buffer));
                                send(new_socket, buffer, strlen(buffer),0);
                            }
                        }
                        else
                        {
                            // Could not open file for user
                            strncpy(buffer, "ERR", sizeof(buffer));
							send(new_socket, buffer, strlen(buffer),0);
                        }
						break;}
					case 4:{
						/*--------------------------*/
						/*     DELETE OPERATION     */
						/*--------------------------*/

						// Confirm DEL case to client
						strncpy(buffer,"4", sizeof(buffer));
						send(new_socket, buffer, strlen(buffer),0);

						// Receive username from client
                        size = recv(new_socket, buffer, BUF-1, 0);
                        buffer[size - 1] = '\0';
                        delete_user = buffer;

                        // Open file for this user
                        MessageIn.open(m_path + delete_user + ".txt");

                        if(MessageIn.is_open())
						{
                            // Send success message to client
                            strncpy(buffer, "OK", sizeof(buffer));
							send(new_socket, buffer, strlen(buffer),0);

							// Receive post number from client
                            size = recv(new_socket, buffer, BUF-1, 0);
                            buffer[size - 1] = '\0';
                            temp = buffer;
                            delete_postnumber = temp.length() > 0 ? stoi(temp) : 0;

							// Fetch all content from file
							file_content = "";

							while(getline(MessageIn, temp))
							{
								file_content += temp + "\n";
							}

							MessageIn.close();

                            // Extract number of posts from content
							file_substr = "";
                            file_substr = file_content.substr(file_content.find("#***#") + 5, file_content.length());
                            file_substr = file_substr.substr(0, file_substr.find("#####"));
                            number_of_messages = file_substr.length() > 0 ? stoi(file_substr) : 0;

                            // Check if there is only one message in the file
                            // and if its number was specified by the user
                            if((number_of_messages == 1) && (delete_postnumber == 1))
							{
                                // Send success message to client
                                strncpy(buffer, "OK", sizeof(buffer));
                                send(new_socket, buffer, strlen(buffer),0);

                                // Since there is only one message in the file
                                // and it's supposed to be deleted,
                                // simply delete the file
								temp = m_path + delete_user + ".txt";
								remove(temp.c_str());
								cout << "OPERATION FINISHED\nWaiting for new Input...\n" << endl;
							}
							// Check if post number is present in current content
							else if((0 < delete_postnumber) && (delete_postnumber <= number_of_messages))
                            {
                                // Send success message to client
                                strncpy(buffer, "OK", sizeof(buffer));
                                send(new_socket, buffer, strlen(buffer),0);

                                // Cut off the string up to the first relevant content
                                file_substr = file_content.substr(file_content.find("#####"), file_content.length());

                                // Loop through the content and write the parts not specified by the post number
                                for(int i = 0; i < number_of_messages; ++i)
                                {
                                    // If the current loop represents the second to last message,
                                    // set find delimiter to the "last-post-delimiter"
                                    temp = (i + 2) == number_of_messages ? "#***#" : "##*##";

                                    // If the current loop represents the post specified by
                                    // the given post number, don't append it to the string
									if((i + 1) != delete_postnumber)
									{
                                        // If i is the second to last of all messages, start it with
                                        // the "last-post-delimiter"
										delete_output += (i + 2) >= number_of_messages ? "#***#" : "##*##";

										// Append current delete index to create a
										// subsequent enumeration for the posts
										delete_output += to_string(delete_index + 1);

										// Increase delete index
										++delete_index;

										// Append the post from start of sender to end of message
										delete_output += file_substr.substr(0, file_substr.find(temp));
									}

									// Cut off irrelevant content
									file_substr = file_substr.substr(file_substr.find(temp) + 5, file_substr.length());
									file_substr = file_substr.substr(file_substr.find("#####"), file_substr.length());
                                }

                                // Open file and write
								MessageOut.open(m_path + delete_user + ".txt");

								if(MessageOut.is_open())
								{
									MessageOut << delete_output;
									MessageOut.close();

									cout << "Message deleted" << endl;
								}
								else
								{
									cout << "Could not write file." << endl;
								}

								cout << "OPERATION FINISHED\nWaiting for new Input...\n" << endl;
                            }
                            else
                            {
                                // Post number was not contained in current file
                                strncpy(buffer, "ERR", sizeof(buffer));
                                send(new_socket, buffer, strlen(buffer),0);
                            }
                        }
                        else
                        {
                            // Could not open file for this user
                            strncpy(buffer, "ERR", sizeof(buffer));
							send(new_socket, buffer, strlen(buffer),0);
                        }
						break;}
					case 5:{
						/*--------------------------*/
						/*      QUIT OPERATION      */
						/*--------------------------*/

						// No return value for the client, since there is no one anymore
						cout << "Client disconnected from the server.\n" << endl;

						break;}
					default:{
						/*--------------------------*/
						/*       NO OPERATION       */
						/*--------------------------*/

						// Confirm NO case to client
						strncpy(buffer,"0", sizeof(buffer));
						send(new_socket, buffer, strlen(buffer),0);

						cout << "No Matching Operation Found" << endl;
						break;}
				}
			}
			else if (size == 0)
			{
				printf("Client closed remote socket\n");
				break;
			}
			else
			{
				perror("recv error");
				return EXIT_FAILURE;
			}
		} while (strncmp (buffer, "quit", 4) != 0);

		close (new_socket);
	}

	close (create_socket);
	return EXIT_SUCCESS;
}

// Function to set every character in a char array to lower case
string StringToLower(char value[])
{
	string output = "";
	int i = 0;

	output.resize(BUF);

	do
	{
		output[i] = tolower(value[i]);
		++i;
	} while((value[i] != '\0') && (value[i] != '\n'));

	output.resize(i);

	return output;
}

// Function to cut a specific pattern from an input string
string CutFromString(string input, string pattern)
{
    string output = "";
    int loop = 0;

    if(input.find(pattern) != string::npos)
    {
        do
        {
            output += input.substr(0, input.find(pattern));
            input = loop == 2 ? input : input.substr(input.find(pattern) + pattern.length(), input.length());
            loop = input.find(pattern) != string::npos ? 1 : (loop == 2 ? 0 : 2);
        }while(loop > 0);
    }
    else
    {
        output = input;
    }

	return output;
}

// Function to check if a string consists of new line characters only
int StringNewlineOnly(string input)
{
    int nl_only = 1;

    if(input.length() > 0)
    {
        for(size_t i = 0; i < (input.length() - 1); ++i)
        {
            nl_only = input[i] != '\n' ? 0 : nl_only;
        }
    }

    return nl_only;
}

