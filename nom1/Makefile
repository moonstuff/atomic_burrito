CFLAGS += -Wall -g
CC = clang
LIBS = -lkyotocabinet 

all: nom1

nom1: *.c *.h
	$(CC) $(CFLAGS) -o $@ *.c $(LIBS)

clean:
	rm -f *.o nom1
