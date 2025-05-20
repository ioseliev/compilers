#include "bitset.h"


bitset_t bitset() {
    bitset_t ret;
    for (uint8_t i = 0; i < 32; ++i) {
        ret.bits[i] = 0;
    }
    return ret;
}

void set(bitset_t *set, uint8_t index) {
    set->bits[index >> 3] |= (1 << (index & 7));
}

bool get(bitset_t *set, uint8_t index) {
    return (set->bits[index >> 3] & (1 << (index & 7))) != 0;
}

void clear(bitset_t *set) {
    for (uint8_t i = 0; i < 32; ++i) {
        set->bits[i] = 0;
    }
}