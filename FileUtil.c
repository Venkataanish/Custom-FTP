#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include "FileUtil.h"

void error(const char *msg) {
	perror(msg);
	exit(0);
}

int roundVal(double num) {
	int int_num = (int) num;
	if (num == (double) int_num) {
		return int_num;
	}
	return int_num + 1;
}

int seek(FILE *fp, int seq) {
	int pos = seq * CHUNKSIZE <= 0 ? 0 : seq * CHUNKSIZE;
	/*if (feof(fp)) {
	 fprintf(stderr, "End of file reached\n");
	 return -1;
	 }*/
	if (fseek(fp, pos, SEEK_SET) < 0) {

		fprintf(stderr, "Error in seek. position req: %d\n", pos);
		exit(0);
	}

	return ftell(fp);
}

int writeChunk(FILE *fp, Message *msg, int seq) {
	int ret_val = -1, pos;
	if (msg) {

		char *buff = msg->info;
		int size = strlen(buff);
		if ((pos = seek(fp, seq)) >= 0) {
			if (fwrite(buff, 1, size, fp) != size) {
				fprintf(stderr, "Less number of bytes written to file!\n");
			} else {
				//free(msg);
				ret_val = 1;

			}
		} else {
			fprintf(stderr, "ERROR in writechunk\n");
		}
	} else {
		fprintf(stderr, "Message in null in write chunk\n\n");
	}

	return ret_val;
}

Message* getChunk(FILE *fp, int seq) {
	Message *msg = (Message*) malloc(sizeof(Message));
	int pos, bytes_read, i = 0;
	if (msg) {
		if ((pos = seek(fp, seq)) >= 0) {

			char buff[CHUNKSIZE];
			if ((bytes_read = fread(buff, 1, CHUNKSIZE, fp)) > 0) {


				for (i = 0; i < bytes_read; i++) {
					msg->info[i] = buff[i];
				}
				msg->info[bytes_read] = '\0';

				msg->seq = seq;
			} else if (ferror(fp)) {
				fprintf(stderr, "End of file reached\n");

				return NULL;
			} else {
				fprintf(stderr, "Error in get chunk\n");
			}

		} else {
			fprintf(stderr, "ERROR in getChunk!!\n");
			return NULL;
		}
	} else {
		fprintf(stderr, "malloc failed in getChunk!\n");

	}

	return msg;
}

Message* getNext(FILE *fp, int seq) {
	Message *msg = (Message*) malloc(sizeof(Message));
	char buff[CHUNKSIZE];
	int bytes_read, i;
	if (msg) {
		if ((bytes_read = fread(buff, 1, CHUNKSIZE, fp)) > 0) {
			msg->seq = seq;


			for (i = 0; i < bytes_read; i++) {
				msg->info[i] = buff[i];
			}
			msg->info[bytes_read] = '\0';
		} else if (feof(fp)) {
			fprintf(stderr, "End of file reached\n");
			return NULL;
		} else {
			fprintf(stderr, "Error in getNext\n");
		}
	}
	return msg;
}

int initFilePtr(char *path, FILE **fp, char *mode) {
	int size = -1;

	struct stat status;
	if (!stat(path, &status)) {
		if (S_ISDIR(status.st_mode)) {
			fprintf(stderr,
					"%s is a directory. Please provide a valid filepath.\n",
					path);
			exit(1);
		}
	} else {
		printf("cannot open file\n");
	}

	*fp = fopen(path, mode);
	if (*fp == NULL) {
		fprintf(stderr, "Error opening the file %s. ", path);
		perror("");
		exit(1);
	} else {
		size = status.st_size;
	}

	return size;
}


int getParts(int size ){

	double temp = (double) size / CHUNKSIZE;

		int parts = roundVal(temp);

		return parts;
}
