#ifndef __CLIENT_TCP_
#define __CLIENT_TCP_

#include "SocketTCP.hpp"
#include "ConnessioneClient.hpp"
#include "Address.hpp"
#include "utilities.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>


#define MAX_MSG 1024


class ClientTCP : public SocketTCP {

	private: ConnessioneClient connessione;

	public: ClientTCP(Address);		// connect()
			int invia(char*);	
			int invia(void*, int);	
			char* ricevi();	
			void* ricevi(int*);	
};


ClientTCP::ClientTCP(Address server) : SocketTCP() {

	this->connessione = ConnessioneClient(this->sock_id);
	if ( connect(this->sock_id,
				(struct sockaddr*) server.getAddress(),
				(socklen_t) sizeof(struct sockaddr_in)) )
		errore((char*) "ClientTCP::connect()", -3);
}


int ClientTCP::invia(char* msg) {
	return connessione.invia(msg);
}

int ClientTCP::invia(void* buffer, int len) {
	return connessione.invia(buffer, len);
}

char* ClientTCP::ricevi() {
	return connessione.ricevi();
}

void* ClientTCP::ricevi(int* len) {
	return connessione.ricevi(len);
}


#endif // __CLIENT_TCP_