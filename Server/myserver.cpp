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
	address.sin_port = htons (atoi(argv[1]));

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

				int input_type = temp == "send" ? 1 : temp == "list" ? 2 : temp == "read" ? 3 : temp == "del" ? 4 : temp == "quit" ? 5 : 6;

				string send_information[4];
				bool check = false;
				ifstream MessageIn;
				ofstream MessageOut;
				string file_content = "";
				string file_substr = "";
				string file_output = "";
				int message_number = 0;

				string list_user = "";
				int number_of_messages = 0;
				string list_sender = "";
				string list_object = "";
				char dump_array[1] = {'a'};

				string read_user = "";
				int read_postnumber = 0;
				string read_sender = "";
				string read_object = "";
				string read_message = "";
				string read_result = "";
				int read_subend = 0;

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

						strncpy(buffer,"1", sizeof(buffer));
						send(new_socket, buffer, strlen(buffer),0);

						/*     GET SENDER NAME     */
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
							send(new_socket, buffer, strlen(buffer),0);
						} while(check && (size != 0));

						/*   GET RECEIVER NAME   */
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
							send(new_socket, buffer, strlen(buffer),0);
						} while(check && (size != 0));

						/*     GET OBJECT     */
						do
						{
							size = recv(new_socket, buffer, BUF-1, 0);
							buffer[size - 1] = '\0';
							temp = buffer;
							temp = CutFromString(temp, "#####");
							temp = CutFromString(temp, "##*##");
							temp = CutFromString(temp, "#***#");
							temp = temp.length() == 0 ? "object" : temp;

							send_information[2] = temp;
							if(size > 81)
							{
								strncpy(buffer, "ERR", sizeof(buffer));
							}
							else
							{
								strncpy(buffer, "OK", sizeof(buffer));
								cout << "Object: " << send_information[2] << endl;
							}
							send(new_socket, buffer, strlen(buffer),0);
						} while((size > 81) && (size != 0));

						/*     GET MESSAGE     */
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

						temp = CutFromString(temp, "#####");
						temp = CutFromString(temp, "##*##");
						temp = CutFromString(temp, "#***#");
						temp = StringNewlineOnly(temp) == 1 ? "Empty Message\n" : temp;

						send_information[3] = temp;
						cout << "Message:\n" << send_information[3];

						/*     WRITE TO FILE     */
						MessageIn.open(m_path + send_information[1] + ".txt");

						if(MessageIn.is_open())
						{
							file_content = "";

							while(getline(MessageIn, temp))
							{
								file_content += temp + "\n";
							}

							MessageIn.close();
							file_substr = "";
							file_substr = file_content.substr(file_content.find("#***#") + 5, file_content.length());
							file_substr = file_substr.substr(0, file_substr.find("#####"));
							file_content.replace(file_content.find("#***#"), 5, "##*##");
						}
						message_number = 0;
						message_number = file_substr.length() > 0 ? stoi(file_substr) : 0;
						++message_number;

						file_output = "#***#" + to_string(message_number) + "#####" + send_information[0] + "#####" + send_information[2] + "#####\n" + send_information[3];

						file_content += file_output;

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

						strncpy(buffer,"2", sizeof(buffer));
						send(new_socket, buffer, strlen(buffer),0);

                        size = recv(new_socket, buffer, BUF-1, 0);
                        buffer[size - 1] = '\0';
                        list_user = buffer;

                        MessageIn.open(m_path + list_user + ".txt");

                        if(MessageIn.is_open())
						{
							file_content = "";

							while(getline(MessageIn, temp))
							{
								file_content += temp + "\n";
							}

							MessageIn.close();

							file_substr = "";
                            file_substr = file_content.substr(file_content.find("#***#") + 5, file_content.length());
                            file_substr = file_substr.substr(0, file_substr.find("#####"));
                            number_of_messages = file_substr.length() > 0 ? stoi(file_substr) : 0;
                            strncpy(buffer, file_substr.c_str(), sizeof(buffer));
                            // SEND NUMBER TO CLIENT HERE
							send(new_socket, buffer, strlen(buffer),0);
                            recv(new_socket, dump_array, BUF-1, 0);

                            file_substr = file_content.substr(file_content.find("#####") + 5, file_content.length());

							for(int i = 0; i < number_of_messages; ++i)
							{
                                list_sender = file_substr.substr(0, file_substr.find("#####"));
                                file_substr = file_substr.substr(file_substr.find("#####") + 5, file_substr.length());
                                // SEND SENDER TO CLIENT HERE
                                strncpy(buffer, list_sender.c_str(), sizeof(buffer));
                                send(new_socket, buffer, strlen(buffer),0);
                                recv(new_socket, dump_array, BUF-1, 0);

                                list_object = file_substr.substr(0, file_substr.find("#####"));
                                file_substr = file_substr.substr(file_substr.find("#####") + 5, file_substr.length());
                                // SEND OBJECT TO CLIENT HERE
                                strncpy(buffer, list_object.c_str(), sizeof(buffer));
                                send(new_socket, buffer, strlen(buffer),0);
                                recv(new_socket, dump_array, BUF-1, 0);

                                file_substr = file_substr.substr(file_substr.find("#####") + 5, file_substr.length());
                            }

                            cout << "OPERATION FINISHED\nWaiting for new Input...\n" << endl;
						}
						else
						{
                            // SEND ERROR TO CLIENT
                            strncpy(buffer, "ERR", sizeof(buffer));
							send(new_socket, buffer, strlen(buffer),0);
						}

						break;}
					case 3:{
						/*--------------------------*/
						/*      READ OPERATION      */
						/*--------------------------*/

						strncpy(buffer,"3", sizeof(buffer));
						send(new_socket, buffer, strlen(buffer),0);

                        size = recv(new_socket, buffer, BUF-1, 0);
                        buffer[size - 1] = '\0';
                        read_user = buffer;

                        MessageIn.open(m_path + read_user + ".txt");

                        if(MessageIn.is_open())
						{
                            temp = "OK";
                            strncpy(buffer,temp.c_str(), sizeof(buffer));
                            send(new_socket, buffer, strlen(buffer),0);

                            size = recv(new_socket, buffer, BUF-1, 0);
                            buffer[size - 1] = '\0';
                            temp = buffer;
                            read_postnumber = temp.length() > 0 ? stoi(temp) : 0;

							file_content = "";

							while(getline(MessageIn, temp))
							{
								file_content += temp + "\n";
							}

							MessageIn.close();

							file_substr = "";
                            file_substr = file_content.substr(file_content.find("#***#") + 5, file_content.length());
                            file_substr = file_substr.substr(0, file_substr.find("#####"));
                            number_of_messages = file_substr.length() > 0 ? stoi(file_substr) : 0;

                            if(read_postnumber <= number_of_messages)
                            {
                                temp = "OK";
                                strncpy(buffer,temp.c_str(), sizeof(buffer));
                                send(new_socket, buffer, strlen(buffer),0);

                                file_substr = file_content.substr(file_content.find("#####") + 5, file_content.length());

                                for(int i = 0; i < number_of_messages; ++i)
                                {
                                    read_sender = (i + 1) == read_postnumber ? file_substr.substr(0, file_substr.find("#####")) : read_sender;
                                    file_substr = file_substr.substr(file_substr.find("#####") + 5, file_substr.length());

                                    read_object = (i + 1) == read_postnumber ? file_substr.substr(0, file_substr.find("#####")) : read_object;
                                    file_substr = file_substr.substr(file_substr.find("#####") + 5, file_substr.length());

                                    if((i + 2) != number_of_messages)
                                    {
                                        read_message = (i + 1) == read_postnumber ? file_substr.substr(0, file_substr.find("##*##")) : read_message;
                                    }
                                    else
                                    {
                                        read_message = (i + 1) == read_postnumber ? file_substr.substr(0, file_substr.find("#***#")) : read_message;
                                    }

                                    file_substr = file_substr.substr(file_substr.find("#####") + 5, file_substr.length());
                                }

                                read_result = "Sender: " + read_sender + "\nObject: " + read_object + "\nMessage:" + read_message;
                                cout << read_result << endl;

                                recv(new_socket, buffer, BUF-1, 0);

                                while(read_result.length() > 1)
                                {
                                    read_subend = read_result.length() > BUF ? BUF : read_result.length();
                                    temp = read_result.substr(0, read_subend);
                                    read_result = read_result.substr(read_subend, read_result.length());

                                    strncpy(buffer,temp.c_str(), sizeof(buffer));
                                    send(new_socket, buffer, strlen(buffer),0);
                                    size = recv(new_socket, buffer, BUF-1, 0);
                                }

                                cout << "OPERATION FINISHED\nWaiting for new Input...\n" << endl;
                                strncpy(buffer, ".\n", sizeof(buffer));
                                send(new_socket, buffer, strlen(buffer),0);
                                size = recv(new_socket, buffer, BUF-1, 0);
                            }
                            else
                            {
                                // SEND ERROR TO CLIENT
                                strncpy(buffer, "ERR", sizeof(buffer));
                                send(new_socket, buffer, strlen(buffer),0);
                            }
                        }
                        else
                        {
                            // SEND ERROR TO CLIENT
                            strncpy(buffer, "ERR", sizeof(buffer));
							send(new_socket, buffer, strlen(buffer),0);
                        }
						break;}
					case 4:{
						/*--------------------------*/
						/*     DELETE OPERATION     */
						/*--------------------------*/

						strncpy(buffer,"4", sizeof(buffer));
						send(new_socket, buffer, strlen(buffer),0);

                        size = recv(new_socket, buffer, BUF-1, 0);
                        buffer[size - 1] = '\0';
                        delete_user = buffer;

                        MessageIn.open(m_path + delete_user + ".txt");

                        if(MessageIn.is_open())
						{
                            strncpy(buffer, "OK", sizeof(buffer));
							send(new_socket, buffer, strlen(buffer),0);

                            size = recv(new_socket, buffer, BUF-1, 0);
                            buffer[size - 1] = '\0';
                            temp = buffer;
                            delete_postnumber = temp.length() > 0 ? stoi(temp) : 0;

							file_content = "";

							while(getline(MessageIn, temp))
							{
								file_content += temp + "\n";
							}

							MessageIn.close();

							file_substr = "";
                            file_substr = file_content.substr(file_content.find("#***#") + 5, file_content.length());
                            file_substr = file_substr.substr(0, file_substr.find("#####"));
                            number_of_messages = file_substr.length() > 0 ? stoi(file_substr) : 0;

                            if((number_of_messages == 1) && (delete_postnumber == 1))
							{
                                strncpy(buffer, "OK", sizeof(buffer));
                                send(new_socket, buffer, strlen(buffer),0);

								temp = m_path + delete_user + ".txt";
								remove(temp.c_str());
								cout << "OPERATION FINISHED\nWaiting for new Input...\n" << endl;
							}
							else if(delete_postnumber <= number_of_messages)
                            {
                                strncpy(buffer, "OK", sizeof(buffer));
                                send(new_socket, buffer, strlen(buffer),0);

                                file_substr = file_content.substr(file_content.find("#####"), file_content.length());

                                for(int i = 0; i < number_of_messages; ++i) // 5
                                {
                                    temp = (i + 2) == number_of_messages ? "#***#" : "##*##";

									if((i + 1) != delete_postnumber) // 4
									{
                                        // if i is the second to last of all messages, make it the new last
										delete_output += (i + 2) >= number_of_messages ? "#***#" : "##*##";
										delete_output += to_string(delete_index + 1);
										++delete_index;
										delete_output += file_substr.substr(0, file_substr.find(temp));
									}
									file_substr = file_substr.substr(file_substr.find(temp) + 5, file_substr.length());
									file_substr = file_substr.substr(file_substr.find("#####"), file_substr.length());
                                }

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
                                // SEND ERROR TO CLIENT
                                strncpy(buffer, "ERR", sizeof(buffer));
                                send(new_socket, buffer, strlen(buffer),0);
                            }
                        }
                        else
                        {
                            // SEND ERROR TO CLIENT
                            strncpy(buffer, "ERR", sizeof(buffer));
							send(new_socket, buffer, strlen(buffer),0);
                        }
						break;}
					case 5:{
						cout << "Client disconnected from the server.\n" << endl;

						break;}
					default:{
						/*--------------------------*/
						/*       NO OPERATION       */
						/*--------------------------*/

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
			//cout << "At the end of the while loop" << endl;
		} while (strncmp (buffer, "quit", 4) != 0);

		close (new_socket);
	}

	close (create_socket);
	return EXIT_SUCCESS;
}

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

