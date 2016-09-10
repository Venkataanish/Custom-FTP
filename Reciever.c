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





int main(int argc, char *argv[]) {

	if (argc < 3) {
		fprintf(stderr, "USage: port host \n");
		exit(0);
	}
	struct hostent *server =  gethostbyname(argv[2]);
	pthread_create(&TCPcontrol_thr,0,TCP_Control,NULL);
	pthread_join( TCPcontrol_thr, NULL);
	pthread_create(&udprecieve_thr, 0, udp_recieve, argv[1]);

	pthread_create(&errorHandler_thr,0,sendErrorSeq,server);

    pthread_join(errorHandler_thr,NULL);
	pthread_join( udprecieve_thr, NULL);
	pthread_exit(0);

	return 0;
}
