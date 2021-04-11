#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "includes/utilities.h"

#define MAX_CONN 10
#define IP_DHCP "0.0.0.0"
#define IP_LO "127.0.0.1"
#define MAX_STR 1024*1024
#define FILE_NOT_FOUND "404 not found"
#define FUORI_SCALA "errore: numero richiesto MAGGIORE del numero di presenti nella classe"
#define FOLDER "./data/"
#define EXIT_CMD "quit"
#define PROMPT "$>"


void* listening(void*);
void* th_receiver(void*);
void invia(int connId, char* msg);
char* takeStudent(FILE* fp, int indexDaEstrarre, int n, int fileLen);
bool isUnique(int num, int array[], int n);


typedef struct {
	int sock_id;
} ListeningParams;

typedef struct {
	int connId;
	struct sockaddr_in client;
} ReceiverParams;


int main(int argc, char* argv[]) {

	if (argc != 2) {
		printf("USAGE: %s PORT_SERVER\n", argv[0]);
		return -1;
	}

	int port = atoi(argv[1]);

	int sock_id = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_id == -1) errore("socket()", -2);

	struct sockaddr_in myself;
	myself.sin_family = AF_INET;
	inet_aton(IP_LO, &myself.sin_addr);
	myself.sin_port = htons(port);
	for (int i=0; i<8; i++) myself.sin_zero[i] = 0;

	if ( bind(sock_id,
				(struct sockaddr*) &myself,
				sizeof(myself)) )
		errore("bind()", -3);

	if ( listen(sock_id, MAX_CONN) )
		errore("listen()", -4);

	ListeningParams params = { sock_id };
	pthread_t thread_id;
	if ( pthread_create(&thread_id, NULL, listening, (void*) &params) )
		errore("pthread_create()", -5);

	printf("%s \n", PROMPT);
	char* cmd = inputStr();
	while ( strcmp(cmd, EXIT_CMD) ) {
		free(cmd);
		cmd = inputStr();
	}
	free(cmd);

	close(sock_id);
	return 0;
}


void* listening(void* params) {
	ListeningParams* p = (ListeningParams*) params;

	struct sockaddr_in client;
	int len = sizeof( (struct sockaddr*) &client);
	int connId;

	while (1) {
		connId = accept(p->sock_id,
						(struct sockaddr*) &client,
						(socklen_t*) &len);
		if (connId == -1) errore("accept()", -5);

		ReceiverParams params = { connId, client };
		pthread_t thread_id;
		if ( pthread_create(&thread_id, NULL, th_receiver, (void*) &params) )
			errore("pthread_create()", -6);
	}
}


void* th_receiver(void* params) {
	ReceiverParams* p = (ReceiverParams*) params;
	char buffer[MAX_STR + 1]; 

	int rc = recv(p->connId,
					buffer,
					MAX_STR,
					0);
	if (rc == -1)
		errore("recv()", -7);
	buffer[rc] = '\0';

	// teoricamente => mutex lock
	char* ipClient = strdup(inet_ntoa(p->client.sin_addr));
	// teoricamente => mutex unlock
	int portClient = ntohs(p->client.sin_port);
	printf("received from\t[%s:%d] \t'%s'\n", ipClient, portClient, buffer);

	char** data = (char**) malloc(sizeof(char**) * MAX_STR);
	data = split(buffer, " ");
	char* tmp = data[0];
	char* file = (char*) malloc(sizeof(char*) * MAX_STR);
	sprintf(file, "%s%s", FOLDER, tmp);
	int nEstrarre = atoi(data[1]);
	free(data);

	printf("now looking for the file '%s' ...\n", file);
	FILE* fp = fopen(file, "r");
	if (fp != NULL) {
		char msg[MAX_STR / nEstrarre];

		printf("reading file '%s' ...\n\n", file);

		int fileLen = 0;
		char* tmp = (char*) malloc(sizeof(char) * MAX_STR);
		while ( fgets(tmp, MAX_STR, fp) != NULL)
			fileLen++;
		free(tmp);

		if (nEstrarre <= fileLen) {
			srand(time(NULL));
			for (int i=0; i<nEstrarre; i++) {
				char* student = takeStudent(fp, i, nEstrarre, fileLen);
				invia(p->connId, student);
			}		
		} else invia(p->connId, FUORI_SCALA);
		
		fclose(fp);
	} else {
		printf("File not found!\n");
		invia(p->connId, FILE_NOT_FOUND);
	}

	printf("Closing connection ...\n\n===================================\n\n");
	shutdown(p->connId, SHUT_RDWR);
}


void invia(int connId, char* msg) {
	if ( send(connId, 
				msg, 
				strlen(msg),
				0) 
		!= strlen(msg) )
			errore("send()", -8);
	printf("'%s' sent!\n", msg);
}


// versione STATEFUL => vettore in cui salvo l'indice degli alunni estratti
char* takeStudent(FILE* fp, int indexDaEstrarre, int n, int fileLen) {

	rewind(fp); // riniziare a leggere dall'inizio del file

	static int alunniEstratti[MAX_STR];
	if (indexDaEstrarre == 0)
		for (int i=0; i<n; i++)
			alunniEstratti[i] = -1;
	
	int x;
	do {
		x = (rand() % fileLen) + 1; // x da 1 a N
	} while (!isUnique(x, alunniEstratti, n));
	alunniEstratti[indexDaEstrarre] = x;

	int rowIndex = 1;
	bool trovato = false;
	char student[MAX_STR / n];
	while ( !trovato && fgets(student, MAX_STR/n, fp) != NULL ) {
		trovato = (rowIndex == x)? true : false;
		rowIndex++;
	}
	student[(strlen(student) - 1)] = '\0';

	return strdup(student);
}


bool isUnique(int num, int array[], int n) {
	bool unique = true;
	int i = 0;
	while (unique && i<n) {
		if (num == array[i])
			unique = false;
		i++;
	}
	return unique;
}

