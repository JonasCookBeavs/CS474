packing: packing.o pack.o
	gcc -o $@ $^

pack.o: pack.c pack.h
	gcc -Wall -Wextra -c $<

packing.o: packing.c
	gcc -Wall -Wextra -c $<

