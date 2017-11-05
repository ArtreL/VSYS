#include <iostream>
#include <ctime>
#include <dirent.h> // Make Directory
#include <chrono>
#define BUF 1024

#define LDAP_HOST "ldap.technikum-wien.at"
#define LDAP_PORT 389
#define SEARCHBASE "dc=technikum-wien,dc=at"
#define SCOPE LDAP_SCOPE_SUBTREE
#define FILTER "(uid=if16b*)"
#define BIND_USER NULL      /* anonymous bind with user and pw NULL */
#define BIND_PW NULL

using namespace std;

class Client
{
    public:
        Client(int new_socket, string m_path, struct sockaddr_in address, int* flagptr)
        {
            this->socket = new_socket;
            this->path = m_path;
            this->client_address = address;
            this->completed = flagptr;
        }

        int GetSocket()
        {
            return socket;
        }

        string GetPath()
        {
            return path;
        }

        struct sockaddr_in GetAddress()
        {
            return client_address;
        }

        int* GetCompleted()
        {
            return completed;
        }

        void ClientSend()
        {
            int socket = this->socket;
            string send_information[4];
            bool check = false;
            string file_output = "";
            int message_number = 0;
            string user_regex = "^[a-zA-Z0-9]{1,8}$";
            string att_path = "";
            int att_length = 0;
            int att_loop = 0;
            string att_export = "";

            string file_content = "";
            string file_substr = "";
            ifstream MessageIn;
            ofstream MessageOut;
            // Regex for the username input
            string temp;
            char buffer[BUF];
            int size;

            // Confirm SEND case to client
            strncpy(buffer,"1", sizeof(buffer));
            send(socket, buffer, strlen(buffer),0);

            // Get receiver name
            // Repeat until input is valid (regex)
            do
            {
                size = recv(socket, buffer, BUF-1, 0);
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
                send(socket, buffer, strlen(buffer),0);
            } while(check && (size != 0));

            do
            {
                size = recv(socket, buffer, BUF-1, 0);
                buffer[size - 1] = '\0';
                temp = buffer;

                if(temp == "y" || temp == "Y")
                {
                    temp = path + send_information[1] + "/";
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

                    size = recv(socket, buffer, BUF-1, 0);
                    buffer[size - 1] = '\0';

                    att_path = buffer; // Filename, parse out of path to file

                    att_export = ", Attachment: " + att_path;

                    strncpy(buffer,"1", sizeof(buffer));
                    send(socket, buffer, strlen(buffer),0);

                    size = recv(socket, buffer, BUF-1, 0);
                    buffer[size] = '\0';                    
                    temp = buffer;
                    strncpy(buffer,"1", sizeof(buffer));
                    send(socket, buffer, strlen(buffer),0);

                    att_length = stoi(temp);
                    att_loop = (att_length / BUF) + 1;

                    char* att_out = new char[att_length];
                    int att_index = 0;

                    for(int i = 0; i < att_loop; ++i)
                    {
                        size = recv(socket, buffer, BUF, 0);
                        cout << "Packagesize: " << size << ", att_index: " << att_index << endl;

                        copy(buffer, buffer + size, att_out + att_index);
                        att_index = (att_index + size) < att_length ? (att_index + size) : (att_length - att_index);

                        strncpy(buffer,"1", sizeof(buffer));
                        send(socket, buffer, strlen(buffer), 0);
                    }

                    MessageOut.open(path + send_information[1] + "/" + att_path);

                    MessageOut.write(att_out, att_length);

                    MessageOut.close();

                    delete att_out;

                    cout << "Attachment received" << endl;

                    check = false;
                }
                else if (temp == "n" || temp == "N")
                {
                    check = false;
                }
                else
                {
                    check = true;
                }

            }while(check);

            // Get subject
            // Repeat until input is valid (char limit)
            do
            {
                size = recv(socket, buffer, BUF-1, 0);
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
                send(socket, buffer, strlen(buffer),0);
            } while((size > 81) && (size != 0));

            // Get message
            // Repeat until the whole message was received
            temp = "";
            do
            {
                size = recv(socket, buffer, BUF-1, 0);
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
            MessageIn.open(this->path + send_information[1] + ".txt");

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
            file_output = "#***#" + to_string(message_number) + "#####" + this->username + "#####" + send_information[2] + att_export + "#####\n" + send_information[3];

            // Append string to existing content
            file_content += file_output;

            // Open file and write
            MessageOut.open(this->path + send_information[1] + ".txt");

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

            send(socket, buffer, strlen(buffer),0);

            cout << "OPERATION FINISHED\nWaiting for new Input...\n" << endl;
        }

        void ClientList()
        {
            int socket = this->socket;
            string path = this->path;
            // Variables for LIST case
            int number_of_messages = 0;
            string list_sender = "";
            string list_subject = "";
            char dump_array[1] = {'a'};
            string file_content = "";
            string file_substr = "";
            string file_output = "";
            ifstream MessageIn;
            ofstream MessageOut;
            string temp;
            char buffer[BUF];

             // Confirm LIST case to client
            strncpy(buffer,"2", sizeof(buffer));
            send(socket, buffer, strlen(buffer),0);

            // Confirm LIST case to client
            recv(socket, buffer, BUF-1, 0);

            // Open file for given user
            MessageIn.open(path + this->username + ".txt");

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
                send(socket, buffer, strlen(buffer),0);

                // Receive confirmation from client
                recv(socket, dump_array, BUF-1, 0);

                file_substr = file_content.substr(file_content.find("#####") + 5, file_content.length());

                // Prepare content and send to client
                for(int i = 0; i < number_of_messages; ++i)
                {
                    // Prepare and send the sender information
                    list_sender = file_substr.substr(0, file_substr.find("#####"));
                    file_substr = file_substr.substr(file_substr.find("#####") + 5, file_substr.length());

                    strncpy(buffer, list_sender.c_str(), sizeof(buffer));
                    send(socket, buffer, strlen(buffer),0);
                    recv(socket, dump_array, BUF-1, 0);

                    // Prepare and send the subject information
                    list_subject = file_substr.substr(0, file_substr.find("#####"));
                    file_substr = file_substr.substr(file_substr.find("#####") + 5, file_substr.length());

                    strncpy(buffer, list_subject.c_str(), sizeof(buffer));
                    send(socket, buffer, strlen(buffer),0);
                    recv(socket, dump_array, BUF-1, 0);

                    // Cut off unused parts of the content
                    file_substr = file_substr.substr(file_substr.find("#####") + 5, file_substr.length());
                }

                cout << "OPERATION FINISHED\nWaiting for new Input...\n" << endl;
            }
            else
            {
                // Could not open file; Send err to client
                strncpy(buffer, "ERR", sizeof(buffer));
                send(socket, buffer, strlen(buffer),0);
            }
        }

        void ClientRead()
        {
            int socket = this->socket;
            string path = this->path;
            // Variables for READ case
            int read_postnumber = 0;
            int number_of_messages = 0;
            string read_sender = "";
            string read_subject = "";
            string read_message = "";
            string read_result = "";
            string file_content = "";
            string file_substr = "";
            string file_output = "";
            int read_subend = 0;
            ifstream MessageIn;
            ofstream MessageOut;
            string temp;
            char buffer[BUF];
            char receiver[BUF];
            int size;

            // Confirm READ case to client
            strncpy(buffer,"3", sizeof(buffer));
            send(socket, buffer, strlen(buffer),0);

            // Receive confirmation from client
            size = recv(socket, buffer, BUF-1, 0);

            // Open file for this user
            MessageIn.open(path + this->username + ".txt");

            if(MessageIn.is_open())
            {
                // Send success message to client
                temp = "OK";
                strncpy(buffer,temp.c_str(), sizeof(buffer));
                send(socket, buffer, strlen(buffer),0);

                // Receive post number from client
                size = recv(socket, buffer, BUF-1, 0);
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
                    send(socket, buffer, strlen(buffer),0);

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
                    read_result = "Sender: " + read_sender + "\nSubject: " + read_subject + "\nMessage:" + read_message;

                    // Receive OK from client to start sending
                    recv(socket, buffer, BUF-1, 0);

                    string att_name = read_result.find("Attachment: ") != string::npos ? read_result.substr(read_result.find("Attachment: ") + 12, read_result.find("\nMessage:") - (read_result.find("Attachment: ") + 12)) : "";

                    // Send master string in 1024 Bit blocks until the master string is empty
                    while(read_result.length() > 1)
                    {
                        // Check if remaining string is longer than 1024 characters and use 1024 if it is
                        read_subend = read_result.length() > BUF ? (BUF - 1) : read_result.length();

                        // Write master string to temp string until char limit is reached
                        temp = read_result.substr(0, read_subend);

                        // Cut off content that has already been prepared to be sent
                        read_result = read_result.substr(read_subend, read_result.length());

                        // Send prepared string
                        strncpy(buffer,temp.c_str(), sizeof(buffer));
                        send(socket, buffer, strlen(buffer),0);

                        // Wait for OK from client
                        size = recv(socket, buffer, BUF-1, 0);
                    }

                    // Send FINISH to client so they can stop receiving data
                    strncpy(buffer, ".\n", sizeof(buffer));
                    send(socket, buffer, strlen(buffer),0);

                    if(att_name != "")
                    {
                        MessageIn.open(path + username + "/" + att_name, ios_base::binary);

                        if(MessageIn.is_open())
                        {

                            MessageIn.seekg (0, MessageIn.end);
                            int length = MessageIn.tellg();
                            MessageIn.seekg (0, MessageIn.beg);

                            char* att_content = new char[length];

                            MessageIn.read(att_content, length);

                            MessageIn.close();

                            int att_length = (length / BUF) + 1;

                            strncpy(buffer, to_string(length).c_str(), sizeof(buffer));
                            send(socket, buffer, strlen(buffer), 0);

                            recv(socket, receiver, BUF-1, 0);

                            int att_index = 0;
                            int att_end = 0;

                            // Send master string in 1024 Bit blocks until the master string is empty
                            for(int i = 0; i < att_length; ++i)
                            {
                                att_end = (att_index + BUF) < length ? BUF : (length - att_index);
                                copy(att_content + att_index, att_content + att_index + att_end, buffer + 0);
                                att_index = att_index + BUF;

                                send(socket, buffer, att_end, 0);

                                recv(socket, receiver, BUF-1, 0);
                            }

                            delete att_content;
                        }
                        else
                        {
                            // Post number was not contained in current file
                            strncpy(buffer, "ERR", sizeof(buffer));
                            send(socket, buffer, strlen(buffer),0);
                        }
                    }

                    cout << "OPERATION FINISHED\nWaiting for new Input...\n" << endl;

                    // Wait for OK from client
                    size = recv(socket, buffer, BUF-1, 0);
                }
                else
                {
                    // Post number was not contained in current file
                    strncpy(buffer, "ERR", sizeof(buffer));
                    send(socket, buffer, strlen(buffer),0);
                }
            }
            else
            {
                // Could not open file for user
                strncpy(buffer, "ERR", sizeof(buffer));
                send(socket, buffer, strlen(buffer),0);
            }
        }

        void ClientDel()
        {
            int socket = this->socket;
            string path = this->path;
            // Variables for DEL case
            int delete_postnumber = 0;
            int number_of_messages = 0;
            string delete_output = "";
            int delete_index = 0;
            string file_content = "";
            string file_substr = "";
            string file_output = "";
            ifstream MessageIn;
            ofstream MessageOut;
            string temp;
            char buffer[BUF];
            int size;

            // Confirm DEL case to client
            strncpy(buffer,"4", sizeof(buffer));
            send(socket, buffer, strlen(buffer),0);

            // Receive username from client
            size = recv(socket, buffer, BUF-1, 0);

            // Open file for this user
            MessageIn.open(path + this->username + ".txt");

            if(MessageIn.is_open())
            {
                // Send success message to client
                strncpy(buffer, "OK", sizeof(buffer));
                send(socket, buffer, strlen(buffer),0);

                // Receive post number from client
                size = recv(socket, buffer, BUF-1, 0);
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
                    send(socket, buffer, strlen(buffer),0);

                    // Since there is only one message in the file
                    // and it's supposed to be deleted,
                    // simply delete the file
                    temp = path + this->username + ".txt";
                    remove(temp.c_str());
                    cout << "OPERATION FINISHED\nWaiting for new Input...\n" << endl;
                }
                // Check if post number is present in current content
                else if((0 < delete_postnumber) && (delete_postnumber <= number_of_messages))
                {
                    // Send success message to client
                    strncpy(buffer, "OK", sizeof(buffer));
                    send(socket, buffer, strlen(buffer),0);

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
                    MessageOut.open(path + this->username + ".txt");

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

                    cout << "OPERATION FINISHED\nWaiting for new Input..." << endl;
                }
                else
                {
                    // Post number was not contained in current file
                    strncpy(buffer, "ERR", sizeof(buffer));
                    send(socket, buffer, strlen(buffer),0);
                }
            }
            else
            {
                // Could not open file for this user
                strncpy(buffer, "ERR", sizeof(buffer));
                send(socket, buffer, strlen(buffer),0);
            }
        }

        int ClientLogin()
        {
            int socket = this->socket;
            string username;
            string password;
            int size;
            char buffer[BUF];
            string bla = inet_ntoa(this->client_address.sin_addr);
            string blub = to_string(this->client_address.sin_port);
            string ip_port = bla + ":" + blub;

            // Confirm LOGIN case to client
            strncpy(buffer,"5", sizeof(buffer));
            send(socket, buffer, strlen(buffer),0);
            
            recv(socket, buffer, BUF-1, 0);

            CleanLockLog();

            if(IpIsLocked(ip_port) == 0)
            {
                strncpy(buffer,"OK", sizeof(buffer));
                send(socket, buffer, strlen(buffer),0);
                // Receive username from client
                size = recv(socket, buffer, BUF-1, 0);
                buffer[size - 1] = '\0';
                username = buffer;

                // Receive password from client
                size = recv(socket, buffer, BUF-1, 0);
                buffer[size - 1] = '\0';
                password = buffer;

                string check_login = LDAPMagic(username, password);

                if(check_login != "")
                {
                    UnlockIp(ip_port);
                    // Allow LOGIN for client
                    string temp = "Welcome " + check_login + "!";
                    strncpy(buffer, temp.c_str(), sizeof(buffer));
                    send(socket, buffer, strlen(buffer),0);

                    this->username = username;

                    return 1;
                }
                else
                {
                    // Deny LOGIN for client
                    strncpy(buffer,"Access denied.", sizeof(buffer));
                    send(socket, buffer, strlen(buffer),0);

                    LockIp(ip_port);

                    return 0;
                }
            }
            else
            {
                // Deny LOGIN for client
                strncpy(buffer,"Your IP has been flagged for suspicious behaviour.\nTry again in 30 minutes.", sizeof(buffer));
                send(socket, buffer, strlen(buffer),0);
                return 0;
            }
        }
    private:
        int socket;
        string path;
        struct sockaddr_in client_address;
        int* completed;
        string username;

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

        string LDAPMagic(string username, string password)
        {
            LDAP *ld;                    /* LDAP resource handle */
            LDAPMessage *result, *e;     /* LDAP result handle */
            BerElement *ber;             /* array of attributes */
            char *attribute;
            char **vals;
            string dn_string, cn_string;
            string uid_string = "";
            int foundit = 0;

            int i, rc = 0;

            char *attribs[3];            /* attribute array for search */

            attribs[0] = strdup("uid");  /* return uid and cn of entries */
            attribs[1] = strdup("cn");
            attribs[2] = NULL;           /* array must be NULL terminated */


            /* setup LDAP connection */
            if ((ld = ldap_init(LDAP_HOST, LDAP_PORT)) == NULL)
            {
              perror("ldap_init failed");
              return 0;
            }

            //printf("connected to LDAP server %s on port %d\n",LDAP_HOST,LDAP_PORT);

            /* anonymous bind */
            rc = ldap_simple_bind_s(ld, BIND_USER, BIND_PW);

            do
            {
                if(foundit == 1)
                {
                    rc = ldap_simple_bind_s(ld, dn_string.c_str(), password.c_str());

                    if(rc == LDAP_SUCCESS)
                    {
                        ldap_unbind(ld);
                        return cn_string;
                    }
                    else
                    {
                        return "";
                    }
                }

                if (rc != LDAP_SUCCESS)
                {
                    fprintf(stderr, "LDAP error: %s\n", ldap_err2string(rc));
                    return "";
                }
                else
                {
                    printf("bind successful\n");
                }

                /* perform ldap search */
                rc = ldap_search_s(ld, SEARCHBASE, SCOPE, FILTER, attribs, 0, &result);

                if (rc != LDAP_SUCCESS)
                {
                    fprintf(stderr,"LDAP search error: %s\n",ldap_err2string(rc));
                    return "";
                }

                //printf("Total results: %d\n", ldap_count_entries(ld, result));

                for (e = ldap_first_entry(ld, result); e != NULL; e = ldap_next_entry(ld, e))
                {
                    dn_string = ldap_get_dn(ld, e);

                    /* Now print the attributes and values of each found entry */

                    for (attribute = ldap_first_attribute(ld, e, &ber); attribute != NULL; attribute = ldap_next_attribute(ld, e, ber))
                    {
                        if ((vals = ldap_get_values(ld, e, attribute)) != NULL)
                        {
                            for (i = 0; vals[i] != NULL; i++)
                            {
                                string temp = attribute;

                                if(temp == "cn")
                                {
                                    cn_string = vals[i];
                                    break;
                                }
                                else if(temp == "uid" && foundit == 0)
                                {
                                    uid_string = vals[i];
                                    break;
                                }
                            }
                            /* free memory used to store the values of the attribute */
                            ldap_value_free(vals);
                        }
                        /* free memory used to store the attribute */
                        ldap_memfree(attribute);

                        if(uid_string == username)
                        {
                            cout << "DN: " << dn_string << endl;
                            foundit = 1;
                            break;
                        }
                    }
                    /* free memory used to store the value structure */
                    if (ber != NULL) ber_free(ber, 0);

                    if(foundit == 1)
                    {
                        break;
                    }
                }

                /* free memory used for result */
                ldap_msgfree(result);
                free(attribs[0]);
                free(attribs[1]);
            } while(foundit != 0);

            ldap_unbind(ld);

            return "";
        }

        int IpIsLocked(string ip)
        {
            ifstream MessageIn;
            string line;
            string blocked_ip;
            int block_num;

            MessageIn.open("iplock.log");

            while(getline(MessageIn, line))
            {
                line = line.substr(line.find(' ') + 1);
                blocked_ip = line.substr(0, line.find(' '));
                block_num = stoi(line.substr(line.find(' ') + 1));

                if(blocked_ip == ip && block_num == 3)
                {
                    return 1;
                }
            }

            MessageIn.close();

            return 0;
        }

        void CleanLockLog()
        {
            string file_content = "";
            string file_substr = "";
            ifstream MessageIn;
            ofstream MessageOut;
            string line;
            int timestamp;
            string blocked_ip;
            string block_num;

            MessageIn.open("iplock.log");

            while(getline(MessageIn, line))
            {
                timestamp = stoi(line.substr(0, line.find(' ')));
                line = line.substr(line.find(' ') + 1);
                blocked_ip = line.substr(0, line.find(' '));
                block_num = line.substr(line.find(' ') + 1);

                // cout << timestamp << " : " << time(0) << " : " << timestamp - time(0) << endl;
                if((time(0) - timestamp) < 1800)
                {
                    file_content += to_string(timestamp) + " " + blocked_ip + " " + block_num + "\n";
                }
            }

            MessageIn.close();

            MessageOut.open("iplock.log");

            MessageOut << file_content;

            MessageOut.close();
        }

        void LockIp(string ip)
        {
            string file_content = "";
            string file_substr = "";
            ifstream MessageIn;
            ofstream MessageOut;
            string line;
            int timestamp;
            string blocked_ip;
            string block_num;
            int new_ip = 1;

            MessageIn.open("iplock.log");

            while(getline(MessageIn, line))
            {
                timestamp = stoi(line.substr(0, line.find(' ')));
                line = line.substr(line.find(' ') + 1);
                blocked_ip = line.substr(0, line.find(' '));
                block_num = line.substr(line.find(' ') + 1);

                if(blocked_ip == ip)
                {
                    block_num = to_string(stoi(block_num) + 1);
                    file_content += to_string(timestamp) + " " + blocked_ip + " " + block_num + "\n";
                    new_ip = 0;
                }
                else
                {
                    file_content += to_string(timestamp) + " " + blocked_ip + " " + block_num + "\n";
                }
            }

            if(new_ip == 1)
            {
                file_content += to_string(time(0)) + " " + ip + " 1\n";
            }

            MessageIn.close();

            MessageOut.open("iplock.log");

            MessageOut << file_content;

            MessageOut.close();
        }

        void UnlockIp(string ip)
        {
            string file_content = "";
            string file_substr = "";
            ifstream MessageIn;
            ofstream MessageOut;
            string line;
            int timestamp;
            string blocked_ip;
            string block_num;

            MessageIn.open("iplock.log");

            while(getline(MessageIn, line))
            {
                timestamp = stoi(line.substr(0, line.find(' ')));
                line = line.substr(line.find(' ') + 1);
                blocked_ip = line.substr(0, line.find(' '));
                block_num = line.substr(line.find(' ') + 1);

                if(blocked_ip != ip)
                {
                    file_content += to_string(timestamp) + " " + blocked_ip + " " + block_num + "\n";
                }
            }

            MessageIn.close();

            MessageOut.open("iplock.log");

            MessageOut << file_content;

            MessageOut.close();
        }
};