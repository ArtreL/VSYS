/* myserver.cpp */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <regex>
#define BUF 1024
#define PORT 6543

using namespace std;

string StringToLower(char value[]);

int main (void) {
	int create_socket, new_socket;
	socklen_t addrlen;
	char buffer[BUF];
	int size;
	struct sockaddr_in address, cliaddress;

	string user_regex = "^[a-zA-Z0-9]{1,8}$";
	string strbuf;

	create_socket = socket (AF_INET, SOCK_STREAM, 0);

	memset(&address,0,sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons (PORT);
	int x = 1;
	int a = setsockopt(create_socket, SOL_SOCKET, SO_REUSEADDR, &x, sizeof(int*));

	if(a == 123090321)
    {
        cout << "Okay des is weird" << endl;
    }

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
			strbuf = "Welcome to myserver, Please enter your command:\n";
			cout << "Client connected from " << inet_ntoa(cliaddress.sin_addr) << ":" << ntohs(cliaddress.sin_port)<< "..." << endl;
			strncpy(buffer, strbuf.c_str(), sizeof(buffer));
			send(new_socket, strbuf.c_str(), strlen(buffer),0);
		}

		do
		{
			//cout << "Waiting on Input" << endl;
			size = recv(new_socket, buffer, BUF-1, 0);
			//cout << "Got some Input" << endl;

			if( size > 0)
			{
				buffer[size] = '\0';
				string temp = StringToLower(buffer);
				cout << "Message received: " << temp << endl;

				int input_type = temp == "send" ? 1 : temp == "list" ? 2 : temp == "read" ? 3 : temp == "del" ? 4 : temp == "quit" ? 5 : 6;

				string send_information[4];
				bool check = false;
				ifstream MessageIn;
				ofstream MessageOut;
				string file_content;
				string file_substr;
				string file_output;
				int message_number;

				string list_user;
				int number_of_messages;
				string list_sender;
				string list_object;
				char dump_array[1];

				string read_user;
				int read_postnumber;
				string read_sender = "";
				string read_object = "";
				string read_message = "";
				string read_result;
				int read_subend;

				string delete_user;
				int delete_postnumber;
				string delete_output;

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
							send_information[2] = buffer;
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
						send_information[3] = temp;
						cout << "Message:\n" << send_information[3];

						/*     WRITE TO FILE     */
						MessageIn.open("data/" + send_information[1] + ".txt");

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

						MessageOut.open("data/" + send_information[1] + ".txt");

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

                        MessageIn.open("data/" + list_user + ".txt");

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

                        size = recv(new_socket, buffer, BUF-1, 0);
                        buffer[size - 1] = '\0';
                        temp = buffer;
                        read_postnumber = temp.length() > 0 ? stoi(temp) : 0;

                        MessageIn.open("data/" + read_user + ".txt");

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

                            if(read_postnumber <= number_of_messages)
                            {
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
                                strncpy(buffer, "ERR\n", sizeof(buffer));
                                send(new_socket, buffer, strlen(buffer),0);
                            }
                        }
                        else
                        {
                            // SEND ERROR TO CLIENT
                            strncpy(buffer, "ERR\n", sizeof(buffer));
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

                        size = recv(new_socket, buffer, BUF-1, 0);
                        buffer[size - 1] = '\0';
                        temp = buffer;
                        delete_postnumber = temp.length() > 0 ? stoi(temp) : 0;

                        MessageIn.open("data/" + delete_user + ".txt");

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

                            if(delete_postnumber <= number_of_messages)
                            {
                                file_substr = file_content.substr(file_content.find("#####") + 5, file_content.length());

                                for(int i = 0; i < number_of_messages; ++i)
                                {
                                }
                            }
                            else
                            {
                                // SEND ERROR TO CLIENT
                                strncpy(buffer, "ERR\n", sizeof(buffer));
                                send(new_socket, buffer, strlen(buffer),0);
                            }
                        }
                        else
                        {
                            // SEND ERROR TO CLIENT
                            strncpy(buffer, "ERR\n", sizeof(buffer));
							send(new_socket, buffer, strlen(buffer),0);
                        }
						break;}
					case 5:
						cout << "I'm quitting here!" << endl;
						return 0;
						break;
					default:
						cout << "No Matching Operation Found" << endl;
						break;
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
		} while (strncmp (buffer, "quit", 4)  != 0);

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

