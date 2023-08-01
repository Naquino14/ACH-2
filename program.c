#include <openssl/rand.h>
#include <stdio.h>

#define DEBUG
#include "ACH-2.h"

int main(void) {
    printf("Hello World!\n");
    // generate 64 bytes of random data for testing
    int n = 64;
    uint8_t data[n];
    for (int i = 0; i < n; i++) {
        data[i] = rand() % 256;
    }

    // print the data
    printf("Data: ");
    for (int i = 0; i < n; i++) {
        printf("%02x ", data[i]);
    }
    printf("\n");

    blockSpike(data);

    // print the data
    printf("Data: ");
    for (int i = 0; i < n; i++) {
        printf("%02x ", data[i]);
    }
    printf("\n");
    return 0;
}