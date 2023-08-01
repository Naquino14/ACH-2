#ifndef ACH2_H
#define ACH2_H

#ifndef _STDBOOL_H
#include <stdbool.h>
#endif  // !_STDBOOL_H

#ifndef _STDINT_H
#include <stdint.h>
#endif  // !_STDINT_H

#define ACH_2_BLOCK_SIZE 64

/// @brief Compute a hash with ACH-2
/// @param data The data to hash
/// @param n The length of the data
/// @return The hash
uint8_t* computeHash(const uint8_t* data, unsigned int n);

#ifdef DEBUG
void blockJump(uint8_t* block);
void blockSpike(uint8_t* block);
#endif  // DEBUG

#endif  // !ACH2_H
