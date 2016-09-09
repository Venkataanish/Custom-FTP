/* Creates a datagram Reciever.  The port
 number is passed as an argument.  This
 server runs forever */

#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include "FileUtil.h"
#include <pthread.h>
#include "RecieverThreads.h"

//global vars
char pkts[1000];
pthread_t udprecieve_thr, errorHandler_thr;

typedef struct input {
	int parts;
} Input;




int main(int argc, char *argv[]) {

	if (argc < 2) {
		fprintf(stderr, "ERROR, no port provided\n");
		exit(0);
	}

	pthread_create(&udprecieve_thr, 0, udp_recieve, argv[1]);
	usleep(5000000);
	pthread_create(&errorHandler_thr,0,sendErrorSeq,NULL);
    pthread_join(errorHandler_thr,NULL);
	pthread_join( udprecieve_thr, NULL);
	pthread_exit(0);

	return 0;
}
