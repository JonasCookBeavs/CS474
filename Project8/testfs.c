#define CTEST_ENABLE
#include "ctest.h"
#include <string.h>
#include "image.h"
#include "block.h"
#include "free.h"
#include "inode.h"
#include "mkfs.h"
#include "pack.h"

#define BLOCK_SIZE 4096
#define TEST_IMAGE "test.img"

void test_image_open(void)
{
    int fd = image_open(TEST_IMAGE, 1);
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

    image_open(TEST_IMAGE, 1);
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

    image_open(TEST_IMAGE, 1);
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

// --- alloc() tests ---

void test_alloc_returns_valid(void)
{
    image_open(TEST_IMAGE, 1);
    mkfs();

    int bnum = alloc();
    CTEST_ASSERT(bnum >= 0, "alloc should return a valid block number");
    image_close();
}

void test_alloc_skips_reserved(void)
{
    image_open(TEST_IMAGE, 1);
    mkfs();

    int bnum = alloc();
    CTEST_ASSERT(bnum >= 7, "alloc should not return a reserved block (0-6)");
    image_close();
}

// --- mkfs() tests ---

void test_mkfs_blocks_reserved(void)
{
    unsigned char block[BLOCK_SIZE];

    image_open(TEST_IMAGE, 1);
    mkfs();
    bread(2, block); // block map
    image_close();

    // first 7 bits should be set (blocks 0-6 reserved)
    CTEST_ASSERT((block[0] & 0x7F) == 0x7F, "first 7 blocks should be marked non-free after mkfs");
}

// --- image setup and teardown ---
static void setup_test_image(void)
{
    image_open(TEST_IMAGE, 1);
    incore_free_all();
}

static void cleanup_test_image(void)
{
    incore_free_all();
    image_close();
}

// --- incore[] ---

void test_incore_find_free_empty(void)
{
    incore_free_all();

    struct inode *in = incore_find_free();

    CTEST_ASSERT(in != NULL, "Expected to find a free in-core inode");
    CTEST_ASSERT(in->ref_count == 0, "Free inode should have ref_count 0");
}

void test_incore_find_existing_inode(void)
{
    incore_free_all();

    struct inode *in = incore_find_free();

    CTEST_ASSERT(in != NULL, "Expected to find a free in-core inode");

    in->inode_num = 42;
    in->ref_count = 1;
    in->size = 1234;
    in->owner_id = 99;

    struct inode *found = incore_find(42);

    CTEST_ASSERT(found != NULL, "Expected to find inode 42");
    CTEST_ASSERT(found == in, "Expected returned pointer to be the same inode");
    CTEST_ASSERT(found->size == 1234, "Expected size to match");
    CTEST_ASSERT(found->owner_id == 99, "Expected owner_id to match");
}

void test_incore_free_all_resets_ref_counts(void)
{
    incore_free_all();

    struct inode *in1 = incore_find_free();
    CTEST_ASSERT(in1 != NULL, "Expected first free inode");

    in1->inode_num = 1;
    in1->ref_count = 1;

    struct inode *in2 = incore_find_free();
    CTEST_ASSERT(in2 != NULL, "Expected second free inode");

    in2->inode_num = 2;
    in2->ref_count = 1;

    incore_free_all();

    CTEST_ASSERT(incore_find(1) == NULL, "Expected inode 1 to be cleared");
    CTEST_ASSERT(incore_find(2) == NULL, "Expected inode 2 to be cleared");

    struct inode *free_inode = incore_find_free();
    CTEST_ASSERT(free_inode != NULL, "Expected free inode after reset");
}

// --- read_inode() and write_inode() ---

void test_write_then_read_inode(void)
{
    setup_test_image();

    struct inode out = {0};

    out.inode_num = 5;
    out.size = 12345;
    out.owner_id = 77;
    out.permissions = 064;
    out.flags = 3;
    out.link_count = 2;

    for (int i = 0; i < INODE_PTR_COUNT; i++)
        out.block_ptr[i] = 100 + i;

    write_inode(&out);

    struct inode in = {0};
    read_inode(&in, 5);

    CTEST_ASSERT(in.size == 12345, "size should match");
    CTEST_ASSERT(in.owner_id == 77, "owner_id should match");
    CTEST_ASSERT(in.permissions == 064, "permissions should match");
    CTEST_ASSERT(in.flags == 3, "flags should match");
    CTEST_ASSERT(in.link_count == 2, "link_count should match");

    for (int i = 0; i < INODE_PTR_COUNT; i++)
        CTEST_ASSERT(in.block_ptr[i] == 100 + i,
                     "block pointer should match");

    cleanup_test_image();
}

void test_write_inode_does_not_store_ref_count(void)
{
    setup_test_image();

    struct inode out = {0};

    out.inode_num = 6;
    out.size = 111;
    out.ref_count = 99;

    write_inode(&out);

    struct inode in = {0};
    read_inode(&in, 6);

    CTEST_ASSERT(in.size == 111, "size should be stored");
    CTEST_ASSERT(in.ref_count == 0,
                 "ref_count should not be read from disk data");

    cleanup_test_image();
}

// --- iget() ---

void test_iget_loads_inode_from_disk(void)
{
    setup_test_image();

    struct inode out = {0};

    out.inode_num = 10;
    out.size = 2222;
    out.owner_id = 12;
    out.permissions = 7;
    out.flags = 1;
    out.link_count = 4;
    out.block_ptr[0] = 300;

    write_inode(&out);

    struct inode *in = iget(10);

    CTEST_ASSERT(in != NULL, "iget should return inode");
    CTEST_ASSERT(in->inode_num == 10, "inode_num should be set");
    CTEST_ASSERT(in->ref_count == 1, "ref_count should be 1");
    CTEST_ASSERT(in->size == 2222, "size should match disk inode");
    CTEST_ASSERT(in->owner_id == 12, "owner_id should match");
    CTEST_ASSERT(in->block_ptr[0] == 300, "block pointer should match");

    cleanup_test_image();
}

void test_iget_existing_inode_increments_ref_count(void)
{
    setup_test_image();

    struct inode out = {0};
    out.inode_num = 11;
    out.size = 3333;
    write_inode(&out);

    struct inode *first = iget(11);
    struct inode *second = iget(11);

    CTEST_ASSERT(first != NULL, "first iget should succeed");
    CTEST_ASSERT(second != NULL, "second iget should succeed");
    CTEST_ASSERT(first == second, "same inode should return same pointer");
    CTEST_ASSERT(first->ref_count == 2, "ref_count should increment");

    cleanup_test_image();
}

// --- iput() ---

void test_iput_decrements_ref_count(void)
{
    setup_test_image();

    struct inode out = {0};
    out.inode_num = 12;
    out.size = 4444;
    write_inode(&out);

    struct inode *in = iget(12);
    iget(12);

    CTEST_ASSERT(in->ref_count == 2, "ref_count should start at 2");

    iput(in);

    CTEST_ASSERT(in->ref_count == 1, "iput should decrement ref_count");

    cleanup_test_image();
}

void test_iput_writes_inode_when_ref_count_zero(void)
{
    setup_test_image();

    struct inode out = {0};
    out.inode_num = 13;
    out.size = 5555;
    write_inode(&out);

    struct inode *in = iget(13);
    CTEST_ASSERT(in != NULL, "iget should succeed");

    in->size = 9999;

    iput(in);

    CTEST_ASSERT(in->ref_count == 0, "ref_count should become 0");

    struct inode check = {0};
    read_inode(&check, 13);

    CTEST_ASSERT(check.size == 9999,
                 "iput should write inode to disk when ref_count reaches 0");

    cleanup_test_image();
}

void test_iput_ref_count_already_zero(void)
{
    setup_test_image();

    struct inode in = {0};

    in.inode_num = 14;
    in.ref_count = 0;

    iput(&in);

    CTEST_ASSERT(in.ref_count == 0,
                 "iput should leave ref_count at 0 if already 0");

    cleanup_test_image();
}

// --- ialloc() ---

void test_ialloc_returns_initialized_inode(void)
{
    setup_test_image();

    struct inode *in = ialloc();

    CTEST_ASSERT(in != NULL, "ialloc should return inode");
    CTEST_ASSERT(in->ref_count == 1, "allocated inode should have ref_count 1");
    CTEST_ASSERT(in->size == 0, "size should be initialized to 0");
    CTEST_ASSERT(in->owner_id == 0, "owner_id should be initialized to 0");
    CTEST_ASSERT(in->permissions == 0, "permissions should be initialized to 0");
    CTEST_ASSERT(in->flags == 0, "flags should be initialized to 0");

    for (int i = 0; i < INODE_PTR_COUNT; i++)
        CTEST_ASSERT(in->block_ptr[i] == 0,
                     "block pointers should be initialized to 0");

    cleanup_test_image();
}

void test_ialloc_writes_initialized_inode_to_disk(void)
{
    setup_test_image();

    struct inode *in = ialloc();
    CTEST_ASSERT(in != NULL, "ialloc should return inode");

    unsigned int inode_num = in->inode_num;

    struct inode check = {0};
    read_inode(&check, inode_num);

    CTEST_ASSERT(check.size == 0, "disk inode size should be 0");
    CTEST_ASSERT(check.owner_id == 0, "disk inode owner_id should be 0");
    CTEST_ASSERT(check.permissions == 0, "disk inode permissions should be 0");
    CTEST_ASSERT(check.flags == 0, "disk inode flags should be 0");

    for (int i = 0; i < INODE_PTR_COUNT; i++)
        CTEST_ASSERT(check.block_ptr[i] == 0,
                     "disk inode block pointers should be 0");

    cleanup_test_image();
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

    test_alloc_returns_valid();
    test_alloc_skips_reserved();

    test_mkfs_blocks_reserved();

    test_incore_find_free_empty();
    test_incore_find_existing_inode();
    test_incore_free_all_resets_ref_counts();

    test_write_then_read_inode();
    test_write_inode_does_not_store_ref_count();

    test_iget_existing_inode_increments_ref_count();
    test_iget_loads_inode_from_disk();

    test_iput_decrements_ref_count();
    test_iput_ref_count_already_zero();
    test_iput_writes_inode_when_ref_count_zero();

    test_ialloc_returns_initialized_inode();
    test_ialloc_writes_initialized_inode_to_disk();

    CTEST_RESULTS();
    CTEST_EXIT();
}