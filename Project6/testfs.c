#define CTEST_ENABLE
#include "ctest.h"
#include <string.h>
#include "image.h"
#include "block.h"

#define BLOCK_SIZE 4096

void test_image_open(void)
{
    int fd = image_open("test.img", 1);
    CTEST_ASSERT(fd >= 0, "image_open should return a valid fd");
}

void test_image_close(void)
{
    int result = image_close();
    CTEST_ASSERT(result == 0, "image_close should return 0 on success");
}

void test_bread_bwrite(void)
{
    unsigned char write_buf[BLOCK_SIZE];
    unsigned char read_buf[BLOCK_SIZE];

    memset(write_buf, 0xAB, BLOCK_SIZE);

    image_open("test.img", 1);
    bwrite(0, write_buf);
    bread(0, read_buf);
    image_close();

    CTEST_ASSERT(memcmp(write_buf, read_buf, BLOCK_SIZE) == 0, "bread should return what bwrite wrote");
}

void test_block_isolation(void)
{
    unsigned char buf0[BLOCK_SIZE];
    unsigned char buf1[BLOCK_SIZE];
    unsigned char read_buf[BLOCK_SIZE];

    memset(buf0, 0x11, BLOCK_SIZE);
    memset(buf1, 0x22, BLOCK_SIZE);

    image_open("test.img", 1);
    bwrite(0, buf0);
    bwrite(1, buf1);
    bread(0, read_buf);
    image_close();

    CTEST_ASSERT(memcmp(buf0, read_buf, BLOCK_SIZE) == 0, "writing block 1 should not corrupt block 0");
}

int main(void)
{
    CTEST_VERBOSE(1);

    test_image_open();
    test_image_close();
    test_bread_bwrite();
    test_block_isolation();

    CTEST_RESULTS();
    CTEST_EXIT();
}