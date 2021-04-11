#ifndef __SERVER_TCP_
#define __SERVER_TCP_


#include "ConnessioneServer.hpp"
#include "SocketTCP.hpp"
#include "Address.hpp"

#include <list>
#include <iterator>

#define MAX_CONN 10
#define MAX_MSG 1024
#define IP_DHCP "0.0.0.0"
#define IP_LO "127.0.0.1"
#define DEFAULT_PORT 7070


class ServerTCP : public SocketTCP {

	private: list<ConnessioneServer> connessioni;

	public: ServerTCP(Address);	// bind() + listen()
			ConnessioneServer accetta();
			void chiudi(ConnessioneServer);
			~ServerTCP();		
};


ServerTCP::ServerTCP(Address myself) : SocketTCP() {

	if ( bind(this->sock_id,
				(struct sockaddr*) myself.getAddress(),
				sizeof(myself)) )
		errore((char*) "bind()", -3);
	printf("Bind successfully competed\n");

	printf("Server is listening on %s:%d ...\n", myself.getIp(), myself.getPort());
	if ( listen(this->sock_id, MAX_CONN) )
		errore((char*) "listen()", -4);
}


ConnessioneServer ServerTCP::accetta() {
	int len;
	struct sockaddr_in client;
	int connId = accept(this->sock_id,
						(struct sockaddr*) &client,
						(socklen_t*) &len);
	if (connId == -1) errore((char*) "accept()", -5);
	ConnessioneServer conn(connId);
	this->connessioni.push_back(conn);
	return conn;
}



void ServerTCP::chiudi(ConnessioneServer conn) {
	// connessioni DA RIMUOVERE DALLA COLLEZIONE
	// delete conn;
}

ServerTCP::~ServerTCP() {
	// chiamata a tutti gli elementi nella collezione della connessione:
	//	- si chiudono una ad una
	//	- faccio la close del server

	/* list<int> :: interator it;
	for (it = this->connessioni.begin; it != this->connessioni.end; ++it)
		chiudi(*it); */

	// ~SocketTCP();
}



#endif // __SERVER_TCP_