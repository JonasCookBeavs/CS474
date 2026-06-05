# Custom File System

## Building

* `make` to build.
* `make test` to build `testfs` if necessary and run it.
* `make clean` to clean up object files and temporary build products.
* `make pristine` to clean up all build products entirely.

## Files

* `testfs.c`: Tests used to verify filesystem functionality
* `image.c`: Functions used to open and close the image file
* `block.c`: Functions used to read and write blocks in the image file
* `free.c`: Functions used to find and mark free blocks or inodes in bitmap blocks
* `inode.c`: Functions used to manage on-disk and in-core inodes
* `inode.h`: Definitions and declarations related to inode management
* `pack.c`: Helper functions for reading and writing multibyte big-endian values
* `pack.h`: Function declarations for packing and unpacking binary values
* `mkfs.c`: Functions used to initialize the filesystem on disk

## Data

The filesystem image is divided into 4096-byte blocks.

A global file descriptor `image_fd` tracks the currently open image file.

Filesystem layout:

* Block 0: Superblock
* Block 1: Free inode bitmap
* Block 2: Free block bitmap
* Blocks 3-6: Inode table
* Remaining blocks: Data blocks

The inode bitmap and block bitmap store allocation information where each bit represents whether a resource is free or in use.

Each inode is 64 bytes in size, allowing 64 inodes per block and 256 total inodes in the filesystem.

The filesystem also maintains an in-core inode table in memory for actively used inodes.

## Functions

### Image and Block Management

* `image_open()`: Opens an image file, optionally truncating it
* `image_close()`: Closes the currently open image file
* `bread()`: Reads a block from the image file
* `bwrite()`: Writes a block to the image file

### Bitmap Management

* `set_free()`: Sets or clears a specific bit in a bitmap block
* `find_free()`: Finds the lowest numbered free entry in a bitmap block

### Inode Management

* `read_inode()`: Reads an inode from disk into memory
* `write_inode()`: Writes an inode from memory back to disk
* `incore_find_free()`: Finds a free in-core inode entry
* `incore_find()`: Finds an in-core inode by inode number
* `incore_free_all()`: Clears all in-core inode reference counts
* `iget()`: Retrieves an in-core inode, loading it from disk if necessary
* `iput()`: Releases an in-core inode and writes it to disk if no longer referenced
* `ialloc()`: Allocates a previously free inode and returns a pointer to its in-core inode structure

### Filesystem Initialization

* `alloc()`: Allocates a previously free data block and returns its index
* `mkfs()`: Initializes the filesystem structure on disk and reserves filesystem metadata blocks

## Inodes

Each inode stores metadata about a file:

* File size
* Owner ID
* Permissions
* Flags
* Link count
* Up to 16 block pointers

Inodes are stored on disk using packed big-endian binary values and are converted into `struct inode` values while in memory.

The in-core inode system tracks active inode usage using a reference count.

## Notes

* `bread()` still does not bake real bread.
* `ialloc()` returns `NULL` if no free inode can be allocated.
* `alloc()` returns `-1` if no free data block exists.
* `iget()` returns an existing in-core inode when possible instead of loading duplicate copies from disk.
* `iput()` automatically writes inode data back to disk when the inode is no longer referenced.