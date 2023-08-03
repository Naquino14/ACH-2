#include <openssl/rand.h>
#include <stdio.h>

#define DEBUG
#include "ACH-2.h"

int main(void) {
    printf("Hello World!\n");
    unsigned char data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    printf("data: ");
    for (int i = 0; i < 10; i++)
        printf("%d ", data[i]);
    int a = 11;
    printf("\nRotating data %d times...\n", a);
    rotateArray(data, 10, a);
    printf("data: ");
    for (int i = 0; i < 10; i++)
        printf("%d ", data[i]);
    printf("\n");
    return 0;
}