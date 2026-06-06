#include "block.h"
#include "free.h"
#include "inode.h"
#include "pack.h"
#include <stddef.h>

#define MAX_SYS_OPEN_FILES 64
#define INODE_SIZE 64
#define INODES_PER_BLOCK 64
#define INODE_FIRST_BLOCK 3

#define INODE_BITMAP_BLOCK 1

#define INODE_SIZE_OFFSET 0
#define INODE_OWNER_ID_OFFSET 4
#define INODE_PERMISSIONS_OFFSET 6
#define INODE_FLAGS_OFFSET 7
#define INODE_LINK_COUNT_OFFSET 8
#define INODE_BLOCK_PTR_OFFSET 9
#define INODE_BLOCK_PTR_SIZE 2
#define INODE_PTR_COUNT 16

static struct inode incore[MAX_SYS_OPEN_FILES] = {0};

struct inode *ialloc(void)
{
    unsigned char block[BLOCK_SIZE];

    bread(INODE_BITMAP_BLOCK, block);

    int index = find_free(block);

    if (index == -1)
    {
        return NULL;
    }

    struct inode *in = iget(index);

    if (in == NULL)
    {
        return NULL;
    }

    set_free(block, index, 1);
    bwrite(INODE_BITMAP_BLOCK, block);

    in->size = 0;
    in->owner_id = 0;
    in->permissions = 0;
    in->flags = 0;
    in->link_count = 0;
    in->inode_num = index;

    for (int i = 0; i < INODE_PTR_COUNT; i++)
    {
        in->block_ptr[i] = 0;
    }

    write_inode(in);

    return in;
}

struct inode *incore_find_free(void)
{
    int len = sizeof(incore) / sizeof(incore[0]);
    for (int i = 0; i < len; i++)
    {
        if (incore[i].ref_count == 0)
        {
            return &incore[i];
        }
    }

    return NULL;
}

struct inode *incore_find(unsigned int inode_num)
{
    int len = sizeof(incore) / sizeof(incore[0]);
    for (int i = 0; i < len; i++)
    {
        if (incore[i].ref_count != 0 && incore[i].inode_num == inode_num)
        {
            return &incore[i];
        }
    }

    return NULL;
}

void incore_free_all(void)
{
    int len = sizeof(incore) / sizeof(incore[0]);
    for (int i = 0; i < len; i++)
    {
        incore[i].ref_count = 0;
    }
}

void read_inode(struct inode *in, int inode_num)
{
    int block_num = inode_num / INODES_PER_BLOCK + INODE_FIRST_BLOCK;
    int block_offset = inode_num % INODES_PER_BLOCK;
    int byte_offset = block_offset * INODE_SIZE;

    unsigned char block[BLOCK_SIZE];

    bread(block_num, block);

    in->size = read_u32(block + byte_offset + INODE_SIZE_OFFSET);
    in->owner_id = read_u16(block + byte_offset + INODE_OWNER_ID_OFFSET);
    in->permissions = read_u8(block + byte_offset + INODE_PERMISSIONS_OFFSET);
    in->flags = read_u8(block + byte_offset + INODE_FLAGS_OFFSET);
    in->link_count = read_u8(block + byte_offset + INODE_LINK_COUNT_OFFSET);

    for (int i = 0; i < INODE_PTR_COUNT; i++)
    {
        in->block_ptr[i] = read_u16(block + byte_offset + INODE_BLOCK_PTR_OFFSET + i * INODE_BLOCK_PTR_SIZE);
    }
}

void write_inode(struct inode *in)
{
    int inode_num = in->inode_num;
    int block_num = inode_num / INODES_PER_BLOCK + INODE_FIRST_BLOCK;
    int block_offset = inode_num % INODES_PER_BLOCK;
    int byte_offset = block_offset * INODE_SIZE;

    unsigned char block[BLOCK_SIZE];
    bread(block_num, block);

    write_u32(block + byte_offset + INODE_SIZE_OFFSET, in->size);
    write_u16(block + byte_offset + INODE_OWNER_ID_OFFSET, in->owner_id);
    write_u8(block + byte_offset + INODE_PERMISSIONS_OFFSET, in->permissions);
    write_u8(block + byte_offset + INODE_FLAGS_OFFSET, in->flags);
    write_u8(block + byte_offset + INODE_LINK_COUNT_OFFSET, in->link_count);
    for (int i = 0; i < INODE_PTR_COUNT; i++)
    {
        write_u16(block + byte_offset + INODE_BLOCK_PTR_OFFSET + i * INODE_BLOCK_PTR_SIZE, in->block_ptr[i]);
    }

    bwrite(block_num, block);
}

struct inode *iget(int inode_num)
{
    struct inode *in = incore_find(inode_num);

    if (in != NULL)
    {
        in->ref_count++;
        return in;
    }

    in = incore_find_free();
    if (in == NULL)
    {
        return NULL;
    }

    read_inode(in, inode_num);
    in->ref_count = 1;
    in->inode_num = inode_num;

    return in;
}

void iput(struct inode *in)
{
    if (in->ref_count == 0)
    {
        return;
    }

    in->ref_count--;
    if (in->ref_count == 0)
    {
        write_inode(in);
    }
}