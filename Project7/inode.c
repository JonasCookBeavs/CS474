#include "block.h"
#include "free.h"
#include "inode.h"

int ialloc(void)
{
    unsigned char block[4096];

    bread(1, block);
    int index = find_free(block);
    if (index == -1)
    {
        return -1;
    }
    set_free(block, index, 1);
    bwrite(1, block);

    return index;
}