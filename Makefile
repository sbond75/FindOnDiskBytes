CFLAGS = -g3 -O0

all: main.o memsearch.o
	gcc $(CFLAGS) main.o memsearch.o -o main

main.o: main.c
	gcc $(CFLAGS) -c main.c

memsearch.o: memsearch.c
	gcc $(CFLAGS) -c memsearch.c

tests: memsearch.c
	gcc -DMAIN_TEST_FN=main memsearch.c -o tests

.PHONY: clean
clean:
	rm -f main.o memsearch.o main
