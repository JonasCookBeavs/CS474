// pointing to stack address 0x7ffde07e84c4
// pointing to stack address 0x7ffde07e8494
// pointing to stack address 0x7ffde07e8464
// pointing to stack address 0x7ffde07e8434
// pointing to stack address 0x7ffde07e8404

// pointing to heap address 0x637b559ee6b0
// pointing to heap address 0x637b559ee700
// pointing to heap address 0x637b559ee750
// pointing to heap address 0x637b559ee7a0
// pointing to heap address 0x637b559ee7f0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

void *heap = NULL;

int stack_loop(int runs){
    int local = 1;
    printf("pointing to stack address %p\n", (void*)&local);
    if(runs == 1){
        return 0;
    }
    return stack_loop(runs - 1);
}

int main(){
    stack_loop(5);

    printf("\n");

    for(int i = 0; i < 5; i++){
        heap = malloc(64);
        printf("pointing to heap address %p\n", heap);
    }

    free(heap);
}