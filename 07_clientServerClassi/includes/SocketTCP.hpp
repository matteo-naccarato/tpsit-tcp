#ifndef __SOCKET_TCP_
#define __SOCKET_TCP_

#include "Address.hpp"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>


class SocketTCP {

	protected: int sock_id;

	public: SocketTCP();	// socket()
			~SocketTCP(); 	// close()			;; ~ => AltGr + ì
};

SocketTCP::SocketTCP() {
	this->sock_id = socket(AF_INET, SOCK_STREAM, 0);
}

SocketTCP::~SocketTCP() {
	close(sock_id);
	delete(&this->sock_id);
}


#endif // __SOCKET_TCP_