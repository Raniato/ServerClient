# ServerClient
Server-client program using multithreading techniques (developed in linux)

This is a project I developed during my bachelor studies for the subject "System Programming".

For my Greek friends, reading the README file will pretty much explain what this program does. For my non-Greek friends, here is a small brief:
Server is actually a bank server simulation, uses a hash table (constant size) which has lists in every bucket. The hash function is nothing special, just a casual division between ascii codes and hashtable size.
Server (bankserver.c) initializes some threads and waits for incoming connections from a client (bankclient.c). When an incoming request arrives, the server binds a file descriptor to a thread.
Hashtable has 1 mutex per 5 positions so as to maintain synchronization and avoid deadlocks.

You run the bankserver.c file as: $ ./bankserver -p <port> -s <thread pool size> -q <queue size>
queue size is the maximum size of incoming requests permitted.
and the bankclient.c file as:     $ ./bankclient -h <server host> -p <server port> -i <command file>
command file is a text file that should end with an "exit" otherwise user gets prompted to stdin input.
Compile everything through makefile.

I hope this project is useful for anyone who is struggling with basic threading techniques and/or server-client issues. :)

For any other questions anyone may have, contact me at rania.spantidi[at]gmail.com
