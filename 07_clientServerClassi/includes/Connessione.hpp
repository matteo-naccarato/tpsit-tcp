#ifndef __CONNESSIONE_
#define __CONNESSIONE_

#include "Address.hpp"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_MSG 1024


class Connessione {

	protected: int id;

	public: Connessione();
			Connessione(int);
			int invia(char*);	
			int invia(void*, int);	// send()
			char* ricevi();		
			void* ricevi(int*);		// recv()
};

Connessione::Connessione() {
	
}

Connessione::Connessione(int id) {
	this->id = id;
}

int Connessione::invia(char* msg) {
	msg[strlen(msg) + 1] = '\0';
	return invia( (void*) msg, (strlen(msg) +1));
}

int Connessione::invia(void* buffer, int len) {
	return ( send(this->id,
					buffer,
					len,
					0)
			!= len )? -1: 0;
}

char* Connessione::ricevi() {
	int len;
	char* buffer = (char*) ricevi(&len);

	if (len <= 0) return NULL;

	buffer[len] = '\0';
	return buffer;
}

void* Connessione::ricevi(int* len) {
	char* buffer = (char*) malloc(sizeof(char*) * (MAX_MSG + 1));

	int rc = recv(this->id,
					buffer,
					MAX_MSG + 1,
					0);
	*len = rc;

	if (rc > 0 ) return (void*) buffer;

	free(buffer);
	return NULL;
}


#endif // __CONNESSIONE_