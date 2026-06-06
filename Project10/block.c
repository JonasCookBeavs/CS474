#include "image.h"
#include "free.h"
#include "block.h"
#include <unistd.h>



unsigned char *bread(int block_num, unsigned char *block)
{
    lseek(image_fd, block_num * BLOCK_SIZE, SEEK_SET); // Seek to the proper block
    read(image_fd, block, BLOCK_SIZE);                 // Read into the buffer
    return block;                                      // Return the buffer
}

void bwrite(int block_num, unsigned char *block)
{
    lseek(image_fd, block_num * BLOCK_SIZE, SEEK_SET); // Seek to the proper block
    write(image_fd, block, BLOCK_SIZE);                // Write from the buffer into the disk
}

int alloc(void)
{
    unsigned char block[4096];

    bread(2, block);
    int index = find_free(block);
    if (index == -1)
    {
        return -1;
    }
    set_free(block, index, 2);
    bwrite(2, block);

    return index;
}