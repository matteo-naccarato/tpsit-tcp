#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>




 // #include <regex.h> => ???

#include "includes/utilities.h"

#define LO "127.0.0.1"
#define DEFAULT_PORT 7071
#define MAX_PACK 1024*1024
#define CODE200 "HTTP/1.1 200 OK\n\
\n"
#define CODE404 "HTTP/1.1 404 Not Found\n\
\n"
#define GET_URL_HEADER "GET "
#define GET_URL_FOOTER " HTTP"
#define PROMPT "$>"
#define EXIT_CMD "quit"
#define REGEX_TAG_SQL "<SQL * />"

typedef struct {
	int sock_id;
	struct sockaddr_in client;
	socklen_t len;
} ListeningParams;

typedef struct {
	int connId;
} ResponseParams;


void* listening(void*);
void* response(void*);
char* readFile(FILE*);


int main(int argc, char* argv[]) {

	if (argc != 3) {
		printf("USAGE: %s file.html .db\n", argv[0]);
		return -1;
	}

	int port = DEFAULT_PORT;
	char* fileHTML = argv[1];
	char* dbName = argv[2];

	printf("init ...\n");

	// CREAZIONE SERVER
	// 	(edit =>) con la classe ServerTCP
	// 	('' =>) cambiare valori return

	struct sockaddr_in myself, client;
	myself.sin_family = AF_INET;
	inet_aton(LO, &myself.sin_addr);
	myself.sin_port = htons(port);
	for (int i=0; i<8; i++) myself.sin_zero[i] = 0;

	int sock_id = socket(AF_INET, SOCK_STREAM, 0);
	if ( sock_id <= 0 ) errore("socket()", -5);

	socklen_t len = sizeof(struct sockaddr);
	if ( bind(sock_id, (struct sockaddr*) &myself, len) == -1)
		errore("bind()", -6);

	printf("Server listening on %s:%d ...\n", LO, port);
	if ( listen(sock_id, 50)) errore("listen()", -5); 

	ListeningParams params = { sock_id, client, len };


	pthread_t thread_id;
	if ( pthread_create(&thread_id, NULL, listening, (void*) &params))
		errore("pthread_create()", -1);

	printf("%s \n", PROMPT);
	char* cmd = inputStr();
	while ( strcmp(cmd, EXIT_CMD) ) {
		free(cmd);
		cmd = inputStr();
	}
	free(cmd);

	// server->chiudi();

	close(sock_id);

	return 0;
}

void* listening(void* param) {

	// server
	ListeningParams* p = (ListeningParams*) param;
	int connId;

	while (1) {
		// server->accetta
		// if ( == -1) errore("accept()", -2);
		connId = accept(p->sock_id,
						(struct sockaddr*) &p->client,
						&p->len);
		if (connId == -1) errore("accept()", -5);
		printf("Client has connected! ...\n");

		ResponseParams param = { connId };

		pthread_t thread_id;
		if ( pthread_create(&thread_id, NULL, response, (void*) &connId))
			errore("pthread_create()", -3);
	}

}


void* response(void* param) {

	// server
	ResponseParams* p = (ResponseParams*) param;
	int rc;
	int connId = p->connId;

	char buffer[MAX_PACK + 1];
	rc = recv(connId,
				buffer,
				MAX_PACK,
				0);
	if (rc <= 0) errore("recv()", -7);

	// char buffer[MAX_PACK + 1];
	/* char* buffer = server->ricevi();
	if (buffer == NULL) errore("ricevi()", -3); */
	printf("Client is making a request ...\n");
	buffer[rc] = '\0';

	// edit succssiva => ricerca file
	// ora ce l'ho come parametro

	FILE* fpHTML = fopen(/*fileHTML*/ "index.html", "r");
	// free(fileHTML);
	char msg[MAX_PACK + strlen(CODE200) + 1];
	int len = 0;
	if (fpHTML == NULL) {
		// ...
		// return -5;
		return NULL;
	}

	sprintf(msg, "%s%s", CODE200, readFile(fpHTML));
	// rc = server->invia(msg);
	// if (rc) errore("invia()", -4);
	if ( send(connId, msg, len, 0) != strlen(msg) ) errore("send()", -6);
	printf("Answer sent\n");

	printf("Closing connection\n\n");
	// server->chidi();
	shutdown(connId, SHUT_RDWR);
	close(connId);
}


char* readFile(FILE* fp) {
	char* std = "ciao";
	int len = 0;
	char* buffer = (char*) malloc(sizeof(char) * (MAX_PACK+1));
	while ( (*(buffer+len) = fgetc(fp)) != EOF )
		len++;

	*(buffer+len) = '\0';
	fclose(fp);

	printf("%s\n\n%s", buffer, REGEX_TAG_SQL);
	fflush(stdout);

	
	// individuo la presenza del tag <SQL "query" />
	// ricontrollare le FREE
	char* sqlQuery = (char*) malloc(sizeof(char) * (MAX_PACK+1));
	sqlQuery = strdup(strstr(buffer, REGEX_TAG_SQL) + 4);
	printf("%s\n", sqlQuery);
	fflush(stdout); 


	return std;  
}

