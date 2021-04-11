#include <stdio.h>
#include <stdlib.h>

#include "includes/ClientTCP.hpp"
#include "includes/utilities.h"

int main(int argc, char* argv[]) {

	if (argc != 4) {
		printf("USAGE: %s IP PORT MESSAGE\n", argv[0]);
		return -1;
	}

	char* ip = argv[1];
	int port = atoi(argv[2]);
	char* msg = argv[3];
	Address server(ip, port);

	ClientTCP* myself = new ClientTCP(server);

	if ( myself->invia(msg) ) errore((char*) "invia()", -2);
	printf("sent to \t[%s:%d] \t'%s'\n", ip, port, msg);

	char* resp = strdup( myself->ricevi() );
	if (resp == NULL) errore((char*) "ricevi()", -3);
	printf("received from \t[%s:%d] \t'%s'\n", ip, port, resp);
	free(resp);

	return 0;
}