#ifndef _RECIEVERTHREADS_H_
#define _RECIEVERTHREADS_H_


#define port_sendfromreceiver 1670
#define port_receivefromsender 1679
#define NUMPACKETS 10

int PACKETS[NUMPACKETS];

void *udp_recieve(void * argv);
void *sendErrorSeq(void *arg);

#endif