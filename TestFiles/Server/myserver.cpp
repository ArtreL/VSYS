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

	string strbuf;
	string send_information[4];
	string user_regex = "if[0-9]{2}b[0-9]{3}$";
	int check = 0;
	ifstream MessageIn;
	ofstream MessageOut;
	string file_content;
	string file_substr;
	string file_output;
	int message_number;

	create_socket = socket (AF_INET, SOCK_STREAM, 0);

	memset(&address,0,sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons (PORT);

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
			cout << "Waiting on Input" << endl;
			size = recv(new_socket, buffer, BUF-1, 0);
			cout << "Got some Input" << endl;

			if( size > 0)
			{
				buffer[size] = '\0';
				string temp = StringToLower(buffer);
				cout << "Message received:" << temp << endl;

				int input_type = temp == "send" ? 1 : temp == "list" ? 2 : temp == "read" ? 3 : temp == "del" ? 4 : temp == "quit" ? 5 : 6;

				switch(input_type)
				{
					case 1:
						strncpy(buffer,"1", sizeof(buffer));
						send(new_socket, buffer, strlen(buffer),0);
						
						do
						{
							size = recv(new_socket, buffer, BUF-1, 0);
							buffer[size - 1] = '\0';
							send_information[0] = buffer;
							// regex check
							check = !(regex_match(send_information[0], regex(user_regex)));
							// send OK/NOK
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
						
						do
						{
							size = recv(new_socket, buffer, BUF-1, 0);
							buffer[size - 1] = '\0';
							send_information[1] = buffer;
							// regex check
							check = !(regex_match(send_information[1], regex(user_regex)));
							// send OK/NOK
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
						
						MessageIn.open("data/" + send_information[1] + ".txt");

						if(MessageIn.is_open())
						{
							file_content = "";

							while(getline(MessageIn, temp))
							{
								file_content += temp + "\n";
							}
        						
							MessageIn.close();
							file_substr = file_content.substr(file_content.find("#***#") + 5, file_content.length());
							file_substr = file_substr.substr(0, file_substr.find("#####"));
							file_content.replace(file_content.find("#***#"), 5, "##*##");
						}
						message_number = file_substr.length() > 0 ? stoi(file_substr) : 0;
						++message_number;

						file_output = "#***#" + to_string(message_number) + "#####" + send_information[0] + "#####" + send_information[2] + "#####\n" + send_information[3];

						file_content += file_output;

						MessageOut.open("data/" + send_information[1] + ".txt");

						MessageOut << file_content;

						MessageOut.close();

						break;
					case 2:
						cout << "I'm listing here!" << endl;
						break;
					case 3:
						cout << "I'm reading here!" << endl;
						break;
					case 4:
						cout << "I'm deleting here!" << endl;
						break;
					case 5:
						cout << "I'm quitting here!" << endl;
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
			cout << "At the end of the while loop" << endl;
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
