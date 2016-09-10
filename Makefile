all: Sender.o Reciever.o 

Sender.o: Sender.c FileUtil.c FileUtil.h
	gcc -Wall -g -o Sender Sender.c FileUtil.c -lpthread -lm


Reciever.o: Reciever.c FileUtil.c FileUtil.h RecieverThreads.c RecieverThreads.h
	gcc -Wall -g -o Reciever Reciever.c FileUtil.c RecieverThreads.c -lpthread -lm

clean:
	rm -f *.o Sender Reciever
