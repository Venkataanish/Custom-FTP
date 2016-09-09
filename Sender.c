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

pthread_t send_thr, resend_thr;

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
	server.sin_port = htons( inp->port);
	length = sizeof(struct sockaddr_in);

	//file handling
	int parts;
	FILE *fp;
	int size = initFilePtr("x.txt", &fp, "r");
	parts = getParts(size);

	//send file sequentially
	for (count = 0; count < parts; count++) {
		newmsg = getNext(fp, count);
		printf("Sending msg  = %s , seq = %d\n", newmsg->info,newmsg->seq);
		usleep(1000);
		sendto(sock,  newmsg, sizeof(Message), 0,
				(const struct sockaddr *) &server, length);

	}
	close(sock);

	return NULL;
}

int main(int argc, char *argv[]) {

	if (argc != 3) {
		printf("Usage: server port\n");
		exit(1);
	}

	Input *inp = malloc(sizeof(Input));
	inp->hostname = argv[1];
	inp->port = atoi(argv[2]);
	pthread_create(&send_thr, 0, udp_send, inp);
	pthread_join( send_thr, NULL);

	pthread_exit(0);
	return 0;
}

