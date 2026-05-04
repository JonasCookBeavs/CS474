all: main

.PHONY: clean all

clean:
	rm -f *.o

main.o: main.c adder.h
	gcc -Wall -Wextra -c $<

adder.o: adder.c
	gcc -Wall -Wextra -c $<

main: main.o adder.o
	gcc -o $@ $^