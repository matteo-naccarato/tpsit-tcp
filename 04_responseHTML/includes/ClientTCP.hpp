#ifndef __CLIENT_TCP_
#define __CLIENT_TCP_

#include "Address.hpp"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>


#define MAX_MSG 1024


class ClientTCP {

	private: int sock_id;

	public: ClientTCP();			// socket()

			int connetti(Address);	// connect()
			int invia(char*); 		// send()
			int invia(void*, int);
			char* ricevi();			// recv()
			void* ricevi(int*);

			~ClientTCP();			// close()

};


ClientTCP::ClientTCP() {
	this->sock_id = socket(AF_INET, SOCK_STREAM, 0);
}

ClientTCP::~ClientTCP() {
	close(sock_id);
	delete(&this->sock_id);
}


int ClientTCP::connetti(Address server) {
	struct sockaddr_in* serverAddr = server.getAddress();
	return (this->sock_id)? connect(this->sock_id,
								(struct sockaddr*) serverAddr,
								(socklen_t) sizeof(struct sockaddr_in))
							: -1;
}


// invio stringa
int ClientTCP::invia(char* msg) {
	msg[strlen(msg) + 1] = '\0';
	return invia( (void*) msg, strlen(msg) + 1 );
}


// invio generico
int ClientTCP::invia(void* buffer, int len) {
	return ( send(this->sock_id,
				buffer,
				len,
				0)
			!= len)? -1:0;
}


// ricezione stringa
char* ClientTCP::ricevi() {
	int len;
	char* buffer = (char*) ricevi(&len);

	if (len <= 0) return NULL;

	buffer[len] = '\0';
	return buffer;
}


// ricezione generica
void* ClientTCP::ricevi(int* len) {
	char* buffer = (char*) malloc(sizeof(char*) * (MAX_MSG + 1));

	int rc = recv(this->sock_id,
					buffer,
					MAX_MSG + 1,
					0);
	*len = rc;

	if (rc > 0 ) return (void*) buffer;

	free(buffer);
	return NULL;
}




#endif // __CLIENT_TCP_