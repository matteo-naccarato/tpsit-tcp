#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "includes/ClientTCP.hpp"
#include "includes/utilities.h"

int main(int argc, char* argv[]) {

	// controllo input
	if (argc != 4) {
		printf("USAGE: %s IP PORT MESSAGE\n", argv[0]);
		return -1;
	}

	// assegnazione input
	char* ip = argv[1];
	int port = atoi(argv[2]);
	char* msg = argv[3];


	ClientTCP* myself = new ClientTCP();
 
	Address server(ip, port);

	if ( myself->connetti(server) ) errore((char*) "connetti()", -2);

	if ( myself->invia(msg) ) errore((char*) "invia()", -3);
	printf("sent to \t[%s:%d] \t'%s'\n", ip, port, msg);

	char* resp = strdup(myself->ricevi());
	if ( resp == NULL ) errore((char*) "ricevi()", -4);
	printf("received from \t[%s:%d] \t'%s'\n", ip, port, resp);
	free(resp);

	return 0;
}