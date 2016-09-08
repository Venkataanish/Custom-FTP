#include "FileUtil.h"
#include <stdio.h>

#include <stdlib.h>

int main() {

	FILE *fp, *rfp, *wfp;

	int i = 0;
	int size = initFilePtr("x.txt", &fp, "r");
	initFilePtr("x.txt", &rfp, "r");

	initFilePtr("z.txt", &wfp, "wa");

	double temp = (double) size / CHUNKSIZE;

	int PARTS = roundVal(temp);
	printf("Parts = %d Size = %d\n", PARTS, size);

	for (i = 0; i < PARTS; i++) {

		Message *msg = getNext(fp, i);

		printf("msg = %s, seq=  %d\n", msg->info, msg->seq);
	}

	int x[] = { 1, 2, 3, 6, 7, 5, 4, 9, 11, 8, 10, 12, 13, 14, 17, 15, 16, 18,
			19, 21, 20, 22, 23, 24, 0, 25, 26 };
	fseek(fp, 0, SEEK_SET);
	seek(fp, 0);

	for (i = 0; i < 27; i++) {

		Message *msg = getChunk(fp, x[i]);

		printf("msg = %s, seq=  %d\n", msg->info, msg->seq);

		if (writeChunk(wfp, msg, x[i]) < 0) {
			fprintf(stderr, "Error in writing!\n");
		}

	}

	fclose(fp);
	fclose(wfp);
	return 0;

}
