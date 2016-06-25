#ifndef THREADS_H_
#define THREADS_H_
#include <pthread.h>
#define hashSize 50
#define perror2(s,e) fprintf(stderr,"%s : %s \n",s,strerror(e))


typedef struct mutex {
	pthread_mutex_t mutex;
	pthread_cond_t   cond;
	int v;
} mutex;

void bubblesort(int *list,int no);
void *connection_handler();

#endif
