#include "mkfs.h"
#include "block.h"
#include "inode.h"
#include "pack.h"
#include <string.h>

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

    struct inode *root_inode = ialloc();
    int root_inode_num = root_inode->inode_num;
    int root_data_block = alloc();

    root_inode->flags = 2;
    root_inode->size = 64;
    root_inode->block_ptr[0] = root_data_block;

    memset(block, 0, sizeof(block));

    write_u16(block, root_inode_num);
    strcpy((char *)block + NAME_OFFSET, ".");

    write_u16(block + DIR_ENTRY_SIZE, root_inode_num);
    strcpy((char *)block + DIR_ENTRY_SIZE + NAME_OFFSET, "..");

    bwrite(root_data_block, block);

    iput(root_inode);
}