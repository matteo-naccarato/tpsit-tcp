#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "includes/utilities.h"

#define MAX_CONN 10
#define IP_DHCP "0.0.0.0" 
#define MAX_STR 1024


typedef struct {
	int connId;
	struct sockaddr_in client;
	char* msg;
} Params;


void* mythread(void*);


int main(int argc, char* argv[]) {

	// controllo input
	if (argc != 3) {
		printf("USAGE: %s PORT MESSAGE\n", argv[0]);
		return -1;
	}

	// assegnazione input
	int port = atoi(argv[1]);
	char* msg = argv[2];


	// apertura socket
	int sock_id = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_id == -1) errore("socket()", -2);

	// indirizzo myself
	struct sockaddr_in myself;
	myself.sin_family = AF_INET;
	inet_aton(IP_DHCP, &myself.sin_addr);
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

		Params param = { connId, client, msg };

		pthread_t thread_id;
		if ( pthread_create(&thread_id, NULL, mythread, (void*) &param) ) errore("pthread_create()", -6);
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
	if (rc <= 0) errore("recv()", -6);
	buffer[rc] = '\0';
	char* ipClient = strdup(inet_ntoa(p->client.sin_addr));
	int portClient = ntohs(p->client.sin_port);
	printf("received from \t[%s:%d] \t'%s'\n", ipClient, portClient, buffer);


	rc = send(p->connId,
				p->msg,
				strlen(p->msg) + 1,
				0);
	if (rc != strlen(p->msg) + 1) errore("send()", -7);
	printf("sent to \t[%s:%d] \t'%s'\n", ipClient, portClient, p->msg);


	// chiudo la connessione, sia in lettura sia in scrittura
	shutdown(p->connId, SHUT_RDWR);

	// free(p); p non e' piu' allocato in modo dinamico


	pthread_exit(NULL);
}