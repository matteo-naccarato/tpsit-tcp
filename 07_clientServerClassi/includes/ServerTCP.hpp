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

	private: std::list<ConnessioneServer*> connessioni;

	public: ServerTCP(Address);	// bind() + listen()
			ConnessioneServer* accetta();
			void chiudi(ConnessioneServer*);
			void chiudi();
			~ServerTCP();		
};


ServerTCP::ServerTCP(Address myself) : SocketTCP() {
	socklen_t len = sizeof(struct sockaddr);
	if ( bind(this->sock_id,
				(struct sockaddr*) myself.getAddress(),
				len) )
		errore((char*) "bind()", -3);
	printf("Bind successfully competed\n");

	printf("Server is listening on %s:%d ...\n", myself.getIp(), myself.getPort());
	if ( listen(this->sock_id, MAX_CONN) )
		errore((char*) "listen()", -4);
}


ConnessioneServer* ServerTCP::accetta() {
	struct sockaddr_in client;
	socklen_t len = sizeof(struct sockaddr);
	int connId = accept(this->sock_id,
						(struct sockaddr*) &client,
						&len);
	if (connId == -1) errore((char*) "accept()", -5);
	ConnessioneServer* conn = new ConnessioneServer(connId);
	this->connessioni.push_back(conn);
	return conn;
}

void ServerTCP::chiudi(ConnessioneServer* conn) {
	connessioni.remove(conn);
	delete conn;
}

void ServerTCP::chiudi() {
	for (std::list<ConnessioneServer*>::iterator it = connessioni.begin(); it != connessioni.end(); it++) {
		delete *it;
	}
	close(this->sock_id);
}

#endif // __SERVER_TCP_