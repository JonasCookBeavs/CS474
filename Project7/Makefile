all: testfs

libvvsfs.a: image.o block.o free.o inode.o mkfs.o
	ar rcs libvvsfs.a $^

testfs: testfs.o libvvsfs.a
	gcc -Wall -Wextra -Werror -o testfs testfs.o libvvsfs.a

%.o: %.c
	gcc -Wall -Wextra -Werror -c $<

test: testfs
	./testfs

clean:
	rm -f *.o

pristine: clean
	rm -f testfs *.a

.PHONY: all test clean pristine