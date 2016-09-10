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
#include <pthread.h>
void *TCP_Control(void *arg) {
	printf("Strated Control\n");
	int sockfd, newsockfd, n;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	char s[20];

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");

	bzero((char *) &serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port_TCPControl);
//
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR on binding");

	listen(sockfd, 5);
	clilen = sizeof(cli_addr);

	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

	if (newsockfd < 0)
		error("ERROR on accept");

	n = read(newsockfd, s, 20);

	NUMPACKETS = atoi(s);
	printf("Got The Number of Packetes from Sender :%d\n", NUMPACKETS);

	if (n < 0)
		error("ERROR reading from socket");

	close(newsockfd);

	return NULL;
}

void *sendErrorSeq(void *arg) {
	printf("Inside sendErrorSeq\n");
	char *allones = (char*) malloc(sizeof(NUMPACKETS));
	memset(allones, '1', NUMPACKETS);
	int sockfd, n;
	//int sockUDP;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
		error("ERROR opening socket");
	}
	printf("Created socket Inside sendErrorSeq\n");
	//TODO: add struct
	server = (struct hostent *) arg;
	printf("aaaaaaaaaaaaaaaaaaaaaget hi=ost socket Inside sendErrorSeq\n");
	if (server == NULL) {
		fprintf(stderr, "ERROR, no such host\n");
		exit(0);
	}
	printf("before bzero Inside sendErrorSeq\n");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	printf("after bzero Inside sendErrorSeq\n");
	serv_addr.sin_family = AF_INET;
	bcopy((char *) server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	printf("AFTER bCOPY Inside sendErrorSeq\n");
	serv_addr.sin_port = htons(port_sendfromreceiver);
	printf("before connect Inside sendErrorSeq\n");
	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))
			< 0) {
	//	if(udpRecieveEnd){
		//	pthread_exit(0);
	//	}
		error("Error connect:");
	}
	printf("Resend Packets = %s\n", PACKETS);
	printf("all ones  = %s , %d\n ", allones, strcmp(PACKETS, allones));

	while (strcmp(PACKETS, allones)!=0) {

	//	if (udpRecieveEnd) {
		//	break;
	//	}
		//printf("Packets = %s\n", PACKETS);
		n = send(sockfd, PACKETS, NUMPACKETS, 0);

		if (n < 0) {
			error("ERROR writing to socket");
		}
	}
	printf("before sending allones sendErrseq\n");
	n = send(sockfd, allones, NUMPACKETS, 0);
	if (n < 0) {
		error("ERROR writing to socket");
	}
	printf("after sending allones sendErrseq\n");
	printf("Packets = %s\n", PACKETS);
	close(sockfd);

	return NULL;
}

void *udp_recieve(void * argv) {
	printf("started udp reciever\n");
	PACKETS = (char*) malloc(NUMPACKETS);
	memset(PACKETS, '0', NUMPACKETS);
	int sock, length, n, i;
	FILE *fp;

	initFilePtr("y.txt", &fp, "w");
	printf("after init file ptr udp recieve\n");

	Message *temp = (Message*) malloc(sizeof(Message));
	socklen_t fromlen;
	struct sockaddr_in server;
	struct sockaddr_in from;

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		error("Opening socket");
	}
	printf("after sock create ptr udp recieve\n");
	length = sizeof(server);
	bzero(&server, length);
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(atoi((char *) (argv)));
	printf("before bind udp recieve\n");
	if (bind(sock, (struct sockaddr *) &server, length) < 0) {
		error("binding");
	}

	fromlen = sizeof(struct sockaddr_in);
	printf("NUMPACKETS = %d", NUMPACKETS);
	printf("before for ptr udp recieve\n");
	for (i = 0; i < NUMPACKETS; i++) {
		printf("inside for udp recieve\n");
		n = recvfrom(sock, temp, sizeof(Message), 0, (struct sockaddr *) &from,
				&fromlen);
		if (n < 0) {
			error("recvfrom");
		}
		PACKETS[temp->seq] = '1';
		printf(
				"Received seq: %d, received message: %s, seqbuff = %s , i value = %d\n",
				temp->seq, temp->info, PACKETS, i);
		writeChunk(fp, temp, temp->seq);
	}

	close(sock);
//	udpRecieveEnd =1;
	printf("UDP Recieve end\n");
//	pthread_cancel(errorHandler_thr);
	return NULL;
}
