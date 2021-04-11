#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>

#include "includes/utilities.h"

#define MAX_CONN 10
#define IP_LO "127.0.0.1" 
#define MAX_STR 1024
#define RESPONSE_STATIC "HTTP/1.1 200 OK\n\
Content-Type: text/html; charset=UTF-8\n\
Server: il.mio.server.http.com\n\
\n\
<!DOCTYPE html>\n\
<html class=\"client-nojs\" lang=\"it\" dir=\"ltr\">\n\
<head>\n\
<title>Hello World</title>\n\
</head>\n\
<body>\n\
<h1> Hi! </h1>\n\
</body>\n\
</html>\n\
"
#define NOT_FOUND_STATIC "HTTP/1.1 404 NOT_FOUND\n\
Content-Type: text/html; charset=UTF-8\n\
Server: il.mio.server.http.com\n\
\n\
<!DOCTYPE html>\n\
<html class=\"client-nojs\" lang=\"it\" dir=\"ltr\">\n\
<head>\n\
<title>Mh</title>\n\
</head>\n\
<body>\n\
<h1> 404, qualcosa e' andato storto! (file not found) </h1>\n\
</body>\n\
</html>\n\
"


typedef struct {
	int connId;
	struct sockaddr_in client;
} Params;


void* mythread(void*);
char* readFile(FILE*);


int main(int argc, char* argv[]) {

	// controllo input
	if (argc != 2) {
		printf("USAGE: %s PORT\n", argv[0]);
		return -1;
	}

	// assegnazione input
	int port = atoi(argv[1]);


	// apertura socket
	int sock_id = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_id == -1) errore("socket()", -2);

	// indirizzo myself
	struct sockaddr_in myself;
	myself.sin_family = AF_INET;
	inet_aton(IP_LO, &myself.sin_addr);
	myself.sin_port = htons(port);
	for (int i=0; i<8; i++) myself.sin_zero[i] = 0;

	int rc = bind(sock_id,
					(struct sockaddr*) &myself,
					sizeof(myself));
	if (rc != 0) errore("bind()", -3);


	// accetta MAX_CONN connessioni simultanee sullo stesso socket (contemporaneamente attive: leggendo e scrivendo)
	rc = listen(sock_id, MAX_CONN);
	if (rc) errore("listen()", -4);

	// indirizzo client
	struct sockaddr_in client;
	int len = sizeof( (struct sockaddr*) &client);
	int connId;

	while (1) {
		connId = accept(sock_id, 
							(struct sockaddr*) &client,
							(socklen_t*) &len);
		if (connId == -1) errore("accept()", -5);	

		// Params param = { connId, client };
		Params param = { connId, client };

		pthread_t thread_id;
		if ( pthread_create(&thread_id, NULL, mythread, (void*) &param) ) 
			errore("pthread_create()", -6);
		pthread_join(thread_id, NULL);	
	}

	close(sock_id);

	return 0;
}


void* mythread(void* param) {
	Params* p = (Params*) param;

	// invio e ricevo sulla connId 
	char buffer[MAX_STR + 1];

	int rc = recv(p->connId,
				buffer,
				MAX_STR,
				0);
	if (rc < 0) errore("recv()", -6);
	buffer[rc] = '\0';
	char* ipClient = strdup(inet_ntoa(p->client.sin_addr));
	int portClient = ntohs(p->client.sin_port);
	printf("received from \t[%s:%d] \t'%s'\n", ipClient, portClient, buffer);



	/* ------------ GET FILE NAME ------------ */
	char* firstLine = (char*) malloc(sizeof(char) * MAX_STR);
	char* tmp; // di supporto durante la strchr
	
	tmp = strchr(buffer, '\n'); // looking for character \n
	int posNewLine = tmp - buffer + 1; // posizione carattere \n (found at x)

	strcpy(firstLine, buffer);
	*(firstLine + posNewLine) = '\0'; // \0 alla prima linea

	// example first line -> GET /pagina.html HTTP/1.1
	
	char* filename = (char*) malloc(sizeof(char) * MAX_STR);
	char* token = strtok(firstLine, " "); 
	if ( token != NULL ) token = strtok(NULL, " "); 
	strcpy(filename, token + 1); // togliere lo '/' prima del nome del file
	
	free(firstLine);

	char* res = (char*) malloc(sizeof(char) * MAX_STR);
	FILE* fp = fopen(filename, "r");

	if (fp != NULL) strcpy(res, readFile(fp));
	else {

		FILE* fpNotFound = fopen("404.html", "r");
		if (fpNotFound == NULL) 
			errore("fopen(404)", -7);
		
		strcpy(res, readFile(fpNotFound));	
	}

	rc = send(p->connId,
				res,
				strlen(res),
				0);
	if (rc != strlen(res)) errore("send()", -8);
	printf("sent to \t[%s:%d] \t'%s'\n", ipClient, portClient, res);

	free(res);
	free(filename);


	// chiudo la connessione, sia in lettura sia in scrittura
	shutdown(p->connId, SHUT_RDWR);

	pthread_exit(NULL);
}


// lettura di un file e restituzione del contenuto in un char*
char* readFile(FILE* fp) {

	char* ret = (char*) malloc(sizeof(char) * MAX_STR);

	char* line = NULL;
	size_t len = 0;
	ssize_t read;

	while ( (read = getline(&line, &len, fp)) != -1)
		strcat(ret, line); // concatenare due stringhe
	
	fclose(fp);

	if (line) free(line);

	return ret;
}