#include <stdio.h>

#define BITS_PER_BYTE 8

unsigned char bitmap[4096];

void print_byte(unsigned char b)
{
    if (b <= 1)
        printf("%d", b);

    else {
        print_byte(b >> 1);
        printf("%d", b & 1);
    }
}

void print_byte_n(unsigned char b)
{
    print_byte(b);
    putchar('\n');
}

int get_bit(int index)
{
    int byte = index / 8;
    int offset = index % 8;

    return (bitmap[byte] >> offset) & 0b1;
}

void set_bit(int index, int value)
{
    int byte = index / 8;
    int offset = index % 8;
    if(value == 1){
        int value_byte = 0b1 << offset;
        
        bitmap[byte] = bitmap[byte] | value_byte;
    }
    else{
        int value_byte = 0b1 << offset;
        value_byte = ~(value_byte);
        
        bitmap[byte] = bitmap[byte] & value_byte;
    }
}

int main(void)
{
    set_bit(0,1);
    set_bit(2,1);
    set_bit(3,1);
    set_bit(4,1);
    set_bit(7,1);
    set_bit(8,1);

    print_byte_n(bitmap[0]);
    print_byte_n(bitmap[1]);

    for (int i = 0; i <= 9; i++) {
        printf("%d: %d\n", i, get_bit(i));
    }
}