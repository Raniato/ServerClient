#ifndef SERVER_H_
#define SERVER_H_
#include <pthread.h>

#define hashSize 50	//we take the hashtable size as granted
#define noMutex 10 	//one mutex for every 5 hashtable buckets

typedef struct {
	int *data;
	int pool_size;
	int start ;
	int end ;
	int count ;
} pool_t ;

typedef struct node1{
	char *name;
	int amount;
	struct node1 *next;
}listNode;

typedef struct node2{
	char *name;
	int amount;
	struct node2 *next;
	listNode *list;
}hashNode;

pthread_mutex_t mutexes[noMutex];

pool_t pool;
hashNode **hp;
pthread_cond_t cond_nonempty ;
pthread_cond_t cond_nonfull ;
pthread_mutex_t mtx ;

void initialize ( pool_t * pool, int queueSize );
void place ( pool_t * pool , int data, int queueSize );
int obtain(pool_t *pool);
int hash(char *name);
hashNode* searchNode(char *name);
void hashInsert(char *name, int amount);
int addTran(char *name1,char *name2, int amount);
void printtest(int i);

#endif
