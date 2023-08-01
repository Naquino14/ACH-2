#include "ACH-2.h"

#define byte uint8_t
#define uint unsigned int

#define SPIKE_STRENGTH 0x4
#define JUMP_CONSTANT 0xfc

byte* computeHash(const byte* data, uint n) {
    return (void*)0;  // NYI
}

static void otpArray(byte* array, uint n, byte* key) {
    for (int i = 0; i < n; i++)
        array[i] ^= key[i];
}

static void rotateArray(byte* array, uint n, int amount) {
    bool right = amount > 0;
    amount *= right ? 1 : -1;
    amount %= n;

    for (int i = 0; i < amount; i++) {
        // todo
    }
}

void blockSpike(byte* block) {
    int mult = 0;
    for (int i = 0; i < ACH_2_BLOCK_SIZE; i++) {
        mult = block[ACH_2_BLOCK_SIZE - 1 - i] * SPIKE_STRENGTH;
        mult %= ACH_2_BLOCK_SIZE - 1;
        block[mult] ^= mult;
    }
}

void blockJump(byte* block) {
    for (int i = 0; i < ACH_2_BLOCK_SIZE; i++) {
        int target = (i + 2) % ACH_2_BLOCK_SIZE;
        block[target] = ((JUMP_CONSTANT - block[i]) * block[target]) % 256;
    }
}

#undef byte