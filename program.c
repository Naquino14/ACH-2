#include <stdio.h>
#include <time.h>

#define DEBUG
#include "ACH-2.h"

int main(void) {
    for (;;) {
        printf("Enter a string to be hashed: ");
        char buf[1024];
        if (!fgets(buf, 1024, stdin))
            break;
        buf[strcspn(buf, "\n")] = 0;
        printf("Hashing \"%s\"...\n", buf);

        clock_t start = clock(), diff;
        unsigned char* hash = computeHash((const unsigned char*)buf, strlen(buf));
        diff = clock() - start;
        int msec = diff * 1000 / CLOCKS_PER_SEC;
        printf("Hash: \n");
        for (int i = 0; i < ACH_2_BLOCK_SIZE; i++)
            printf("%02x", (unsigned char)hash[i]);
        printf("\n");
        printf("Time: %d milliseconds\n", msec);
        free(hash);
    }
    return 0;
}