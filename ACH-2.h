#ifndef ACH2_H
#define ACH2_H

#ifndef _STDBOOL_H
#include <stdbool.h>
#endif  // !_STDBOOL_H

#ifndef _STDINT_H
#include <stdint.h>
#endif  // !_STDINT_H

#ifndef _STDLIB_H
#include <stdlib.h>
#endif  // !_STDLIB_H

#ifndef _MATH_H
#include <math.h>
#endif  // !_MATH_H

#ifndef _STRING_H
#include <string.h>
#endif  // !_STRING_H

#define ACH_2_BLOCK_SIZE 64
#define ACH_2_BLOCK_MAIN 32
#define ACH_2_SUBBLOCK_SIZE 8

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// @brief Restore the dynamic seeds
/// @details Should you not need to hash seprate data back-to-back,
/// you can call this function to restore the dynamic seeds.
/// Resets the following:
/// RMC1C(1-3)
/// RMC2C(1-3)
void restoreDynamicSeeds();

/// @brief Compute a hash with ACH-2
/// @param data The data to hash
/// @param n The length of the data
/// @return The hash
uint8_t* computeHash(const uint8_t* data, unsigned int n);

/// @brief Compute a hash with ACH-2, without restoring the dynamic seeds.
/// @details Use this method when you need to hash data back-to-back,
/// with the order of the data being hashed being important.
/// @param data The data to hash
/// @param n The length of the data
/// @return The hash
uint8_t* chainComputeHash(const uint8_t* data, unsigned int n);

#ifdef DEBUG
void rotateArray(uint8_t* array, unsigned int n, int amount);
void blockJump(uint8_t* block);
void blockSpike(uint8_t* block);
uint8_t* M1(uint8_t* a, unsigned int n, uint8_t* b, uint8_t* c, int sc);
uint8_t* M2(uint8_t* a, unsigned int n, uint8_t* b, uint8_t* c, int sc);
uint8_t* RM1(uint8_t* a, unsigned int n, int sc);
uint8_t* RM2(uint8_t* a, unsigned int n, int sc);
int GSC(uint8_t* block, int ci);
#endif  // DEBUG

#endif  // !ACH2_H

#ifdef __cplusplus
}
#endif  // __cplusplus