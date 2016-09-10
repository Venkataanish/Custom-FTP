#ifndef _RECIEVERTHREADS_H_
#define _RECIEVERTHREADS_H_


#define port_sendfromreceiver 1670
#define port_receivefromsender 1679
#define port_TCPControl 1677

pthread_t udprecieve_thr, errorHandler_thr,TCPcontrol_thr;


int NUMPACKETS;
//int udpRecieveEnd;
char *PACKETS;

void *TCP_Control(void *arg);
void *udp_recieve(void * argv);
void *sendErrorSeq(void *arg);

#endif
