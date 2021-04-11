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

int main(int argc, char* argv[]) {

	int port = argc>1? atoi(argv[1]) : DEFAULT_PORT;

	printf("init ...\n");

	Address myself((char*) IP_LO, port);
	ServerTCP* serverTCP = new ServerTCP(myself);

	pthread_t thread_id;
	if ( pthread_create(&thread_id, NULL, listening, (void*) serverTCP) )
		errore((char*) "pthread_create()", -1);

	printf("%s \n", PROMPT);
	char* cmd = inputStr();
	while ( strcmp(cmd, EXIT_CMD) ) {
		free(cmd);
		cmd = inputStr();
	}
	free(cmd);


	delete serverTCP;
	return 0;
}

void* listening(void* param) {
	ServerTCP* serverTCP = (ServerTCP*) param;

	while (1) {
		ConnessioneServer conn = serverTCP->accetta();
		printf("Client has connected! ...\n");

		pthread_t thread_id;
		if ( pthread_create(&thread_id, NULL, response, (void*) &conn) )
			errore((char*) "pthread_create()", -6);
	}

	return NULL;
}

void* response(void* param) {
	ConnessioneServer* conn = (ConnessioneServer*) param;

	char* resp = strdup( conn->ricevi() );
	if (resp == NULL) errore((char*) "ricevi()", -7);
	printf("Client is making a request ...\n");

	if ( conn->invia((char*) MSG) != strlen(MSG) ) 
		errore((char*) "invia()", -8);
	printf("Answer sent!\n");

	printf("Closing connection ...\n\n");

	// delete conn;
	return NULL;
}