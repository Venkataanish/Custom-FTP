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
#include "RecieverThreads.h"

char buffer[NUMPACKETS];

pthread_t send_thr, resend_thr, tcp_thr;
;

typedef struct input {

	char *hostname;

	int port;

} Input;

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

	hp = gethostbyname(inp->hostname);

	if (hp == 0) {

		error("Unknown host");

	}

	bcopy((char *) hp->h_addr,

	(char *)&server.sin_addr,

	hp->h_length);

	server.sin_port = htons(inp->port);

	length = sizeof(struct sockaddr_in);

//file handling

	int parts;

	FILE *fp;

	int size = initFilePtr("x.txt", &fp, "r");

	parts = getParts(size);

//send file sequentially

	for (count = 0; count < parts; count++) {

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
	while (strcmp(buffer, "111111111111111")) {

		n = read(newsockfd, buffer, NUMPACKETS);
		buffer[NUMPACKETS] = '\0';

		printf("Received retransmit request:%s\n", buffer);

		if (n < 0)
			error("ERROR reading from socket");
	}
	//close(newsockfd);

	//close(sockfd);

	return NULL;

}

void *udp_resend(void * argv) {
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

	hp = gethostbyname(inp->hostname);

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
	while (strcmp(buffer, "111111111111111")) {
		printf("%s\n", buffer);
		for (count = 0; count < NUMPACKETS; count++) {
//printf("Inisde for loop\n");
			if (buffer[count] == '0') {

				newmsg = getChunk(fp, count);

//printf("REEEESending msg  = %s , seq = %d\n", newmsg->info,newmsg->seq);

//usleep(1000);

				sendto(sock, newmsg, sizeof(Message), 0,

				(const struct sockaddr *) &server, length);

			}
		}
	}
	close(sock);

	return NULL;
}

int main(int argc, char *argv[]) {

	if (argc != 3) {

		printf("Usage: server port\n");

		exit(1);

	}

	memset(buffer, 0, sizeof(buffer));

	Input *inp = malloc(sizeof(Input));

	inp->hostname = argv[1];

	inp->port = atoi(argv[2]);

	pthread_create(&send_thr, 0, udp_send, inp);

	pthread_create(&tcp_thr, 0, tcp_receive, NULL);

	pthread_create(&resend_thr, 0, udp_resend, inp);

	pthread_join(send_thr, NULL);

	pthread_join(tcp_thr, NULL);

	pthread_join(resend_thr, NULL);

	pthread_exit(0);

	return 0;

}
