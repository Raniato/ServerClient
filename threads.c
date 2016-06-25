#include <string.h>
#include <stdlib.h> 
#include <sys/socket.h>
#include <stdio.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <errno.h>
#include <ctype.h>
#include "serveraux.h"
#include "threads.h"

int delay(char *test)
{
	int i,flag;
	flag = 0;
	for(i=0;i!=strlen(test);test++){
		if(!isdigit(test[i]))
			flag=1;
	}
	if(flag==1) return -1; //not an integer value
	return atoi(test);
}

void bubblesort(int *list,int no)
{
	int temp;
   	int i,j;
	int swapped = 0;
   	for(i = 0; i < no-1; i++) { 
      		swapped = 0;
	      	for(j = 0; j < no-1-i; j++) {
			if(list[j] > list[j+1]) {
			    	temp = list[j];
			    	list[j] = list[j+1];
			    	list[j+1] = temp;
			    	swapped = 1;
			}
		}
		if(!swapped) break;		
      	}
}

void *connection_handler()
{
	//Get the socket descriptor
	//int sock = *(int*)socket_desc;
	int read_size;
	char client_message[2000];
	    //Receive a message from client
	//pthread_mutex_lock(&mtx);
	int sock = obtain(&pool);
	pthread_cond_signal(&cond_nonfull) ;
	printf ("Hello I'm thread %lu and I will manage the connection with fd: %d \n " , pthread_self(),sock);
	
	while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 ) //read client's messages
	{
		char message[2000],clientmsg[2000];
		char *token;
		strcpy(clientmsg,client_message);
		token = strtok(client_message," ");
		if(!strcmp(token,"add_account")){
			if(token!=NULL){
				int add_acc,amount;
				char *name;
				add_acc=0;
				while(token!=NULL){
					//printf("%s\n",token);
					if(add_acc==1) amount=atoi(token);
					if(add_acc==2){
						name=malloc(strlen(token)*sizeof(char));
						strcpy(name,token);
					}
					if(add_acc==3){
						int delayTime = delay(token);
						if(delayTime!=-1)
							usleep(1000*delayTime);
					}
					token = strtok(NULL, " ");
					add_acc++;
				}
				int mut;
				mut = hash(name)/5;
				pthread_mutex_lock(&mutexes[mut]);
				hashInsert(name,amount);
				sprintf(message, "Success. Account creation (%s:%d)", name,amount);
				pthread_mutex_unlock(&mutexes[mut]);
			}
		}
		else if(!strcmp(token,"add_transfer")){
			if(token!=NULL){
				int add_tran,amount;
				char *name1,*name2;
				add_tran=0;
				while(token!=NULL){
					if(add_tran==1) amount=atoi(token);
					if(add_tran==2){
						name1=malloc(strlen(token)*sizeof(char));
						strcpy(name1,token);
					}
					if(add_tran==3){
						name2=malloc(strlen(token)*sizeof(char));
						strcpy(name2,token);
					}
					if(add_tran==4){
						int delayTime = delay(token);
						if(delayTime!=-1)
							usleep(1000*delayTime);
					}
					token = strtok(NULL, " ");
					add_tran++;
				}
				int muts[2],err;
				muts[0]=hash(name1)/5;
				muts[1]=hash(name2)/5;
				bubblesort(muts,2);
				if ( (err = pthread_mutex_lock(&mutexes[muts[0]])) ) { /* Lock mutex */
					perror2 ( " pthread_mutex_ lock " , err ) ; exit (1) ; }
				
				if(muts[0]!=muts[1])
					if ( (err = pthread_mutex_lock(&mutexes[muts[1]])) ) { /* Lock mutex */
						perror2 ( " pthread_mutex_ lock " , err ) ; exit (1) ; }
				if(addTran(name1,name2,amount)==1)
					sprintf(message,"Success. Transfer addition (%s:%s:%d)",name1,name2,amount);
				else
					sprintf(message,"Error. Transfer addition failed(%s:%s:%d)",name1,name2,amount);
				if ( (err = pthread_mutex_unlock(&mutexes[muts[0]])) ) { /* Lock mutex */
					perror2 ( " pthread_mutex_unlock " , err ) ; exit (1) ; }
				
				if(muts[0]!=muts[1])
					if ( (err = pthread_mutex_unlock(&mutexes[muts[1]])) ) { /* Lock mutex */
						perror2 ( " pthread_mutex_unlock " , err ) ; exit (1) ; }
			}
		
		}
		else if(!strcmp(token,"add_multi_transfer")){
			if(token!=NULL){
				int print,names,j,amount;
				print = 0;
				names = 0;
				while(token!=NULL){
					if(print==1) amount = atoi(token);
					if(print>1){
						int delayTime = delay(token);
						if(delayTime!=-1){
							usleep(1000*delayTime);
							printf("I kinda slept\n");
						}
						else names++;
					}
					token = strtok(NULL, " ");
					print++;
				}
				char **namesprints;
				int *muts;
				muts = malloc(names*sizeof(int));
				namesprints = malloc(names*sizeof(char*));
				if(namesprints==NULL){
					printf("Critical error while malloc-ing\n");
					exit(1);
				}
				for(j=0;j!=names;j++){
					*(namesprints+j) = malloc(20*sizeof(char*));
					if(*(namesprints+j)==NULL){
						printf("Critical error while malloc-ing\n");
						exit(1);
					}
				}
				char *newtoken = clientmsg;
				newtoken = strtok(clientmsg," ");
				print=0;
				int a,b,i,count,found;
				a=0;
				count=0;
				while(newtoken!=NULL){
					if(print>0){
						int delayTime = delay(newtoken);
						if(delayTime!=-1){
							usleep(1000*delayTime);
							printf("I kinda slept\n");
						}
						else{
							int test = hash(newtoken)/5;
							found=0;
							for(i=0;i!=count;i++){
								if(test==muts[i])
									found=1;
							}
							if(found==0){
								muts[count] = test;
								count++;
							}
							strcpy(namesprints[a],newtoken);
							a++;
						}
					}
					newtoken = strtok(NULL, " ");
					print++;
				}
				int error;
				error=0;
				char multiprint[1000];
				memset(multiprint,0,1000);
				bubblesort(muts,names);
				for(i=0;i!=count;i++)
					pthread_mutex_lock(&mutexes[muts[i]]);
				char *namefrom = namesprints[0];
				for(b=1;b!=names && error==0;b++){
					if(addTran(namefrom,namesprints[b],amount)==0)
						error=1;
				}
				if(!error)
					sprintf(message,"Success. Multi-Transfer addition (%s:%d)",namefrom,amount);
				else
					sprintf(message,"Error. Multi-Transfer addition failure (%s:%d)",namefrom,amount);
				for(i=0;i!=count;i++)
					pthread_mutex_unlock(&mutexes[muts[i]]);
			}
		}
		else if(!strcmp(token,"print")){
			if(token!=NULL){
				int i;
				for(i=0;i!=hashSize;i++){ //DEBUGGING PURPOSES
					printf("[%d] = ",i);
					printtest(i);
					printf("\n");
				}
				sprintf(message,"Success. Printed the debugging stuff\n");
			}
		}
		else if(!strcmp(token,"print_balance")){
			if(token!=NULL){
				int print;
				char *name;
				print=0;
				while(token!=NULL){
					if(print==1){
						name=malloc(strlen(token)*sizeof(char));
						strcpy(name,token);
					}
					if(print>1){
						int delayTime = delay(token);
						if(delayTime!=-1)
							usleep(1000*delayTime);
					}
					token = strtok(NULL, " ");
					print++;
				}
				int mut;
				mut = hash(name)/5;
				pthread_mutex_lock(&mutexes[mut]);
				hashNode *hn;
				hn=searchNode(name);
				if(hn!=NULL)
					sprintf(message,"Success. Balance (%s:%d)",hn->name,hn->amount);
				else
					sprintf(message,"Error. Balance (%s)",name);
				pthread_mutex_unlock(&mutexes[mut]);
			}
		}
		else if(!strcmp(token,"print_multi_balance")){
			if(token!=NULL){
				int print,names,j;
				print = 0;
				names = 0;
				while(token!=NULL){
					if(print>0){
						int delayTime = delay(token);
						if(delayTime!=-1){
							usleep(1000*delayTime);
							printf("I kinda slept\n");
						}
						else names++;
					}
					token = strtok(NULL, " ");
					print++;
				}
				char **namesprints;
				int *muts;
				muts = malloc(names*sizeof(int));
				namesprints = malloc(names*sizeof(char*));
				if(namesprints==NULL){
					printf("Critical error while malloc-ing\n");
					exit(1);
				}
				for(j=0;j!=names;j++){
					*(namesprints+j) = malloc(20*sizeof(char*));
					if(*(namesprints+j)==NULL){
						printf("Critical error while malloc-ing\n");
						exit(1);
					}
				}
				char *newtoken = clientmsg;
				newtoken = strtok(clientmsg," ");
				print=0;
				int a,b,i,count,found;
				a=0;
				count=0;
				while(newtoken!=NULL){
					if(print>0){
						int delayTime = delay(newtoken);
						if(delayTime!=-1){
							usleep(1000*delayTime);
							printf("I kinda slept\n");
						}
						else{
							int test = hash(newtoken)/5;
							found=0;
							for(i=0;i!=count;i++){
								if(test==muts[i])
									found=1;
							}
							if(found==0){
								muts[count] = test;
								count++;
							}
							strcpy(namesprints[a],newtoken);
							a++;
						}
					}
					newtoken = strtok(NULL, " ");
					print++;
				}
				int error;
				error=0;
				char multiprint[1000];
				memset(multiprint,0,1000);
				bubblesort(muts,names);
				for(i=0;i!=count;i++)
					pthread_mutex_lock(&mutexes[muts[i]]);
				for(b=0;b!=names && error==0;b++){
					hashNode *hn;
					hn=searchNode(namesprints[b]);
					if(hn==NULL) error=1;
					else{
						char amountch[10];
						memset(amountch,0,10);
						strcat(multiprint,hn->name);
						strcat(multiprint,"/");
						sprintf(amountch,"%d",hn->amount);
						strcat(multiprint,amountch);
						if(b<names-1)
							strcat(multiprint,":");
					}
				}
				if(error==1){
					memset(multiprint,0,1000);
					for(b=0;b!=names;b++){
						strcat(multiprint,namesprints[b]);
						if(b<names-1)
							strcat(multiprint,":");
					}
				}
				if(error==0)
					sprintf(message,"Success. Multi-Balance %s",multiprint);
				else
					sprintf(message,"Error. Multi-Balance %s",multiprint);	
				for(i=0;i!=count;i++)
					pthread_mutex_unlock(&mutexes[muts[i]]);
			}
		}
		else if(!strcmp(token,"sleep")){
			if(token!=NULL){
				int sleep;
				sleep=0;
				while(token!=NULL){
					if(sleep==1){
						sprintf(message,"You slept for %d milliseconds",atoi(token));
					}
					token = strtok(NULL," ");
					sleep++;
				}
			}
		}
		else
			sprintf(message,"Error, Unknown command");
		write(sock , message , strlen(message));
		memset(client_message, 0, 2000);
		memset(message, 0, 2000);
		memset(clientmsg, 0, 2000);
	}
	if(read_size == 0)
	{
		printf("Client with fd %d disconnected",sock);
		fflush(stdout);
	}
	else if(read_size == -1)
	{
		perror("recv failed");
	}
		 
	return 0;
}
