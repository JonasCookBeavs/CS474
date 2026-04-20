// pointing to stack address  0x7fff15e6dfd4
// pointing to heap address   0x59c0410342a0
// pointing to global address 0x59c02d556010

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

void *heap = NULL;
int gloal_var = 2;

int main(){
    heap = malloc(256);
    int local = 1;
    printf("pointing to stack address %p\n", (void*)&local);
    
    printf("pointing to global address %p\n", (void*)&gloal_var);
    printf("pointing to main address %p\n", (void*)main);
}