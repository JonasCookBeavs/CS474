#include "mkfs.h"
#include "block.h"
#include <string.h>

#define BLOCK_SIZE 4096

void mkfs(void)
{
    unsigned char block[BLOCK_SIZE];

    memset(block, 0, BLOCK_SIZE);
    bwrite(0, block);  // superblock
    bwrite(1, block);  // inode map
    bwrite(3, block);
    bwrite(4, block);
    bwrite(5, block);
    bwrite(6, block);

    block[0] = 0x7F;
    bwrite(2, block);  // block map
}