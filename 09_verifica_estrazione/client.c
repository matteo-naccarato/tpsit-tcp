#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "includes/utilities.h"

#define MAX_STR 1024*1024
#define FILE_NOT_FOUND "404 not found"
#define FUORI_SCALA "errore: numero richiesto MAGGIORE del numero di presenti nella classe"

int main(int argc, char* argv[]) {

	if (argc != 5) {
		printf("USAGE: %s IP_SERVER PORT_SERVER FILE_CLASSE N_ESTRATTI\n", argv[0]);
		return -1;
	}

	char* ip = argv[1];
	int port = atoi(argv[2]);
	char* fileClasse = argv[3];
	int nEstratti = atoi(argv[4]);

	int sock_id = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_id == -1) errore("socket()", -2);

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	inet_aton(ip, &server.sin_addr);
	server.sin_port = htons(port);
	for (int i=0; i<8; i++) server.sin_zero[i] = 0;

	if ( connect(sock_id,
					(struct sockaddr*) &server,
					(socklen_t) sizeof(struct sockaddr_in)) )
		errore("connect()", -3);

	char msg[MAX_STR];
	strcat(msg, fileClasse);
	strcat(msg, " ");
	strcat(msg, argv[4]);

	if ( send(sock_id, 
				msg,
				strlen(msg) + 1,
				0)
		!= strlen(msg) + 1 )
			errore("send()", -4);
	printf("requested to \t[%s:%d] \t'%s'\n\n", ip, port, msg);

	bool fileTrovato = true;
	char buffer[MAX_STR + 1];
	int rc, i = 0;
	while (i < nEstratti && fileTrovato) {
		rc = recv(sock_id,
					buffer,
					MAX_STR + 1,
					0);
		if (rc == -1) errore("recv()", -5);
		buffer[rc] = '\0';

		if (!strcmp(buffer, FILE_NOT_FOUND)
			|| !strcmp(buffer, FUORI_SCALA)) // strcmp() == 0
				fileTrovato = false;
			
		printf("received from \t[%s:%d] \t'%s'\n", ip, port, buffer);

		i++;
	}

	close(sock_id);

	return 0;
}