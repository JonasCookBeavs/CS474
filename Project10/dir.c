#include "inode.h"
#include "dir.h"
#include "mkfs.h"
#include "block.h"
#include "pack.h"
#include "dirbasename.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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

struct inode *namei(char *path)
{
    if(strcmp(path, "/") == 0)
    {
        return iget(ROOT_INODE_NUM);
    }

    struct directory *dir = directory_open(ROOT_INODE_NUM);
    struct directory_entry ent;

    char basename[PATH_MAX];
    get_basename(path, basename);

    while(true)
    {
        int dir_call = directory_get(dir, &ent);
        if(dir_call == -1)
        {
            break;
        }

        if(strcmp(ent.name, basename) == 0)
        {
            directory_close(dir);
            return iget(ent.inode_num);
        }
    }

    directory_close(dir);
    return NULL;
}

int directory_make(char *path)
{
    char dirname[PATH_MAX];
    char basename[PATH_MAX];
    get_dirname(path, dirname);
    get_basename(path, basename);

    struct inode *parent_inode = namei(dirname);
    if(parent_inode == NULL)
    {
        return -1;
    }

    struct inode *new_inode = ialloc();
    if(new_inode == NULL)
    {
        return -1;
    }

    int new_data_block = alloc();
    if(new_data_block == -1)
    {
        return -1;
    }

    unsigned char block[BLOCK_SIZE];
    memset(block, 0, BLOCK_SIZE);

    write_u16(block, new_inode->inode_num);
    strcpy((char *)block + NAME_OFFSET, ".");

    write_u16(block + DIR_ENTRY_SIZE, parent_inode->inode_num);
    strcpy((char *)block + DIR_ENTRY_SIZE + NAME_OFFSET, "..");

    bwrite(new_data_block, block);

    new_inode->flags = 2;
    new_inode->size = 64;
    new_inode->block_ptr[0] = new_data_block;

    int data_block_index = parent_inode->size / BLOCK_SIZE;
    int data_block_num = parent_inode->block_ptr[data_block_index];

    unsigned char parent_block[BLOCK_SIZE];
    bread(data_block_num, parent_block);

    int offset_in_block = parent_inode->size % BLOCK_SIZE;

    write_u16(parent_block + offset_in_block, new_inode->inode_num);
    strcpy((char *)parent_block + offset_in_block + NAME_OFFSET, basename);

    bwrite(data_block_num, parent_block);
    
    parent_inode->size += DIR_ENTRY_SIZE;

    iput(new_inode);
    iput(parent_inode);

    return 0;
}