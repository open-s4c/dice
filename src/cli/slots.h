#ifndef DICE_SLOTS_H
#define DICE_SLOTS_H
#include <stdbool.h>
#include <stddef.h>
#define SLOT_LIMIT 256
#define NAME_SIZE  128
struct slot {
    char name[NAME_SIZE];
    int forced, number;
};
int slots_resolve(struct slot *slots, size_t count,
                  bool edges[SLOT_LIMIT][SLOT_LIMIT], char *error, size_t size);
#endif
