#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

const int SIZE=1024;  // bytes

struct firework {
    int shell_caliber;
    float lift_charge;
    float burst_charge;
    int fuse_delay_ms;
};

int main(void)
{
    // Allocate some space from the OS
    void *heap = mmap(NULL, SIZE, PROT_READ|PROT_WRITE,
                      MAP_ANON|MAP_PRIVATE, -1, 0);

    // Challenge: Store your favorite letter at the byte `heap` points
    // to, then print it with `%c` and also with `%d`.
    *(char *)heap = 'j';
    printf("%c, %d\n", *(char *)heap, *(char *)heap);
    
    // Challenge: Store a string at the byte `heap` points to, then
    // print it.
    char *str = heap;
    strcpy(str, "Hello World!");
    printf("%s\n", (char *)heap);

    // Challenge: Store an integer at the byte `heap` points to, then
    // print it.
    int *number = heap;
    *number = 1234;
    printf("%d\n", *number);

    // Challenge: Store a float right *after* the int you just stored.
    // Print them both.
    float *float_num = heap + sizeof(number);
    *float_num = 56.78;
    printf("%d, %f\n", *number, *float_num);

    // Challenge: Store a short (a smallish integer) at byte offset 512
    // in the `heap` area. Then print it with `%d`, along with the int
    // and float you already stored.
    short *short_num = heap + 512;
    *short_num = 3;
    printf("%d, %d, %f\n", *short_num, *number, *float_num);

    // Challenge: Store a struct firework at byte offset 48 in the
    // `heap` area. Make a `struct firework *` initialized to the right
    // position, then fill it with data (with the `->` operator). Then
    // print out the data, and also print out the int, float, and short
    // from above.
    struct firework *fw = heap + 48;
    fw->shell_caliber = 10;
    fw->burst_charge = 12.5;
    fw->fuse_delay_ms = 300;
    fw->lift_charge = 135.75;

    printf("Shell Caliber: %d\n", fw->shell_caliber);
    printf("Burst Charge: %f\n", fw->burst_charge);
    printf("Fuse Delay: %d\n", fw->fuse_delay_ms);
    printf("Lift Charge: %f\n", fw->lift_charge);
    printf("%d, %d, %f\n", *short_num, *number, *float_num);

    // Challenge: Store a second struct firework directly after the
    // first one. Initialize and print.
    struct firework *fw2 = heap + 48 + sizeof(fw);
    fw2->shell_caliber = 20;
    fw2->burst_charge = 52.5;
    fw2->fuse_delay_ms = 700;
    fw2->lift_charge = 295.60;
    printf("Shell Caliber: %d\n", fw2->shell_caliber);
    printf("Burst Charge: %f\n", fw2->burst_charge);
    printf("Fuse Delay: %d\n", fw2->fuse_delay_ms);
    printf("Lift Charge: %f\n", fw2->lift_charge);
}