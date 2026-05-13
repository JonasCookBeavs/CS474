#include <stdio.h>

#define TEST_COUNT 16

// int parity(unsigned char b)
// {
//     int total = 0;
//     for(int i = 0; i < 8; i++){
//         if((b & 0b1) == 1) {
//             total++;
//         } 
//         b >>= 1;
//     }
//     return total % 2;
// }

int parity(unsigned char b)
{
    int parity = 0;
    for(int i = 0; i < 8; i++){
        parity ^= (b & 0b1);
        b >>= 1;
    }
    return parity;
}

int main(void)
{
    char *green = "\033[0;92m";  // Green
    char *red = "\033[0;30;101m";  // Black on red
    char *reset = "\033[0m";

    unsigned char test[TEST_COUNT] = {
        0x00, 0xff, 0x01, 0x80, 0xaa, 0x9a, 0x19, 0xaf,
        0x75, 0x2d, 0x87, 0x4f, 0x93, 0x49, 0xf4, 0x30
    };

    int results[TEST_COUNT] = {
        0, 0, 1, 1, 0, 0, 1, 0,
        1, 0, 0, 1, 0, 1, 1, 0
    };

    for (int i = 0; i < TEST_COUNT; i++) {
        int result = parity(test[i]);

        if (result == results[i])
            printf("%sPASS%s: %02x -> %d\n", green, reset, test[i], result);
        else
            printf("%sFAIL%s: %02x -> %d (expected %d)\n", red, reset, test[i],
                result, results[i]);
    }
}