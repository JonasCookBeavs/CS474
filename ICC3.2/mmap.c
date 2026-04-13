#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

const int SIZE=1024;  // bytes

struct bicycle {
    int wheel_count;
    char name[32];
};

int main(void)
{
    // Allocate some space from the OS
    void *heap = mmap(NULL, SIZE, PROT_READ|PROT_WRITE,
                  MAP_ANON|MAP_PRIVATE, -1, 0);

    // Challenge 1:
    //
    // Treat heap as an array of chars
    // Store a string there with strcpy() and print it
    strcpy(heap, "Hello World!");
    printf("%s\n", (char *)heap);

    // Challenge 2:
    //
    // Treat heap as an array of ints
    // Use a loop to store the values 0, 10, 20, 30, 40 in it
    // Use a loop to retrieve the value and print them
    for(int i = 0; i < 5; i++){
        int *num = heap + i * sizeof(int);
        *num = i * 10; 
    }
    for(int i = 0; i < 5; i++){
        printf("Number %d: %d\n", i, *(int *)(heap + i * sizeof(int)));
    }

    // Challenge 3:
    //
    // Treat heap as an array of struct bicycles
    // Store 3 bicycles
    // Print out the bicycle data in a loop
    struct bicycle *b = (struct bicycle *)heap;

    for(int i = 0; i < 3; i++){
        sprintf((b+i)->name, "Bike %d", i);
        (b+i)->wheel_count = i;
    }

    for(int i = 0; i < 3; i++){
        printf("Bicycle %d: %s with %d wheels\n", i, (b+i)->name, (b+i)->wheel_count);
    }

    // Challenge 4:
    //
    // If you used pointer notation, redo challenges 1-3 with array
    // notation, and vice versa.

    // Challenge 1b
    printf("\n");
    strcpy(heap, "Hello World!");
    printf("%s\n", (char *)heap);

    // Challenge 2b
    int *nums = (int *)heap;

    for(int i = 0; i < 5; i++){
        nums[i] = i * 10;
    }
    for(int i = 0; i < 5; i++){
        printf("Number %d: %d\n", i, nums[i]);
    }

    // Challenge 3b
    struct bicycle *b2 = (struct bicycle *)heap;

    for(int i = 0; i < 3; i++){
        sprintf(b2[i].name, "Bike %d", i);
        b2[i].wheel_count = i;
    }

    for(int i = 0; i < 3; i++){
        printf("Bicycle %d: %s with %d wheels\n", i, b2[i].name, b2[i].wheel_count);
    }

    printf("\n");
    // Challenge 5:
    //
    // Make the first 32 bytes of the heap a string (array of chars),
    // and follow that directly with an array of ints.
    //
    // Store a short string in the string area of the heap
    // Use a loop to store the values 0, 10, 20, 30, 40 in the int array
    // Use a loop to retrieve the value and print them
    // Print the string
    char *str = heap;
    strcpy(str, "Hello World!");
    printf("%s\n", str);

    for(int i = 0; i < 5; i++){
        int *num = heap + i * sizeof(int) + 32;
        *num = i * 10; 
    }
    for(int i = 0; i < 5; i++){
        printf("Number %d: %d\n", i, *(int *)(heap + i * sizeof(int) + 32));
    }

    printf("\n");
    // Challenge 6:
    //
    // Make an array of interleaved `struct bicycle` and `int` data
    // types.
    //
    // The first entry in the heap will be a `struct bicycle`, then
    // right after that, an `int`, then right after that a `struct
    // bicycle`, then an `int`, etc.
    //
    // Store as many of these as can fit in SIZE bytes. Loop through,
    // filling them up with programmatically-generated data. Then loop
    // through again and print out the elements.
    
    // Fill
    int i = 0;
    int total_size = sizeof(struct bicycle) + sizeof(int);
    while ((i + 1) * total_size <= SIZE) {
        char *slot = (char *)heap + i * total_size;
        
        struct bicycle *bike = (struct bicycle *)slot;
        sprintf(bike->name, "Bike %d", i);
        bike->wheel_count = i;

        int *num = (int *)(slot + sizeof(struct bicycle));
        *num = i * 10;

        i++;
    }

    // Print
    int count = SIZE / total_size;
    for (int j = 0; j < count; j++) {
        char *slot = (char *)heap + j * total_size;

        struct bicycle *bike = (struct bicycle *)slot;
        int *num = (int *)(slot + sizeof(struct bicycle));

        printf("Bike %d: %s with %d wheels, num = %d\n", j, bike->name, bike->wheel_count, *num);
    }

    // Free it up
    munmap(heap, SIZE);
}