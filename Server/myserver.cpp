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
#include <pthread.h>
#include <ldap.h>
#include "clientinfo.h"
#define BUF 1024

using namespace std;

// Some functions used in the program
string StringToLower(char value[]);
void* BasicallyEverything(void *arg);

int main (int argc, char **argv)
{
	int create_socket, new_socket;
	socklen_t addrlen;
	struct sockaddr_in address, cliaddress;
    pthread_t t1[12];
    int thread_flags[12] = {0};
    int t_iterator = 0;
    bool threadwait = true;

    // Regex for the serverstart input
	string dir_regex = "^[a-zA-Z0-9_+.,]{1,200}/$";

	create_socket = socket (AF_INET, SOCK_STREAM, 0);

    // If not enough arguments are delivered, the program exits
	if(argc < 3)
	{
		cout << "Missing Arguments: " << argv[0] << " Port Number + Mailspookdirectory" << endl;
		cout << "Maybe try 'data/' for a directory :)" << endl;
		exit(EXIT_FAILURE);
	}

    // Path to mailspooldirectory
	string m_path = argv[2]; // "data/";
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
    
    printf("Waiting for connections...\n");

	while (1)
	{
		listen (create_socket, 5);
		addrlen = sizeof (struct sockaddr_in);

        new_socket = accept (create_socket, (struct sockaddr *) &cliaddress, &addrlen);

        // multithreading
        do
        {
            if(thread_flags[t_iterator] == 0)
            {
        		Client* thread_arg = new Client(new_socket, m_path, cliaddress, &thread_flags[t_iterator]);
        		pthread_create(&t1[t_iterator], NULL, BasicallyEverything, thread_arg);
                thread_flags[t_iterator] = 2;
                threadwait = false;
            }

            ++t_iterator;
            t_iterator = t_iterator > 12 ? 0 : t_iterator;

            for(int i = 0; i < 12; ++i)
            {
                // variable to see used threads
                // cout << "Flag #" << i << ": " << thread_flags[i] << endl;

                if(thread_flags[i] == 1)
                {
                    pthread_join(t1[i], NULL);
                    thread_flags[i] = 0;
                }
            }
        }while (threadwait);

        threadwait = true;
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

// used to do everything (basically), now most if it is being done in the clientinfo class
void* BasicallyEverything(void *arg)
{
    Client User = *(Client*) arg;
    delete (Client*)arg;
	int new_socket = User.GetSocket();
	string m_path = User.GetPath();
    int* completed = User.GetCompleted();
	// Temporary variable used every now and then
	string temp;
	struct sockaddr_in cliaddress = User.GetAddress();
	char buffer[BUF];
	int size;
    int login = 0;

    if (new_socket > 0)
    {
        // Send a warm welcome to the client
        temp = "Welcome to myserver, please enter your command:\n";
        cout << "Client connected from " << inet_ntoa(cliaddress.sin_addr) << ":" << ntohs(cliaddress.sin_port) << "..." << endl;
        strncpy(buffer, temp.c_str(), sizeof(buffer));
        send(new_socket, temp.c_str(), strlen(buffer),0);
    }

    // Set client in a loop to login
    do
    {
        // Reset buffer
        memset(buffer, 0, sizeof(buffer));
        size = recv(new_socket, buffer, BUF-1, 0);
        buffer[size] = '\0';
        temp = StringToLower(buffer);

        if(temp == "login")
        {
            login = User.ClientLogin();
        }
        else if(temp == "quit")
        {
            cout << "Client: " << inet_ntoa(cliaddress.sin_addr) << ":" << ntohs(cliaddress.sin_port) << " disconnected from the server.\n" << endl;
            close (new_socket);
            *completed = 1;
            return NULL;
        }
        else
        {
            // Confirm NO case to client
            strncpy(buffer,"6", sizeof(buffer));
            send(new_socket, buffer, strlen(buffer),0);
        }
    }while(login != 1);

    // Set client in the main action loop
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

            switch(input_type)
            {
                case 1:{
                    /*--------------------------*/
                    /*      SEND OPERATION      */
                    /*--------------------------*/

                    User.ClientSend();

                    break;}
                case 2:{
                    /*--------------------------*/
                    /*      LIST OPERATION      */
                    /*--------------------------*/

                    User.ClientList();

                    break;}
                case 3:{
                    /*--------------------------*/
                    /*      READ OPERATION      */
                    /*--------------------------*/

                    User.ClientRead();

                    break;}
                case 4:{
                    /*--------------------------*/
                    /*     DELETE OPERATION     */
                    /*--------------------------*/

                    User.ClientDel();

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
        }
    } while (strncmp (buffer, "quit", 4) != 0);

    close (new_socket);
    *completed = 1;
	return NULL;
}