#ifndef __CONNESSIONE_CLIENT_
#define __CONNESSIONE_CLIENT_

#include "Connessione.hpp"
#include "Address.hpp"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>


class ConnessioneClient : public Connessione {

	public: ConnessioneClient();
			ConnessioneClient(int);	
			
};

ConnessioneClient::ConnessioneClient() : Connessione() {}

ConnessioneClient::ConnessioneClient(int id) : Connessione(id) {}


#endif // __CONNESSIONE_CLIENT_