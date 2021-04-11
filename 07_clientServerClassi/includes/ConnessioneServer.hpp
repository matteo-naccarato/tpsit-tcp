#ifndef __CONNESSIONE_SERVER_
#define __CONNESSIONE_SERVER_

#include "Connessione.hpp"
#include "Address.hpp"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>


class ConnessioneServer : public Connessione {

	public: ConnessioneServer();	
			ConnessioneServer(int);
			~ConnessioneServer();
			
};

ConnessioneServer::ConnessioneServer() : Connessione() {}
ConnessioneServer::ConnessioneServer(int id) : Connessione(id) {
	this->id = id;
}

ConnessioneServer::~ConnessioneServer() {
	shutdown(this->id, SHUT_RDWR);
}


#endif // __CONNESSIONE_SERVER_