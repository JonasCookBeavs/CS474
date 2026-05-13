#include <stdio.h>
#include <string.h>

#define TEST_COUNT 10

int parity(unsigned char b)
{
    int parity = 0;
    for(int i = 0; i < 7; i++){
        parity ^= (b & 0b1);
        b >>= 1;
    }
    return parity;
}

/**
 * Repair the three bytes of data.
 *
 * Return 1 if the data is correct or has been corrected.
 * Return 0 if the data cannot be corrected.
 */
int repair(unsigned char *data) 
{
    unsigned char a = data[0];
    unsigned char b = data[1];
    unsigned char c = data[2];

    // Corruption variables
    int a_corr = 0;
    int b_corr = 0;
    int c_corr = 0;

    // Compute parity bits 
    int parity_bit_a = a >> 7;
    a &= ~(0b1 >> 7);

    int parity_bit_b = b >> 7;
    b &= ~(0b1 >> 7);

    int parity_bit_c = c >> 7;
    c &= ~(0b1 >> 7);

    // Check if bytes are corrupted
    if(parity_bit_a != parity(a)){
        a_corr = 1;
    }
    if(parity_bit_b != parity(b)){
        b_corr = 1;
    }
    if(parity_bit_c != parity(c)){
        c_corr = 1;
    }

    // Check if all data is good
    if(a_corr == 0 && b_corr == 0 && c_corr == 0){
        return 1;
    }

    // Check if more than one byte is corrupted
    if((a_corr == 1 && b_corr == 1) || (a_corr == 1 && c_corr == 1) || (b_corr == 1 && c_corr == 1)) {
        return 0;
    }

    // Repair bytes
    if(a_corr == 1){
        a = b ^ c;
        parity_bit_a = parity(a);
        a &= ~(parity_bit_a >> 7);
    }

    if(b_corr == 1){
        b = a ^ c;
        parity_bit_b = parity(b);
        b &= ~(parity_bit_b >> 7);
    }

    if(c_corr == 1){
        c = a ^ b;
        parity_bit_c = parity(c);
        c &= ~(parity_bit_c >> 7);
    }

    // Put bytes back into data array
    data[0] = a;
    data[1] = b;
    data[2] = c;
    return 1;
}

int main(void)
{
    char *green = "\033[0;92m";  // Green
    char *red = "\033[0;30;101m";  // Black on red
    char *reset = "\033[0m";

    unsigned char trips[TEST_COUNT][3] = {
        {0x26, 0xd9, 0xec},
        {0xcc, 0x71, 0xbd},
        {0xbb, 0xdb, 0x60},
        {0x5e, 0xb0, 0x7e},
        {0x72, 0x04, 0x56},
        {0x8e, 0x10, 0x1e},
        {0xde, 0xd7, 0x09},
        {0x47, 0x66, 0x29},
        {0x05, 0xe2, 0xef},
        {0x29, 0x1e, 0x33}
    };

    unsigned char solutions[TEST_COUNT][4] = {
        {0x24, 0xd8, 0xfc, 0x00},
        {0xcc, 0x71, 0xbd, 0x01},
        {0xbb, 0xdb, 0x60, 0x01},
        {0xde, 0xa0, 0x7e, 0x00},
        {0x72, 0x24, 0x56, 0x01},
        {0x8e, 0x90, 0x1e, 0x01},
        {0xde, 0xd7, 0x09, 0x01},
        {0x47, 0x66, 0x21, 0x01},
        {0x05, 0xe2, 0xe7, 0x01},
        {0x2d, 0x1e, 0x33, 0x01}
    };

    unsigned char temp[3];

    for (int i = 0; i < TEST_COUNT; i++) {
        memcpy(temp, trips[i], sizeof temp);

        int result = repair(temp);
        int expected_result = solutions[i][3];

        if (result != expected_result) {
            printf("%sFAIL%s: [%02x,%02x,%02x] returned %d (expected %d)\n",
                red, reset,
                trips[i][0], trips[i][1], trips[i][2],
                result, expected_result);
            continue;
        }

        if (result == 1) { // corrected it
            int bad_count = 0;

            for (int j = 0; j < 3; j++)
                if (temp[j] != solutions[i][j])
                    bad_count++;

            if (bad_count > 0) {
                printf("%sFAIL%s: [%02x,%02x,%02x] -> [%02x,%02x,%02x] "
                    "(expected [%02x,%02x,%02x])\n", red, reset,
                    trips[i][0], trips[i][1], trips[i][2],
                    temp[0], temp[1], temp[2],
                    solutions[i][0], solutions[i][1], solutions[i][2]);
                continue;
            }

            printf("%sPASS%s: [%02x,%02x,%02x] -> [%02x,%02x,%02x]\n",
                green, reset,
                trips[i][0], trips[i][1], trips[i][2],
                solutions[i][0], solutions[i][1], solutions[i][2]);
        } else {
            // Could not correct it
            printf("%sPASS%s: [%02x,%02x,%02x] could not be corrected\n",
                green, reset,
                trips[i][0], trips[i][1], trips[i][2]);
        }
    }
}