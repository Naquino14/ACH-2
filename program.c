#include <openssl/rand.h>
#include <stdio.h>

#define DEBUG
#include "ACH-2.h"

int main(void) {
    unsigned char data[64];
    for (int i = 0; i < 64; i++)
        data[i] = rand() % 255;
    data[4] = 63;
    for (int i = 0; i < 1025; i++) {
        int seed = GSC(data, i);
        printf("%d\n", seed);
    }
    return 0;
}