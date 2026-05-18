# Custom File System

## Building

* `make` to build.
* `make test` to build `testfs` if necessary and run it.
* `make clean` to clean up all build products except the executable.
* `make pristine` to clean up all build products entirely.

## Files

* `testfs.c`: Tests to run to verify the codebase works
* `image.c`: Functions used to open and close the image file
* `block.c`: Functions used to read and write to the image file
* `free.c`: Functions used to find and mark free blocks or inodes in a bitmap
* `inode.c`: Functions used to allocate inodes from the inode map
* `mkfs.c`: Functions used to initialize the filesystem on disk

## Data

There is an image file that can be created by the user with blocks of size 4096 bytes each.
A global file descriptor `image_fd` tracks the open image file across the program.
The inode map (block 1) and block map (block 2) are bitmaps where each bit represents
whether that numbered resource is in use.

## Functions

* `image_open()`: Opens an image file, creating it if it doesn't exist and truncating it if specified to.
* `image_close()`: Closes the open image file
* `bread()`: Reads from the image file
* `bwrite()`: Writes into the image file
* `set_free()`: Sets or clears a specific bit in a bitmap block
* `find_free()`: Finds the lowest numbered free entry in a bitmap block
* `ialloc()`: Allocates a previously-free inode and returns its index
* `alloc()`: Allocates a previously-free data block and returns its index
* `mkfs()`: Writes the initial filesystem structure to disk, reserving the first 7 blocks

## Notes

* `bread()` does not currently bake you any real bread. This may change in the future.
* `ialloc()` and `alloc()` will return `-1` if there are no free inodes or blocks respectively. Plan accordingly.