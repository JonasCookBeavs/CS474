#include <stdio.h>
#include <stdlib.h>

unsigned short read_u16(void *addr)
{
    unsigned char *bytes = addr;

    return (bytes[0] << 8) | (bytes[1] << 0);
}

struct reader {
    unsigned char *pos;
};

struct reader *reader_open(unsigned char *p)
{
    struct reader *r = malloc(sizeof *r);
    r->pos = p;

    return r;
}

void reader_close(struct reader *r)
{
    free(r);
}

/*
 * Every call to this should return the next 2-byte value in the array
 * we used with reader_open().
 */
unsigned int reader_get_next_u16(struct reader *r)
{
    int two_bytes = read_u16(r->pos);
    r->pos += 2;
    return two_bytes;
}

#define TEST_VALUES 5

int main(void) {
    struct reader *r;

    unsigned char data[(TEST_VALUES+1)*2] = {
        0x00, 0x01,
        0x00, 0x02,
        0x01, 0x00,
        0x34, 0x90,
        0x12, 0xEF,
        0x00, 0x00
    };

    unsigned int test_values[TEST_VALUES] =
        { 0x0001, 0x0002, 0x0100, 0x3490, 0x12EF };

    r = reader_open(data);

    int i = 0;
    unsigned int v;

    while ((v = reader_get_next_u16(r)) != 0) {
        printf("Test %d: %s\n", i, v == test_values[i]? "pass": "fail");
        i++;
    }

    reader_close(r);
}