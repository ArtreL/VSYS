Hello fellow student!

You have been chosen to review our program, and we want to make it as easy as possible for you to do so.
That's why we have commented a lot of our code. We also named our variables in a way that should make their job obvious.

To quickly explain the peculiarities of our program:
Every receiver has one .txt-file with all their received messages. When you take a look at one of those files, it might look like this:

##*##1#####if16b057#####Herbert#####
This is a test message.
##*##2#####if16b057#####Bertl#####
This is also a test.
#***#3#####if16b057#####Schneckerl#####
Guess what?!
Another test!

To quickly explain our structure:
Each message consists of a subsequent number, the sender's name and the object. 
Since each user gets their own file, we don't need to save the receiver's name.
The message is displayed after each object. Since messages can have multiple lines, we didn't want to display them in one line.
Also: readability!

Before each number you'll find a delimiter, namely "##*##" or "#***#". The rest (sender, object and message) is split with "#####".
We used those delimiters to find the given messages within each text file. "##*##" indicates, that a new message has started,
and "#***#" tells us that the following message will be the last one. This is used to find out how many messages a certain user has.

Note that if you delete the last remaining message from a specific user, their entire file gets deleted rather than leaving an empty file floating around.

You'll notice that both client and server print out a lot more information than needed. While the exercise didn't include those printouts,
we felt like it's much easier to understand what's happening with this additional information, both for debugging and for actual usage.

You can make the files with the attached MAKE-file. When you want to start the programs, the syntax is as requested in the excercise:

Server: ./myserver <PORT> <DIR>
The directory needs to have a slash at the end, like so:
./myserver 6543 data/
If the given directory does not exist, it will be created. The relative path starts at the directory your .exe is in.

Client: ./myclient <IP> <PORT>
./myclient 127.0.0.1 6543

This should also work with the client and the server being executed on two different computers, using the external IP address of the computer that is running the server.
Note that personal firewall/security settings might prevent this from working (we tested it on several computers and it did work on some of them, but not on all).

Thanks for rating our program and for giving feedback, we appreciate it!

All the best,
Lukas and Stephan