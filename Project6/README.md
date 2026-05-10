# Custom File System

## Building

* `make` to build. An executable called `clife` will be produced.
* `make test` to build `testfs` if necessary and run it.
* `make clean` to clean up all build products except the executable.
* `make pristine` to clean up all build products entirely.

## Files

* `testfs.c`: Tests to run to verify the codebase works
* `image.c`: Functions used to open and close the image file
* `block.c`: Functions used to read and write to the image file

## Data

There is an image file that can be created by the user with blocks of size 4096 bytes each. 
A global file descriptor `image_fd` tracks the open image file across the program.

## Functions

* `image_open()`: Opens an image file, creating it if it doesn't exist and truncate it if specified to.
* `image_close()`: Closes the open image file
* `bread()`: Reads from the image file
* `bwrite()`: Writes into the image file

## Notes

* `bread()` does not currently bake you any real bread. This may change in the future.