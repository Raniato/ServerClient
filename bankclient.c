#include <stdio.h>
#include <string.h>    //strlen
#include <stdlib.h>    //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <pthread.h> //for threading , link with lpthread
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>

//-h server host -p server port -i command file

int resolve_hostname(char *hostname , char *ip)
{
	struct addrinfo hints, *res;
 	struct in_addr addr;
 	int err;
 	memset(&hints, 0, sizeof(hints));
 	hints.ai_socktype = SOCK_STREAM;
 	hints.ai_family = AF_INET;

 	if ((err = getaddrinfo(hostname, NULL, &hints, &res)) != 0) {
 		printf("error %d\n", err);
 		return 1;
 	}
 	addr.s_addr = ((struct sockaddr_in *)(res->ai_addr))->sin_addr.s_addr;
	strcpy(ip,inet_ntoa(addr));
 	freeaddrinfo(res);
}

int main(int argc, char* argv[])
{
	if(argc<7){
		printf("You did not enter all needed flags. Aborting...\n");
		exit(1);
	}
	int i,port;
	char *serverHost, *file;
	for(i=0;i!=argc;i++){
		if(!strcmp(argv[i],"-h")){
			int length = strlen(argv[i+1]);
			serverHost = malloc(length*sizeof(char));
			strcpy(serverHost,argv[i+1]);
		}
		if(!strcmp(argv[i],"-p")) port=atoi(argv[i+1]);
		if(!strcmp(argv[i],"-i")){
			int length = strlen(argv[i+1]);
			file = malloc(length*sizeof(char));
			strcpy(file,argv[i+1]);
		}
	}
	char *host = serverHost;
	char ip[30];
	resolve_hostname(host, ip);
	printf("Connecting to ip %s\n",ip);
    	int sock;
    	struct sockaddr_in server;
    	char message[1000] , server_reply[2000];
	FILE *fp;
	fp=fopen(file, "r");
	if(fp==NULL) printf("File you entered does not exist in the current context\n");
    //Create socket
    	sock = socket(AF_INET , SOCK_STREAM , 0);
   	if (sock == -1)
    	{
        	printf("Could not create socket");
    	}
    	printf("Socket created\n");
     
    	server.sin_addr.s_addr = inet_addr(ip); //127.0.0.1
    	server.sin_family = AF_INET;
    	server.sin_port = htons( port );
 
    //Connect to remote server
    	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    	{
        	perror("connect failed. Error");
        	return 1;
    	}
     
    	puts("Connected\n");
	char buff[255],oneword[255];
	char c;
	while(fgets(buff, 255, (FILE*)fp)!=NULL){
		char *token;
		strcpy(oneword,buff);
		token = strtok(oneword," ");
		buff[strcspn(buff, "\n")] = 0;
		if(!strcmp(buff,"exit")){
			printf("Exiting..\n");
			return 0;
		}
		if(!strcmp(token,"sleep")){
			int sleep;
			sleep=0;
			while(token!=NULL){
				if(sleep==1){
					usleep(1000*atoi(token));
					printf("Slept for %d milliseconds\n",atoi(token));
				}
				token = strtok(NULL," ");
				sleep++;
			}
		}
		if( send(sock , buff , strlen(buff) , 0) < 0)
        	{
            		puts("Send failed");
            		return 1;
        	}
		if( recv(sock , server_reply , 2000 , 0) < 0)
        	{
            		puts("recv failed");
            		break;
        	}
        	puts("Server reply :");
        	puts(server_reply);
		memset(server_reply,0,2000);
		memset(oneword,0,255);
		
	}
    //keep communicating with server
	printf("Switching to manual input (stdin) mode.\n");
    	while(fgets(buff, 255, stdin)!=NULL){
		char *token;
		strcpy(oneword,buff);
		token = strtok(oneword," ");
		buff[strcspn(buff, "\n")] = 0;
		if(!strcmp(buff,"exit")){
			printf("Exiting..\n");
			return 0;
		}
		if(!strcmp(token,"sleep")){
			int sleep;
			sleep=0;
			while(token!=NULL){
				if(sleep==1){
					usleep(1000*atoi(token));
					printf("Slept for %d milliseconds\n",atoi(token));
				}
				token = strtok(NULL," ");
				sleep++;
			}
		}
		if( send(sock , buff , strlen(buff) , 0) < 0)
        	{
            		puts("Send failed");
            		return 1;
        	}
		if( recv(sock , server_reply , 2000 , 0) < 0)
        	{
            		puts("recv failed");
            		break;
        	}
        	puts("Server reply :");
        	puts(server_reply);
		memset(server_reply,0,2000);
		memset(oneword,0,255);
		
	}
     
    	close(sock);
    	return 0;
}
