#ifndef INODE_H
#define INODE_H

#define INODE_PTR_COUNT 16

struct inode *ialloc(void);
struct inode *incore_find_free(void);
struct inode *incore_find(unsigned int inode_num);
void incore_free_all(void);
void read_inode(struct inode *in, int inode_num);
void write_inode(struct inode *in);
struct inode *iget(int inode_num);
void iput(struct inode *in);

struct inode {
    unsigned int size;
    unsigned short owner_id;
    unsigned char permissions;
    unsigned char flags;
    unsigned char link_count;
    unsigned short block_ptr[INODE_PTR_COUNT];

    unsigned int ref_count;
    unsigned int inode_num;
};
#endif