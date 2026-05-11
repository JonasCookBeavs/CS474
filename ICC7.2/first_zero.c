#include <stdio.h>

#define TEST_COUNT 6
#define DATA_LEN 4
#define BITS_PER_BYTE 8

int find_first_zero_bit(int *a)
{
    for(int i = 0; i < DATA_LEN; i++)
    {
        if(a[i] == 0xFF) { continue; }
        for(int j = 0; j < BITS_PER_BYTE; j++){
            if(((a[i] >> j) & 0b1) == 0){
                return i * BITS_PER_BYTE + j;
            } 
        }
    }

    return -1;
}

int main(void)
{
    int a[TEST_COUNT][DATA_LEN] = {
        { 0xFF, 0xFF, 0xEF, 0xFF },
        { 0xFF, 0x0F, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0x7F },
        { 0xFF, 0xFF, 0xF7, 0xFF },
        { 0x70, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF },
    };

    int results[TEST_COUNT] = {
        20, 12, 31, 19, 0, -1
    };

    for (int i = 0; i < TEST_COUNT; i++) {
        int fz = find_first_zero_bit(a[i]);
        printf("Test %d: %s\n", i, fz == results[i]? "pass": "FAIL");
    }
}