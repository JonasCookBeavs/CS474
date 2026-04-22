#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if(argc != 2){
        exit(1);
    }
    int address = atoi(argv[1]);
    
    int page_number = address >> 8;
    int offset = address & 0b11111111;

    printf("Address %d = Page %d, offset %d\n", address, page_number, offset);
}