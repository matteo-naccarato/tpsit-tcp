#ifndef __ADDRESS_HPP
#define __ADDRESS_HPP

#include "utilities.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define IP_LO "127.0.0.1"
#define IP_DHCP "0.0.0.0"
#define MAX_STR 1024

class Address {
	private: struct sockaddr_in * address;

	public: static pthread_mutex_t mutex_id;
			Address(char*, int);
			Address(struct sockaddr_in*);
			Address();
			Address(const Address&);

			~Address();

			Address*	setIp(char*);
			Address*	setPort(int);
			Address*	setAddress(struct sockaddr_in*);

			char*		getIp();
			int 		getPort();
			struct sockaddr_in* getAddress();

			char* toString();
};

pthread_mutex_t Address::mutex_id = PTHREAD_MUTEX_INITIALIZER;

Address::Address(char* ip, int port) {
	// allocazione dinamica spazio di memoria per la struttura sockaddr_in
	this->address = (sockaddr_in*) malloc(sizeof(sockaddr_in));

	// assegnazione valori alla struttura
	this->address->sin_family = AF_INET;
	inet_aton(ip, &this->address->sin_addr);
	this->address->sin_port = htons(port);
	for (int i=0; i<8; i++) this->address->sin_zero[i] = 0;
}

Address::Address(struct sockaddr_in* add) {
	// allocazione dinamica spazio di memoria per la struttura sockaddr_in
	this->address = (sockaddr_in*) malloc(sizeof(sockaddr_in));

	// assegnazione valori alla struttura
	this->address->sin_family = AF_INET;
	// regione critica
	pthread_mutex_lock(&mutex_id);
	this->setIp(strdup(inet_ntoa(add->sin_addr)));
	pthread_mutex_unlock(&mutex_id);
	// fine regione critica
	this->setPort(ntohs(add->sin_port));
	for (int i=0; i<8; i++) this->address->sin_zero[i] = 0;
}

Address::Address():Address((char*)IP_DHCP, 0) {}

// costruttore di copia
Address::Address(const Address& old_addr) {
	this->address = (sockaddr_in*) malloc(sizeof(sockaddr_in));
	*(this->address) = *(old_addr.address);
}

Address::~Address() {
	delete(this->address);
}

Address* Address::setIp(char* ip) {
	inet_aton(ip, &this->address->sin_addr);
	return this;
}

Address* Address::setPort(int port) {
	this->address->sin_port = htons(port);
	return this;
}

Address* Address::setAddress(struct sockaddr_in* add) {
	// regione critica
	pthread_mutex_lock(&mutex_id);
	this->setIp(strdup(inet_ntoa(add->sin_addr)));
	pthread_mutex_unlock(&mutex_id);
	// fine regione critica

	this->setPort(ntohs(add->sin_port));
	return this;
}

char* Address::getIp() {
	// regione critica
	pthread_mutex_lock(&mutex_id);
	char* ip = strdup(inet_ntoa(this->address->sin_addr));
	pthread_mutex_unlock(&mutex_id);
	// fine regione critica

	return ip;
}

int Address::getPort() {
	return ntohs(this->address->sin_port);
}

struct sockaddr_in* Address::getAddress() {
	return this->address;
}

char* Address::toString() {
	char buffer[MAX_STR+1];
	char* ip = this->getIp();
	sprintf(buffer, "[%s:%d]", ip, this->getPort());
	free(ip);
	return strdup(buffer);
}

#endif