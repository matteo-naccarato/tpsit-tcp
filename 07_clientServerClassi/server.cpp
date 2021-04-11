#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "includes/ServerTCP.hpp"
#include "includes/utilities.h"

#define MSG "ricevuto :)"
#define PROMPT "$>"
#define EXIT_CMD "quit"


void* listening(void*);
void* response(void*);

char* testa;

typedef struct {
	ServerTCP* myself;
	ConnessioneServer* conn;
} ResponseParams;

int main(int argc, char* argv[]) {

	printf("Init ...\n");
	int port = argc>1? atoi(argv[1]) : DEFAULT_PORT;

	testa = argv[2];

	Address myself((char*) IP_LO, port);
	ServerTCP* serverTCP = new ServerTCP(myself);

	pthread_t thread_id;
	if ( pthread_create(&thread_id, NULL, listening, (void*) serverTCP) )
		errore((char*) "pthread_create()", -1);

	printf("\n");
	char* cmd = inputStr();
	while ( strcmp(cmd, EXIT_CMD) ) {
		free(cmd);
		cmd = inputStr();
	}
	free(cmd);

	serverTCP->chiudi();
	printf("Server closed!\n");
	return 0;
}

void* listening(void* param) {
	ServerTCP* serverTCP = (ServerTCP*) param;

	while (1) {
		ConnessioneServer* conn = serverTCP->accetta();
		printf("Client has connected! ...\n");

		ResponseParams params = { serverTCP, conn };
		pthread_t thread_id;
		if ( pthread_create(&thread_id, NULL, response, (void*) &params) )
			errore((char*) "pthread_create()", -6);
	}

	return NULL;
}

void* response(void* params) {
	ResponseParams* p = (ResponseParams*) params;

	char* resp = strdup( p->conn->ricevi() );
	if (resp == NULL) errore((char*) "ricevi()", -7);
	printf("Client is making a request ... he said: \"%s\"\n", resp);
	free(resp);

	if ( p->conn->invia((char*) MSG) ) 
		errore((char*) "invia()", -8);
	printf("Answer sent!\n");
	
	p->myself->chiudi(p->conn);
	printf("Connection closed!\n\n");
	return NULL;
}