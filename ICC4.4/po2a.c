#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if(argc != 3){
        exit(1);
    }
    int offset = atoi(argv[2]);
    int page_number = atoi(argv[1]);

    int address = page_number << 8;
    address = address | offset;

    printf("Page %d, offset %d = Address %d\n", page_number, offset, address);
}