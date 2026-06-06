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
* `dir.c`: Functions used to manage directories
* `dir.h`: Definitions and declarations related to directory management
* `ls.c`: Function used to list the contents of a directory
* `dirbasename.c` Functions to help with getting base and directory names

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

Each directory entry is 32 bytes and contains an inode number and a file name of up to 15 characters. Each data block can hold up to 128 directory entries.

Inode flags indicate the type of file:
* 0: Unknown
* 1: Regular file
* 2: Directory

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
* `mkfs()`: Initializes the filesystem structure on disk, reserves filesystem metadata blocks, and creates the root directory

### Directory Management

* `directory_open()`: Opens a directory by inode number and returns a pointer to a directory structure
* `directory_get()`: Reads the next entry from an open directory into a directory entry structure
* `directory_close()`: Closes an open directory and releases its resources
* `directory_make()`: Creates a new directory at the specified path
* `namei()`: Maps a path string to the inode of the file or directory at the end of the path
* `ls()`: Lists the inode number and name of all entries in a directory

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
* `directory_open()` returns `NULL` if the specified inode cannot be loaded.
* `directory_get()` returns `-1` when there are no more entries to read.
* `namei()` returns `NULL` if any component of the path cannot be found.
* The root directory inode is always inode number 0, and both `.` and `..` entries point to it.