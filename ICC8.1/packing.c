#include <stdio.h>
#include <string.h>
#include "pack.h"

#define ANIMAL_NAME_SIZE 16  // bytes
#define ANIMAL_LEG_COUNT_SIZE 2  // bytes
#define ANIMAL_POPULATION_SIZE 4  // bytes
#define ANIMAL_SIZE (ANIMAL_NAME_SIZE + ANIMAL_LEG_COUNT_SIZE + ANIMAL_POPULATION_SIZE) // bytes
#define ANIMAL_COUNT 3

// WARNING: do not modify this struct without updating ANIMAL_SIZE,
// above!
struct animal {
    char name[ANIMAL_NAME_SIZE];  // 16 bytes
    unsigned short leg_count;     // 16 bit, 2 bytes
    unsigned int population;      // 32 bit, 4 bytes
};

/*
 * Pack an animal record at the given index in the buffer.
 *
 * Hint: use memcpy() to copy the string bytes in. Use write_u16() and
 * write_32() to write the numbers.
 */
void store_animal(int index, unsigned char *buf, struct animal *a)
{
    index = index * ANIMAL_SIZE;
    memcpy(&buf[index], a->name, ANIMAL_NAME_SIZE);
    write_u16(&buf[index + ANIMAL_NAME_SIZE], a->leg_count);
    write_u32(&buf[index + ANIMAL_NAME_SIZE + ANIMAL_LEG_COUNT_SIZE], a->population);
}

/*
 * Unpack an animal record from the given index in the buffer.
 */
void load_animal(int index, unsigned char *buf, struct animal *a)
{
    index = index * ANIMAL_SIZE;
    memcpy(a->name, &buf[index], ANIMAL_NAME_SIZE);
    a->leg_count = read_u16(&buf[index + ANIMAL_NAME_SIZE]);
    a->population = read_u32(&buf[index + ANIMAL_NAME_SIZE + ANIMAL_LEG_COUNT_SIZE]);
}

int main(void)
{
    struct animal animals[ANIMAL_COUNT] = {
        {
            .name="aardvark",
            .leg_count=4,
            .population=27834412
        },
        {
            .name="manatee",
            .leg_count=1,
            .population=8357
        },
        {
            .name="centipede",
            .leg_count=100,
            .population=3810036467
        }
    };

    unsigned char buf[1024];

    for (int i = 0; i < ANIMAL_COUNT; i++)
        store_animal(i, buf, animals + i);

    for (int i = 0; i < ANIMAL_COUNT; i++) {
        struct animal a;

        load_animal(i, buf, &a);

        int pass = memcmp(a.name, animals[i].name, ANIMAL_NAME_SIZE) == 0 &&
            a.leg_count == animals[i].leg_count &&
            a.population == animals[i].population;

        printf("Test %d: %s\n", i, pass? "pass": "fail");
    }
}