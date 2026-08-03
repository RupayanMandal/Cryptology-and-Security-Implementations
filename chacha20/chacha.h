#ifndef CHACHA20_H
#define CHACHA20_H

#include <stdint.h>
#include <stddef.h>

/* Generate one 64-byte ChaCha20 keystream block */
void chacha20_block(
    uint32_t state[16],
    uint8_t keystream[64]);

/* Encrypt or decrypt data using ChaCha20 */
void chacha20_encrypt(
    const uint8_t key[32],
    const uint8_t nonce[12],
    uint32_t counter,
    const uint8_t *input,
    uint8_t *output,
    size_t length);

#endif /* CHACHA20_H */