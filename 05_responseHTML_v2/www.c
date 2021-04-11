#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "includes/utilities.h"

#define LO "127.0.0.1"
#define DEFAULT_PORT 7070
#define MAX_PACK 1024*1024
#define CODE200 "HTTP/1.1 200 OK\n\
\n"
#define CODE404 "HTTP/1.1 404 Not Found\n\
\n"
#define GET_URL_HEADER "GET "
#define GET_URL_FOOTER " HTTP"
#define URL_404 "404.html"
#define PROMPT "$>"
#define EXIT_CMD "quit"

typedef struct {
	int sock_id;
	struct sockaddr_in client;
	socklen_t len;
} ListeningParams;

typedef struct {
	int connId;
} ReponseParams;

void* listening(void*);
void* response(void*);
char* readFile(FILE*);

int main(int argc, char* argv[]) {
	int port = argc>1? atoi(argv[1]) : DEFAULT_PORT;

	printf("init...\n");

	struct sockaddr_in myself, client;
	myself.sin_family = AF_INET;
	inet_aton(LO, &myself.sin_addr);
	myself.sin_port = htons(port);
	for (int i=0; i<8; i++) myself.sin_zero[i] = 0;

	int sock_id = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_id <= 0) errore("socket()", -1);

	socklen_t len = sizeof(struct sockaddr);
	if (bind(sock_id, (struct sockaddr*) &myself, len) == -1)
		errore("bind()", -2);

	printf("Server listening on %s:%d...\n", LO, port);
	if (listen(sock_id, 50)) 
		errore("listen()", -3);

	ListeningParams params = { sock_id, client, len };

	pthread_t thread_id;
	if ( pthread_create(&thread_id, NULL, listening, (void*) &params) )
		errore("pthread_create()", -4);
	printf("%s \n", PROMPT);
	char* cmd = inputStr();
	while ( strcmp(cmd, EXIT_CMD) ) {
		free(cmd);
		cmd = inputStr();
	}
	free(cmd);


	close(sock_id);

	return 0;
}

void* listening(void* param) {
	ListeningParams* p = (ListeningParams*) param;

	int connId;

	while (1) {
		connId = accept(p->sock_id, 
							(struct sockaddr*) &p->client,
							&p->len);
		if (connId == -1) errore("accept()", -5);	
		printf("Client has connected!...\n");

		ReponseParams param = { connId };

		pthread_t thread_id;
		if ( pthread_create(&thread_id, NULL, response, (void*) &connId) )
			errore("pthread_create()", -6);
	}
}


void* response(void* param) {
	ReponseParams* p = (ReponseParams*) param;
	int rc;
	int connId = p->connId;

	char buffer[MAX_PACK + 1];
	rc = recv(connId, 
				buffer, 
				MAX_PACK, 
				0);
	if (rc < 0) errore("recv()", -7);
	printf("Client is making a request...\n");
	buffer[rc] = '\0';

    // strstr() -> restituisce la prima corrispondenza 
	char* url = strdup(strstr(buffer, GET_URL_HEADER) + 3);
	*url = '.';
	*strstr(url, GET_URL_FOOTER) = '\0';

	printf("Now looks for file %s\n", url);

	FILE* fp = fopen(url, "r");
	free(url);

	char msg[MAX_PACK + strlen(CODE200) + 1];
	int len = 0;
	if (fp != NULL) {
		sprintf(msg, "%s%s", CODE200, readFile(fp));
	} else {
		printf("File Not Found!\n");
		FILE* fpNotFound = fopen(URL_404, "r");
		if (fpNotFound != NULL)
			sprintf(msg, "%s%s", CODE404, readFile(fpNotFound));
		else printf("Error opening 404 file!\n");;
	}

	len = strlen(msg);
	if ( send(connId, msg, len, 0) != strlen(msg) ) 
		errore("send()", -8);
	printf("Answer sent\n");

	printf("Closing connection\n\n");
	shutdown(connId, SHUT_RDWR);	
}

char* readFile(FILE* fp) {
	int len = 0;
	char* buffer = (char*) malloc(sizeof(char) * (MAX_PACK+1));

	while ( (*(buffer+len) = fgetc(fp)) != EOF )
		len++;
	*(buffer+len) = '\0';
	fclose(fp);

	return buffer;
}