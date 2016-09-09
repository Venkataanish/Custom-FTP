#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include "FileUtil.h"
#include "RecieverThreads.h"

void *sendErrorSeq(void *arg) {

	int sockfd, n;
	int sockUDP;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	int buffer[10];
	int j;

	for (j = 0; j < 10; j++) {

		if (j % 2 == 0) {
			buffer[j] = 1;
		} else {
			buffer[j] = 0;
		}
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
		error("ERROR opening socket");
	}
	sockUDP = socket(AF_INET, SOCK_DGRAM, 0);

	if (sockUDP < 0) {
		error("ERROR opening socket sockUDP");
	}
	//TODO: add struct
	server = gethostbyname("localhost");

	if (server == NULL) {
		fprintf(stderr, "ERROR, no such host\n");
		exit(0);
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *) server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(port_sendfromreceiver);
	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))
			< 0) {
		error("ERROR connecting");
	}

	n = send(sockfd, (char *) buffer, sizeof(buffer), 0);
	if (n < 0) {
		error("ERROR writing to socket");
	}

	close(sockfd);

	return NULL;
}



void *udp_recieve(void * argv) {

	int sock, length, n, i, parts=5;
	Message *temp = (Message*)malloc(sizeof(Message));
	socklen_t fromlen;
	struct sockaddr_in server;
	struct sockaddr_in from;

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		error("Opening socket");
	}

	length = sizeof(server);
	bzero(&server, length);
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(atoi((char *) (argv)));

	if (bind(sock, (struct sockaddr *) &server, length) < 0) {
		error("binding");
	}

	fromlen = sizeof(struct sockaddr_in);



	for (i = 0; i < parts; i++) {


		n = recvfrom(sock, temp, sizeof(Message), 0, (struct sockaddr *) &from,
				&fromlen);
		if (n < 0) {
			error("recvfrom");
		}
		PACKETS[temp->seq] = 1;
		printf("Received seq: %d, received message: %s\n", temp->seq,
				temp->info);
	}

	close(sock);

	return NULL;
}
