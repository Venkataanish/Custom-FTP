// UDP client in the internet domain

#include <sys/types.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <arpa/inet.h>

#include <netdb.h>

#include <stdio.h>

#include <stdlib.h>

#include <unistd.h>

#include <string.h>

#include <pthread.h>

#include "FileUtil.h"
#define port_sendfromreceiver 1670
#define port_receivefromsender 1679
#define port_TCPControl 1677

int NUMPACKETS;

char *buffer;

pthread_t send_thr, resend_thr, tcp_thr, control_thr;

typedef struct input {

	struct hostent *hostname;

	int port;
	FILE *fp;

} Input;

void *tcp_control(void * argv) {
	Input *inp = (Input*) argv;
	printf("Started Control on Sender side\n");
	int sockfd, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	char buffer[20];
	printf("Packkets ==== %d", NUMPACKETS);
	sprintf(buffer, "%d", NUMPACKETS);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");
	server = inp->hostname;
	if (server == NULL) {
		fprintf(stderr, "ERROR, no such host\n");
		exit(0);
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *) server->h_addr,
	(char *)&serv_addr.sin_addr.s_addr,
	server->h_length);
	serv_addr.sin_port = htons(port_TCPControl);
	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR connecting");

	n = write(sockfd, buffer, strlen(buffer));
	if (n < 0)
		error("ERROR writing to socket");

	printf("Sent the control message  Packets = %s \n ", buffer);

	close(sockfd);
	return NULL;
}

void *udp_send(void * argv) {

	Input *inp = (Input*) argv;

	int sock, count;

	unsigned int length;

	struct sockaddr_in server;

	struct hostent *hp;

	Message *newmsg;

	sock = socket(AF_INET, SOCK_DGRAM, 0);

	if (sock < 0) {

		error("socket");

	}

	server.sin_family = AF_INET;

	hp = inp->hostname;

	if (hp == 0) {

		error("Unknown host");

	}

	bcopy((char *) hp->h_addr,

	(char *)&server.sin_addr,

	hp->h_length);

	server.sin_port = htons(inp->port);

	length = sizeof(struct sockaddr_in);

//file handling

	FILE *fp = inp->fp;
//send file sequentially

	for (count = 0; count < NUMPACKETS; count++) {

		newmsg = getNext(fp, count);

		if (count % 2 == 0) {
			printf("Sending msg  = %s , seq = %d\n", newmsg->info, newmsg->seq);

			usleep(1000);
			sendto(sock, newmsg, sizeof(Message), 0,

			(const struct sockaddr *) &server, length);

		}

	}

	close(sock);

	return NULL;

}

void *tcp_receive(void *argv) {
	char *allones = (char*) malloc(sizeof(NUMPACKETS));
	memset(allones, '1', NUMPACKETS);
	printf("Started tcp RECIEVE \n");
	int sockfd, newsockfd;

	socklen_t clilen;

	//char buffer[NUMPACKETS];

	//int *resend_seq;

	struct sockaddr_in serv_addr, cli_addr;

	int n;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0)

		error("ERROR opening socket");

	bzero((char *) &serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;

	serv_addr.sin_addr.s_addr = INADDR_ANY;

	serv_addr.sin_port = htons(port_sendfromreceiver);

	if (bind(sockfd, (struct sockaddr *) &serv_addr,

	sizeof(serv_addr)) < 0)

		error("ERROR on binding");

	listen(sockfd, 5);

	clilen = sizeof(cli_addr);

	newsockfd = accept(sockfd,

	(struct sockaddr *) &cli_addr,

	&clilen);

	if (newsockfd < 0)

		error("ERROR on accept");

	//bzero(buffer, NUMPACKETS);
	printf("Current retranmit seq= %s, cmp = %d", buffer, strcmp(buffer, allones));
	while (strcmp(buffer, allones)) {

		n = read(newsockfd, buffer, NUMPACKETS);
		//buffer[NUMPACKETS] = '\0';

		printf("Received retransmit request:%s\n", buffer);

		if (n < 0)
			error("ERROR reading from socket");
	}
	close(newsockfd);

	close(sockfd);
pthread_cancel(resend_thr);
	return NULL;

}

void *udp_resend(void * argv) {
	char *allones = (char*) malloc(sizeof(NUMPACKETS));
	memset(allones, '1', NUMPACKETS);

	printf("Start retransmit\n");

	Input *inp = (Input*) argv;

	//char *buffer = (char *) argv;

	int sock, count;

	unsigned int length;

	struct sockaddr_in server;

	struct hostent *hp;

	Message *newmsg;

	sock = socket(AF_INET, SOCK_DGRAM, 0);

	if (sock < 0) {

		error("socket");

	}

	server.sin_family = AF_INET;

	hp = inp->hostname;

	if (hp == 0) {

		error("Unknown host");

	}

	bcopy((char *) hp->h_addr,

	(char *)&server.sin_addr,

	hp->h_length);

	server.sin_port = htons(inp->port);

	length = sizeof(struct sockaddr_in);

//file handling

//int seq;

	FILE *fp;

	initFilePtr("x.txt", &fp, "r");

//send file sequentially
	while (strcmp(buffer, allones)) {
		printf("%s\n", buffer);
		for (count = 0; count < NUMPACKETS; count++) {
//printf("Inisde for loop\n");
			if (buffer[count] == '0') {

				newmsg = getChunk(fp, count);

				printf("RESending msg  = %s , seq = %d\n", newmsg->info,
						newmsg->seq);

//usleep(1000);

				sendto(sock, newmsg, sizeof(Message), 0,

				(const struct sockaddr *) &server, length);

				free(newmsg);

			}
		}
	}
	close(sock);
	pthread_cancel(send_thr);
	return NULL;
}

int main(int argc, char *argv[]) {

	if (argc != 3) {

		printf("Usage: server port\n");

		exit(1);

	}

	FILE *fp;

	int size = initFilePtr("x.txt", &fp, "r");

	NUMPACKETS = getParts(size);

	//setiing retransmission seq buffer to all '0'
	buffer = (char *) malloc(NUMPACKETS);
	memset(buffer, '0', NUMPACKETS);

	Input *inp = malloc(sizeof(Input));

	inp->hostname   =  gethostbyname("localhost");

	inp->port = atoi(argv[2]);
	inp->fp = fp;

	pthread_create(&control_thr, 0, tcp_control, inp);
	pthread_join(control_thr, NULL);


	printf("Creating udp send thr \n");
	pthread_create(&send_thr, 0, udp_send, inp);

	pthread_create(&tcp_thr, 0, tcp_receive, NULL);

	pthread_create(&resend_thr, 0, udp_resend, inp);

	pthread_join(send_thr, NULL);

	pthread_join(tcp_thr, NULL);

	pthread_join(resend_thr, NULL);

	pthread_exit(0);

	return 0;

}
