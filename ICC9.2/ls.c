#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

/*
 * Takes a dirname as a string ("." for current directory) and shows the
 * contents of that directory.
 */
void ls(const char *dirname)
{
    DIR *dir = opendir(dirname);
    struct dirent *addr = readdir(dir);
    char fullpath[4096];
    
    while (addr != NULL)
    {
        struct stat statbuf;
        sprintf(fullpath, "%s/%s", dirname, addr->d_name);
        stat(fullpath, &statbuf);
        printf("Name: %s, Inode Num: %lu, Mode: %o, User ID: %d, Group ID: %d, Size: %ld, Block size: %ld, Num of 512 Blocks: %ld\n", 
            addr->d_name, addr->d_ino, statbuf.st_mode, statbuf.st_uid, statbuf.st_gid, statbuf.st_size, statbuf.st_blksize, statbuf.st_blocks);
        addr = readdir(dir);
    }
    closedir(dir);
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "usage: myls dirname\n");
        exit(1);
    }

    char *dirname = argv[1];

    ls(dirname);
}