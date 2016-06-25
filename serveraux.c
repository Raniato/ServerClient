#include <string.h>    //strlen
#include <stdlib.h>    //strlen
#include <sys/socket.h>
#include <stdio.h>
#include <sys/types.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <pthread.h> //for threading , link with lpthread
#include <sys/time.h>
#include <errno.h>
#include "serveraux.h"

#define hashSize 50	//we take the hashtable size as granted


void initialize ( pool_t * pool, int queueSize ) {
	pool->start = 0;
	pool->end = -1;
	pool->count = 0;
	pool->pool_size = queueSize;
	pool->data = malloc(sizeof(int)*pool->pool_size);
}

void place (pool_t *pool, int fd,int queueSize ) {
	pthread_mutex_lock(&mtx);
	while(pool->count>=queueSize){
		printf (">>Too many requests\n" ) ;
		pthread_cond_wait(&cond_nonfull ,&mtx) ;
	}
	pool->end = (pool->end + 1) % queueSize;
	pool->data[pool->end] = fd ;
	pool->count++;
	pthread_mutex_unlock(&mtx);
}

int obtain(pool_t *pool){
	int data = 0;
	pthread_mutex_lock(&mtx);
	while ( pool->count<=0) {
		printf (">>No requests yet\n " ) ;
		pthread_cond_wait(&cond_nonempty, &mtx);
	}
	data = pool->data[pool->start];
	pool->start = (pool->start + 1) % (pool->pool_size);
	pool->count--;
	pthread_mutex_unlock(&mtx);
	return data;
}

int hash(char *name)
{
	return (*name) % hashSize;
}

hashNode* searchNode(char *name)
{
	int hashedValue = hash(name);
	int found = 0;
    	hashNode *hn = hp[hashedValue];
    	while(hn!=NULL && found==0){
		if(!strcmp(hn->name,name)){
			found=1;}
        	else hn = hn->next;
    	}
	if(found) return hn;
    	printf("Account does not exist\n");
	return NULL;
}

void hashInsert(char *name, int amount)
{
	int position = hash(name);
	if((hp[position]) == NULL){    				//creating a new list in the bucket [position]
		hp[position] = malloc(sizeof(hashNode));   
		hp[position]->amount = amount;
		hp[position]->next = NULL;
		hp[position]->list = NULL;
		hp[position]->name = malloc(strlen(name)*sizeof(char));
		strcpy(hp[position]->name,name);
	}
	else{							//already a list in this bucket
		hashNode *htNode = hp[position];
		while(htNode->next!=NULL)                         	//searching list's last node
			htNode = htNode->next;
		htNode->next = malloc(sizeof(hashNode));      	//new node goes in as the last one
		htNode->next->amount = amount;
		htNode->next->next = NULL;
		htNode->next->name = malloc(strlen(name)*sizeof(char));
		htNode->next->list = NULL;
		strcpy(htNode->next->name,name);
	}
}

int addTran(char *name1,char *name2, int amount)
{
	hashNode *node = searchNode(name2);
	hashNode *nodeInit = searchNode(name1);
	if(node==NULL || nodeInit==NULL) return 0;
	if(nodeInit->amount<amount) return 0;
	nodeInit->amount -=amount;
	node->amount+=amount;
	if(node->list==NULL){				//this account hasn't received any amounts from other accounts
		node->list = malloc(sizeof(listNode));
		node->list->name = malloc(strlen(name1)*sizeof(char));
		node->list->amount = amount;
		strcpy(node->list->name,name1);
		node->list->next=NULL;
	}
	else{						//this account has already received amounts from other accounts
		if(!strcmp(node->list->name,name1)){
			node->list->amount+=amount;	//already received amounts from this account
			return 1;
		}
		while(node->list->next!=NULL){
			if(!strcmp(node->list->name,name1)){
				node->list->amount+=amount;	//already received amounts from this account
				return 1;
			}
			node->list=node->list->next;
		}
		node->list->next = malloc(sizeof(listNode));
		node->list->next->name = malloc(strlen(name1)*sizeof(char));
		node->list->next->amount = amount;
		strcpy(node->list->next->name,name1);
		node->list->next->next=NULL;
	}
	return 1;
}

void printtest(int i){ //!!!DEBUG FUNCTION!!!
	hashNode *t = hp[i];
	while(t!=NULL){
		printf("%d | %s ",t->amount,t->name);  
		while(t->list!=NULL){
			printf(" -> %d | %s ",t->list->amount,t->list->name);
			t->list=t->list->next;
		} 
		printf("\n");
		t = t->next;
	}
}
