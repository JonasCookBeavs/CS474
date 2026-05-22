#include <fcntl.h>
#include <unistd.h>
#include "image.h"

int image_fd;

int image_open(char *filename, int truncate)
{
    // Opens an image file with read/write permissions, truncating if truncate is true
    image_fd = open(filename, O_CREAT | O_RDWR | (truncate ? O_TRUNC : 0), 0600);
    return image_fd;
}

int image_close(void)
{
    // Closes the image file
    return close(image_fd);
}