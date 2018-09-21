HEADERS = bt.h

default: main

bt.o: bt.c $(HEADERS)
	gcc -c bt.c -o bt.o

main: bt.o main.c
	gcc main.c bt.o -o main

clean:
	rm -f main
	rm -f bt.o
