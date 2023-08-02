#include "ACH-2.h"

#define byte uint8_t
#define uint unsigned int

#define MAGIC
#ifdef MAGIC

/// Block spike strength
#define SPIKE_STRENGTH 0x4
/// Block jump constant
#define JUMP_CONSTANT 0xfc

/// RMC2 Constant 1
#define RMC2MC1 0x96
/// RMC2 Constant 2
#define RMC2MC2 0x1A
/// RMC2 Constant 3
#define RMC2MC3 0xF0

/// RM1 dynamic seed 1
static byte RMC1C1 = 0x50;
/// RM1 dynamic seed 2
static byte RMC1C2 = 0x0A;
/// RM1 dynamic seed 3
static byte RMC1C3 = 0x0F;
/// RM2 dynamic seed 1
static byte RMC2C1 = 0x0F;
/// RM2 dynamic seed 2
static byte RMC2C2 = 0x0A;
/// RM2 dynamic seed 3
static byte RMC2C3 = 0x50;

/// RM1 Seed Constant Minimum
#define RM1_SC_MIN 11
/// RM2 Seed Constant Minimum
#define RM2_SC_MIN 12

/// GSC Computation Iteration seed Minimum
#define GSC_CI_MIN 13
/// GSC Computation Iteration seed Limit
#define GSC_CI_LIM 47
/// GSC Term 1 sample index
#define GSC_T1_SAM 21
/// GSC Term 2 sample index
#define GSC_T2_SAM 43
/// GSC Term 3 sample index
#define GSC_T3_SAM 60

#endif  // !MAGIC

static int dynamicSeed;

void restoreDynamicSeeds() {
    RMC1C1 = 0x50;
    RMC1C2 = 0x0A;
    RMC1C3 = 0x0F;
    RMC2C1 = 0x0F;
    RMC2C2 = 0x0A;
    RMC2C3 = 0x50;
}

static void otpArray(byte* array, uint n, byte* key) {
    for (int i = 0; i < n; i++)
        array[i] ^= key[i];
}

void rotateArray(byte* array, uint n, int amount) {
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

byte* M1(byte* a, uint n, byte* b, byte* c, int sc) {
    byte* o = malloc(n);
    for (int i = 0; i < n; i++)  // todo: analyze and simplify with boolean algebra
        o[i] = a[i] ^ (b[i] * sc % 256) & (~a[i] ^ c[i]) ^ ~(b[i] ^ c[i]);
    return 0;
}

byte* M2(byte* a, uint n, byte* b, byte* c, int sc) {
    byte* o = malloc(n);
    for (int i = 0; i < n; i++)  // todo: analyze and simplify with boolean algebra
        o[i] = ~((~a[i] & b[i]) ^ (a[i] & c[i]) ^ (~(b[i] & (c[i] * sc % 255)))) ^ (a[i] ^ b[i]) ^ (b[i] ^ c[i]);
}

byte* RM1(byte* a, uint n, int sc) {
    byte* o = malloc(n);
    sc = sc == 0 ? RM1_SC_MIN : sc;
    RMC1C1 = RMC1C1 * sc % 255;
    RMC1C2 = RMC1C2 * sc % 255;
    RMC1C3 = RMC1C3 * sc % 255;
    byte* t1 = malloc(n);
    byte* t2 = malloc(n);
    byte* t3 = malloc(n);
    for (int i = 0; i < n; i++) {
        t1[i] = a[i];
        t2[i] = a[i];
        t3[i] = a[i];
    }
    rotateArray(t1, n, -RMC1C1);
    rotateArray(t2, n, RMC1C2);
    rotateArray(t3, n, -RMC1C3);

    for (int i = 0; i < n; i++)
        o[i] = t1[i] ^ t2[i] ^ t3[i];

    free(t1);
    free(t2);
    free(t3);
    return o;
}

byte* RM2(byte* a, uint n, int sc) {
    byte* o = malloc(n);
    sc = sc == 0 ? RM2_SC_MIN : sc;
    RMC2C1 = RMC2MC1 % sc;
    RMC2C2 = RMC2MC2 % sc;
    RMC2C2 = RMC2MC3 % sc;
    byte* t1 = malloc(n);
    byte* t2 = malloc(n);
    byte* t3 = malloc(n);
    for (int i = 0; i < n; i++) {
        t1[i] = a[i];
        t2[i] = a[i];
        t3[i] = a[i];
    }
    rotateArray(a, n, RMC2C1);
    rotateArray(a, n, -RMC2C2);
    rotateArray(a, n, RMC2C3);

    for (int i = 0; i < n; i++)
        o[i] = t1[i] ^ t2[i] ^ t3[i];

    free(t1);
    free(t2);
    free(t3);
    return o;
}

int GSC(byte* block, int ci) {  // todo: this needs to be completely rewritten
    ci == 0 ? GSC_CI_MIN : ci;
    ci %= GSC_CI_LIM;
    float t1 = sin(block[GSC_T1_SAM] * ci / 20);
    float t2 = cos(pow(ci, block[GSC_T2_SAM] / 10));
    float t3 = block[GSC_T3_SAM] * (ci + 1) / 100;
    t2 = pow(t2, t3);
    float val = t1 * t2;
    byte valb[4];
    memcpy(valb, &val, 4);
    valb[1] = valb[1] << 1 | valb[1] >> 7;
    byte tmp = valb[0];
    valb[0] = valb[1];
    valb[1] = tmp;
    int o;
    memcpy(&o, valb, 4);
    return o;
}

static byte* ach2(const byte* data, uint n) {
    return (void*)0;  // NYI
}

byte* chainComputeHash(const byte* data, uint n) {
    return ach2(data, n);
}

byte* computeHash(const byte* data, uint n) {
    restoreDynamicSeeds();
    return ach2(data, n);  // NYI
}

#undef byte
#undef uint
#undef MAGIC