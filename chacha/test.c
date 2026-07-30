#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <x86intrin.h> 

#include "chacha.h"

int main(void)
{
    uint8_t key[32] = {
        0x10, 0x21, 0x32, 0x43,
        0x54, 0x65, 0x76, 0x87,
        0x98, 0xA9, 0xBA, 0xCB,
        0xDC, 0xED, 0xFE, 0x0F,
        0x1F, 0x2E, 0x3D, 0x4C,
        0x5B, 0x6A, 0x79, 0x88,
        0x97, 0xA6, 0xB5, 0xC4,
        0xD3, 0xE2, 0xF1, 0x00};
    uint8_t nonce[12] = { 
        0xAB, 0xCD, 0xEF, 0x01,
        0x23, 0x45, 0x67, 0x89,
        0x10, 0x32, 0x54, 0x76};

    char plaintext[] = "Hello ChaCha20!";
    size_t len = strlen(plaintext);

    uint8_t ciphertext[64];
    uint8_t recovered[64];

    chacha20_encrypt(key, nonce, 1,
                     (uint8_t *)plaintext,
                     ciphertext,
                     len);

    chacha20_encrypt(key, nonce, 1,
                     ciphertext,
                     recovered,
                     len);

    recovered[len] = '\0';

    printf("Plaintext : %s\n", plaintext);
    printf("Recovered : %s\n", recovered);

    return 0;
}