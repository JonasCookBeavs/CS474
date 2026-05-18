#define CTEST_ENABLE
#include "ctest.h"
#include <string.h>
#include "image.h"
#include "block.h"
#include "free.h"
#include "inode.h"
#include "mkfs.h"

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

// --- free.c tests ---

void test_set_free_marks_used(void)
{
    unsigned char block[BLOCK_SIZE];
    memset(block, 0, BLOCK_SIZE);

    set_free(block, 0, 1);
    CTEST_ASSERT(block[0] == 0x01, "bit 0 should be set");

    set_free(block, 7, 1);
    CTEST_ASSERT(block[0] == 0x81, "bit 7 should be set");
}

void test_set_free_marks_clear(void)
{
    unsigned char block[BLOCK_SIZE];
    memset(block, 0xFF, BLOCK_SIZE);

    set_free(block, 0, 0);
    CTEST_ASSERT(block[0] == 0xFE, "bit 0 should be cleared");
}

void test_find_free_empty_block(void)
{
    unsigned char block[BLOCK_SIZE];
    memset(block, 0, BLOCK_SIZE);

    int result = find_free(block);
    CTEST_ASSERT(result == 0, "first free bit in empty block should be 0");
}

void test_find_free_after_set(void)
{
    unsigned char block[BLOCK_SIZE];
    memset(block, 0, BLOCK_SIZE);

    set_free(block, 0, 1);
    int result = find_free(block);
    CTEST_ASSERT(result == 1, "first free bit after setting bit 0 should be 1");
}

void test_find_free_full_block(void)
{
    unsigned char block[BLOCK_SIZE];
    memset(block, 0xFF, BLOCK_SIZE);

    int result = find_free(block);
    CTEST_ASSERT(result == -1, "find_free on full block should return -1");
}

// --- ialloc() tests ---

void test_ialloc_returns_valid(void)
{
    image_open("test.img", 1);
    mkfs();

    int inum = ialloc();
    CTEST_ASSERT(inum >= 0, "ialloc should return a valid inode number");
    image_close();
}

void test_ialloc_increments(void)
{
    image_open("test.img", 1);
    mkfs();

    int first = ialloc();
    int second = ialloc();
    CTEST_ASSERT(second == first + 1, "successive iallocs should return incrementing numbers");
    image_close();
}

// --- alloc() tests ---

void test_alloc_returns_valid(void)
{
    image_open("test.img", 1);
    mkfs();

    int bnum = alloc();
    CTEST_ASSERT(bnum >= 0, "alloc should return a valid block number");
    image_close();
}

void test_alloc_skips_reserved(void)
{
    image_open("test.img", 1);
    mkfs();

    int bnum = alloc();
    CTEST_ASSERT(bnum >= 7, "alloc should not return a reserved block (0-6)");
    image_close();
}

// --- mkfs() tests ---

void test_mkfs_blocks_reserved(void)
{
    unsigned char block[BLOCK_SIZE];

    image_open("test.img", 1);
    mkfs();
    bread(2, block);  // block map
    image_close();

    // first 7 bits should be set (blocks 0-6 reserved)
    CTEST_ASSERT((block[0] & 0x7F) == 0x7F, "first 7 blocks should be marked non-free after mkfs");
}

int main(void)
{
    CTEST_VERBOSE(1);

    test_image_open();
    test_image_close();
    test_bread_bwrite();
    test_block_isolation();

    test_set_free_marks_used();
    test_set_free_marks_clear();
    test_find_free_empty_block();
    test_find_free_after_set();
    test_find_free_full_block();

    test_ialloc_returns_valid();
    test_ialloc_increments();

    test_alloc_returns_valid();
    test_alloc_skips_reserved();

    test_mkfs_blocks_reserved();

    CTEST_RESULTS();
    CTEST_EXIT();
}