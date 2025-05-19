#ifndef BITSET_H
#define BITSET_H
#include <stdbool.h>
#include <stdint.h>


typedef struct {
    uint8_t bits[32];
} bitset_t;

bitset_t bitset();

void set(bitset_t *set, uint8_t index);

bool get(bitset_t *set, uint8_t index);

void clear(bitset_t *set);

#endif