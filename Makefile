CFLAGS = -g3 -O3 #-g3 -O0

all: main.o memsearch.o hexdump.o
	gcc $(CFLAGS) main.o memsearch.o hexdump.o -o main

main.o: main.c
	gcc $(CFLAGS) -c main.c

memsearch.o: memsearch.c
	gcc $(CFLAGS) -c memsearch.c

hexdump.o: hexdump.c
	gcc $(CFLAGS) -c hexdump.c

tests: memsearch.c
	gcc -O0 -g3 -DMAIN_TEST_FN=main memsearch.c -o tests

.PHONY: clean
clean:
	rm -f main.o memsearch.o hexdump.o main
