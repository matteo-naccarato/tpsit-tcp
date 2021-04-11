#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "includes/utilities.h"

#define MAX_STR 1024

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


	// apertura socket
	int sock_id = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_id == -1) errore("socket()", -2);

	// server per effettuare la connessione
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	inet_aton(ip, &server.sin_addr);
	server.sin_port = htons(port);
	for (int i=0; i<8; i++) server.sin_zero[i] = 0;

	// creo connessione con il server
	int rc = connect(sock_id,
						(struct sockaddr*) &server,
						(socklen_t) sizeof(struct sockaddr_in));
	if (rc) errore("connect()", -3);

	// invio dati
	rc = send(sock_id,
				msg,
				strlen(msg) + 1,
				0);
	if (rc != strlen(msg) + 1) errore("send()", -4);
	printf("sent to \t[%s:%d] \t'%s'\n", ip, port, msg);

	// ricezione dati
	char buffer[MAX_STR + 1];
	rc = recv(sock_id,
				buffer,
				MAX_STR,
				0);
	if (rc < 0) errore("recv()", -5);
	printf("received from \t[%s:%d] \t'%s'\n", ip, port, buffer);

	close(sock_id);

	return 0;
}