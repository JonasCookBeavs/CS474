#include "image.h"
#include <unistd.h>

#define BLOCK_SIZE 4096

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