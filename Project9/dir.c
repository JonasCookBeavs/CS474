#include "inode.h"
#include "dir.h"
#include "mkfs.h"
#include "block.h"
#include "pack.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

struct directory *directory_open(int inode_num)
{
    struct inode *inode = iget(inode_num);
    if (inode == NULL)
    {
        return NULL;
    }

    struct directory *dir = malloc(sizeof(struct directory));
    if (dir == NULL)
    {
        iput(inode);
        return NULL;
    }

    dir->inode = inode;
    dir->offset = 0;

    return dir;
}

int directory_get(struct directory *dir, struct directory_entry *ent)
{
    unsigned char block[BLOCK_SIZE];
    
    if(dir->offset >= dir->inode->size)
    {
        return -1;
    }

    int data_block_index = dir->offset / BLOCK_SIZE;

    int data_block_num = dir->inode->block_ptr[data_block_index];
    bread(data_block_num, block); 

    int offset_in_block = dir->offset % BLOCK_SIZE;

    ent->inode_num = read_u16(block + offset_in_block);
    strcpy(ent->name, (char *)block + offset_in_block + NAME_OFFSET);

    dir->offset += DIR_ENTRY_SIZE;

    return 0;
}

void directory_close(struct directory *d)
{
    iput(d->inode);
    free(d);
}