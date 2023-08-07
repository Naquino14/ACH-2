#include "ACH-2.h"

#define HELPER
#ifdef HELPER

#define byte uint8_t
#define uint unsigned int
#define MOD(a, b) (((a % b) + b) % b)
#define ABS(a) (a > 0 ? a : -a)
/// Get read index
#define GRI(ci) (ci * 32)
#define MIN(a, b) (a < b ? a : b)

#endif  // !HELPER

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

/// GSC Power Limit
#define GSC_POWN_LIM 1732
#define GSC_SC_LIM 4646351
/// GSC Term 1 sample index
#define GSC_T1_SAM 21
/// GSC Term 2 sample index
#define GSC_T2_SAM 43
/// GSC Term 3 sample index
#define GSC_T3_SAM 60

/// Subblock A
#define SA 0
/// Subblock B
#define SB 1
/// Subblock C
#define SC 2
/// Subblock D
#define SD 3
/// Subblock E
#define SE 4
/// Subblock F
#define SF 5
/// Subblock G
#define SG 6
/// Subblock H
#define SH 7
/// Number of subblocks
#define NUM_SUBBLOCKS 8
/// Padding byte
#define BLOCK_PAD 0x15

/// Block Rotation index Sample 1
#define BRS1 0x21
/// Block Rotation index Sample 2
#define BRS2 0x2F
/// Block Rotation index Sample 3
#define BRS3 0x06

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
    for (uint i = 0; i < n; i++)
        array[i] ^= key[i];
}

void rotateArray(byte* array, uint n, int amount) {
    byte* tmp = malloc(n);
    for (uint i = 0; i < n; i++)
        tmp[MOD(amount + i, n)] = array[i];
    memcpy(array, tmp, n);
    free(tmp);
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
    for (uint i = 0; i < n; i++)  // todo: analyze and simplify with boolean algebra
        o[i] = ((a[i] ^ (b[i] * sc % 256)) & (~a[i] ^ c[i])) ^ ~(b[i] ^ c[i]);
    return o;
}

byte* M2(byte* a, uint n, byte* b, byte* c, int sc) {
    byte* o = malloc(n);
    for (uint i = 0; i < n; i++)  // todo: analyze and simplify with boolean algebra
        o[i] = ~((~a[i] & b[i]) ^ (a[i] & c[i]) ^ (~(b[i] & (c[i] * sc % 255)))) ^ (a[i] ^ b[i]) ^ (b[i] ^ c[i]);
    return o;
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
    for (uint i = 0; i < n; i++) {
        t1[i] = a[i];
        t2[i] = a[i];
        t3[i] = a[i];
    }
    rotateArray(t1, n, -RMC1C1);
    rotateArray(t2, n, RMC1C2);
    rotateArray(t3, n, -RMC1C3);

    for (uint i = 0; i < n; i++)
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
    for (uint i = 0; i < n; i++) {
        t1[i] = a[i];
        t2[i] = a[i];
        t3[i] = a[i];
    }
    rotateArray(a, n, RMC2C1);
    rotateArray(a, n, -RMC2C2);
    rotateArray(a, n, RMC2C3);

    for (uint i = 0; i < n; i++)
        o[i] = t1[i] ^ t2[i] ^ t3[i];

    free(t1);
    free(t2);
    free(t3);
    return o;
}

int GSC(byte* block, int ci) {  // todo: this needs to be completely rewritten
    ci += 1;
    float t1 = sin((float)(block[GSC_T1_SAM] * (ci + 1)) / 20);
    int pown = (float)block[GSC_T2_SAM] / 10;
    for (int i = pown, pown = ci; i > 0; i--) {
        pown *= ci;
        pown %= GSC_POWN_LIM;
    }
    float t2 = cos(pown);
    float t3 = (float)(block[GSC_T3_SAM] * (ci + 1)) / 100;
    float val = t1 * t2 * t3;
    byte valb[4];
    memcpy(valb, &val, 4);
    valb[1] = valb[1] << 1 | valb[1] >> 7;
    byte tmp = valb[0];
    valb[0] = valb[1];
    valb[1] = tmp;
    valb[0] = valb[3] << ci % 8 | valb[1] >> (8 - ci) % 8;
    int o;
    memcpy(&o, valb, 4);
    return MOD(o, GSC_SC_LIM);
}

void AddMod8(byte* o, uint n, byte* a, byte* b) {
    for (uint i = 0; i < n; i++)
        o[i] = (a[i] + b[i]) % 256;
}

static byte* ach2(const byte* data, uint n) {
    // alloc hash
    byte* block = malloc(ACH_2_BLOCK_SIZE);      // FREE ME
    byte* prevBlock = malloc(ACH_2_BLOCK_SIZE);  // DONT FREE ME!!
    // compute flag
    bool cf = true;
    // computation iteration
    int ci = 0;
    // alloc subblocks
    byte** subblocks = malloc(NUM_SUBBLOCKS * sizeof(byte*));  // FREE ME
    for (int i = 0; i < NUM_SUBBLOCKS; i++)
        subblocks[i] = malloc(ACH_2_SUBBLOCK_SIZE);  // FREE ME

    while (cf) {
#ifdef DEBUG
        printf("Iteration %d\n", ci);
#endif
        // read up to 32 bytes of data, or until the end
        memcpy(block, data + GRI(ci), MIN(ACH_2_BLOCK_SIZE, n - GRI(ci)));
        // pad if needed, and set the flag to false bc its the last round
        if (n - GRI(ci) < ACH_2_BLOCK_SIZE) {
            memset(block + n - GRI(ci), BLOCK_PAD, ACH_2_BLOCK_SIZE - (n - GRI(ci)));
            cf = false;
        }

        // generate dynamic seed
        dynamicSeed = GSC(block, ci);

#ifdef DEBUG
        printf("Dynamic seed: %d\n", dynamicSeed);
#endif

        // generate ciphered bytes key
        // xor with seed, and rotate 16 times
        byte* key = malloc(ACH_2_BLOCK_SIZE);  // FREE ME
        for (int i = 0; i < ACH_2_BLOCK_MAIN; i++)
            key[i] = block[i] ^ dynamicSeed;
        rotateArray(key, ACH_2_BLOCK_MAIN, 16);
        // append key to block
        memcpy(block + ACH_2_BLOCK_MAIN, key, ACH_2_BLOCK_MAIN);
        free(key);

        // xor with the previous block
        if (ci > 0)
            otpArray(block, ACH_2_BLOCK_SIZE, prevBlock);

        // seed block
        rotateArray(block, ACH_2_BLOCK_SIZE, block[BRS1]);
        blockSpike(block);
        rotateArray(block, ACH_2_BLOCK_SIZE, -block[BRS2]);
        blockJump(block);
        rotateArray(block, ACH_2_BLOCK_SIZE, block[BRS3]);

        // populate subblocks
        for (int i = 0; i < NUM_SUBBLOCKS; i++)
            memcpy(subblocks[i], block + i * ACH_2_SUBBLOCK_SIZE, ACH_2_SUBBLOCK_SIZE);

        // alloc target subblocks
        byte** targetSubblocks = malloc(NUM_SUBBLOCKS * sizeof(byte*));  // FREE ME
        for (int i = 0; i < NUM_SUBBLOCKS; i++)
            targetSubblocks[i] = malloc(ACH_2_SUBBLOCK_SIZE);  // FREE ME

        // compute mixing function results
        // M1 output
        byte* m1o = M1(subblocks[SE], ACH_2_SUBBLOCK_SIZE, subblocks[SF], subblocks[SG], dynamicSeed);  // FREE ME
        byte* m2o = M2(subblocks[SC], ACH_2_SUBBLOCK_SIZE, subblocks[SB], subblocks[SA], dynamicSeed);  // FREE ME
        byte* rm1o = RM1(subblocks[SH], ACH_2_SUBBLOCK_SIZE, dynamicSeed);                              // FREE ME
        byte* rm2o = RM2(subblocks[SG], ACH_2_SUBBLOCK_SIZE, dynamicSeed);                              // FREE ME

        // create dynamic seed array to feed it to the downrange compressor
        byte downrangeArray[ACH_2_SUBBLOCK_SIZE];
        for (int i = 0; i < ACH_2_SUBBLOCK_SIZE; i++)
            downrangeArray[i] = dynamicSeed;
        AddMod8(downrangeArray, ACH_2_SUBBLOCK_SIZE, downrangeArray, m1o);
        AddMod8(downrangeArray, ACH_2_SUBBLOCK_SIZE, downrangeArray, m2o);
        AddMod8(downrangeArray, ACH_2_SUBBLOCK_SIZE, downrangeArray, rm1o);
        AddMod8(downrangeArray, ACH_2_SUBBLOCK_SIZE, downrangeArray, rm2o);

        // assign target subblocks
        memcpy(targetSubblocks[SA], rm1o, ACH_2_SUBBLOCK_SIZE);
        memcpy(targetSubblocks[SB], rm2o, ACH_2_SUBBLOCK_SIZE);
        // add sc to m1o and assign that to rc
        // then add rc to sa and assign that to target sc
        byte rc[ACH_2_SUBBLOCK_SIZE];
        AddMod8(rc, ACH_2_SUBBLOCK_SIZE, m1o, subblocks[SC]);
        AddMod8(targetSubblocks[SC], ACH_2_SUBBLOCK_SIZE, rc, subblocks[SA]);
        // add sd to downrange array and assign it to rd
        // then add rd to sb and assign that to target sd
        byte rd[ACH_2_SUBBLOCK_SIZE];
        AddMod8(rd, ACH_2_SUBBLOCK_SIZE, downrangeArray, subblocks[SD]);
        AddMod8(targetSubblocks[SD], ACH_2_SUBBLOCK_SIZE, rd, subblocks[SB]);
        // add se to m2o and assign it to re
        byte re[ACH_2_SUBBLOCK_SIZE];
        AddMod8(re, ACH_2_SUBBLOCK_SIZE, m2o, subblocks[SE]);
        // set the rest
        memcpy(targetSubblocks[SE], rc, ACH_2_SUBBLOCK_SIZE);
        memcpy(targetSubblocks[SF], rd, ACH_2_SUBBLOCK_SIZE);
        memcpy(targetSubblocks[SG], re, ACH_2_SUBBLOCK_SIZE);
        // add sf to re and assign it to target sh
        AddMod8(targetSubblocks[SH], ACH_2_SUBBLOCK_SIZE, re, subblocks[SF]);

        // copy subblocks to previous block
        for (int i = 0; i < NUM_SUBBLOCKS; i++)
            memcpy(prevBlock + i * ACH_2_SUBBLOCK_SIZE, targetSubblocks[i], ACH_2_SUBBLOCK_SIZE);

        // cleanup
        for (int i = 0; i < NUM_SUBBLOCKS; i++)
            free(targetSubblocks[i]);
        free(targetSubblocks);
        free(m1o);
        free(m2o);
        free(rm1o);
        free(rm2o);
    }
#ifdef DEBUG
    printf("Done hashing.\n");
#endif

    // cleanup
    for (int i = 0; i < NUM_SUBBLOCKS; i++)
        free(subblocks[i]);
    free(subblocks);
    free(block);

    return prevBlock;  // todo: test
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