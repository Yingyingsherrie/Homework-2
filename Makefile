CC=gcc
CFLAGS=-g

.PHONY: all
all: chat

.PHONY: test
test: chat
	@./test.sh

chat: chat.o
	gcc -o chat chat.o -lpthread

chat.o: chat.c

.PHONY: clean
clean:
	rm -f *.o chat my_* >/dev/null ?2?
