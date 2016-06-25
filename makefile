CC=gcc
CFLAGS=-c -g 

all: bankclient bankserver

bankclient: bankclient.c
	$(CC) -o bankclient bankclient.c -Wall

bankserver: bankserver.c serveraux.c serveraux.h threads.h threads.c
	$(CC) -pthread -o bankserver bankserver.c serveraux.c serveraux.h threads.c threads.h -Wall

clean: 
	$(RM) bankserver
