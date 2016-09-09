#include <stdio.h>

#include <stdlib.h>


#ifndef _FILEUTIL_H_
#define _FILEUTIL_H_


#define CHUNKSIZE  7

typedef struct message {
	int seq;
	char info[CHUNKSIZE];
} Message;

int roundVal(double num);
int seek(FILE *fp, int seq) ;
int writeChunk(FILE *fp, Message *msg, int seq);
Message* getChunk(FILE *fp, int seq);
Message* getNext(FILE *fp, int seq);
int initFilePtr(char *path, FILE **fp, char *mode);
int getParts(int size);
void error(const char *msg);

#endif
