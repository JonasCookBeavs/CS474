#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(void)
{
    int fd = open("foo.txt", O_CREAT|O_TRUNC|O_WRONLY, 0666);
    lseek(fd, 400000000, SEEK_SET);
    write(fd, "a", 1);
    close(fd);
}