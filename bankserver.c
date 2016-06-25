#include <string.h>    //strlen
#include <stdlib.h>    //strlen
#include <sys/socket.h>
#include <stdio.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <errno.h>
#include "serveraux.h"
#include "threads.h"


#define MAXMSG 1000

int main(int argc, char* argv[])
{
	if(argc<7){
		printf("You did not enter all needed flags. Aborting...\n");
		exit(1);
	}
	int i,port,threadPool,queueSize;
	for(i=0;i!=argc;i++){
		if(!strcmp(argv[i],"-p")) port=atoi(argv[i+1]);
		if(!strcmp(argv[i],"-s")) threadPool=atoi(argv[i+1]);
		if(!strcmp(argv[i],"-q")) queueSize=atoi(argv[i+1]);
	}
	int socket_desc , c , client_sock;
	fd_set active_fd_set, read_fd_set;
    	struct sockaddr_in server , client;
	//Initialize job pool
	initialize (&pool,queueSize);
     	//Initialize mutexes
	for(i=0;i!=noMutex;i++)
		pthread_mutex_init(&mutexes[i], 0);
	pthread_mutex_init(&mtx, 0) ;
	pthread_cond_init(&cond_nonempty, 0) ;
	pthread_cond_init(&cond_nonfull, 0) ;
	for(i=0;i!=threadPool;i++){
		pthread_t newthr;
		//pthread_create(&newthr, 0, connection_handler, (void*)queueSize) ;
		pthread_create( &newthr , 0 ,  connection_handler , 0);
	}
    	//Create socket
    	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    	if (socket_desc == -1)
    	{
        	printf("Could not create socket");
    	}
    	puts("Socket created");
     
    	//Prepare the sockaddr_in structure
    	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
    	server.sin_port = htons( port );
    	//Bind
    	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    	{
        //print the error message
        	perror("bind failed. Error");
        	return 1;
    	}
    	puts("Bind done");
     
    	//Listen
    	if(listen(socket_desc , queueSize)==-1){
		perror("listen");
		exit(1);
	}
     	hp = malloc(hashSize*sizeof(*hp));
	for(i=0;i!=hashSize;i++)
		hp[i]=NULL;
	 /* Initialize the set of active sockets. */
  	FD_ZERO(&active_fd_set);
  	FD_SET(socket_desc, &active_fd_set);

    	//Accept and incoming connection
    	puts("Waiting for incoming connections...");
    	c = sizeof(struct sockaddr_in);
	while(1)
	{
		//(client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c))
		/* Block until input arrives on one or more active sockets. */
      		read_fd_set = active_fd_set;
		if (select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0){
          		perror ("select");
          		exit (EXIT_FAILURE);
        	}
		for (i = 0; i < FD_SETSIZE; ++i)
			if (FD_ISSET (i, &read_fd_set)){
				if (i == socket_desc){
					int new;
					new = accept(socket_desc,
				              	(struct sockaddr *) &client,
				              	(socklen_t*)&c);
					if (new < 0){
				    		perror ("accept");
				    		exit (EXIT_FAILURE);
				  	}
					fprintf (stderr,
				         "Server: connect from host %s, file descriptor %d\n",
				         inet_ntoa (client.sin_addr),
				         new);
					place(&pool , new, queueSize);
					pthread_cond_signal(&cond_nonempty);
					FD_SET (new, &active_fd_set);
			      }
			   /* else
			      {
				if (read_from_client (i) < 0)
				  {
				    close (i);
				    FD_CLR (i, &active_fd_set);
				  }
			      }*/
			  }
	}
     
    	if (client_sock < 0)
    	{
       		perror("accept failed");
        	return 1;
    	}
     	
    	return 0;

    
}
