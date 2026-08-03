//#include "chacha20.h"
#include <string.h>
#include <stdint.h>
#include <stddef.h>


typedef unsigned char uint8_t;
typedef unsigned int  uint32_t;

#define ROTL32(v,n) (((v) << (n)) | ((v) >> (32-(n))))




#define quarter_round(a,b,c,d) \
    a += b; d ^= a; d = ROTL32(d,16); \
    c += d; b ^= c; b = ROTL32(b,12); \
    a += b; d ^= a; d = ROTL32(d,8); \
    c += d; b ^= c; b = ROTL32(b,7);

static uint32_t load32(const uint8_t *x) //converts four bytes into one unsigned 32-bit integer using little-endian ordering.
{
    return ((uint32_t)x[0]) |
           ((uint32_t)x[1] << 8) |
           ((uint32_t)x[2] << 16) |
           ((uint32_t)x[3] << 24);
}

static void store32(uint8_t *x, uint32_t v) //converts one unsigned 32-bit integer using little-endian ordering into four bytes 
{
    x[0] = v;
    x[1] = v >> 8;
    x[2] = v >> 16;
    x[3] = v >> 24;
}

/*
static void quarter_round(uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d)
{
    *a += *b;    *d ^= *a;    *d = ROTL32(*d,16);

    *c += *d;    *b ^= *c;    *b = ROTL32(*b,12);

    *a += *b;    *d ^= *a;    *d = ROTL32(*d,8);

    *c += *d;    *b ^= *c;    *b = ROTL32(*b,7);
}
*/



static void chacha20_block( uint32_t state[16],  uint8_t keystream[64])
{
    //uint32_t x[16];
    int i;

    //memcpy(x, state, sizeof(x));
    uint32_t x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15;
    for(i = 0; i < 10; i++)
    {

         x0 = state[0];
         x1 = state[1];
         x2 = state[2];
         x3 = state[3];
         x4 = state[4];
         x5 = state[5];
         x6 = state[6];
         x7 = state[7];
         x8 = state[8];
         x9 = state[9];
         x10 = state[10];
         x11 = state[11];
         x12 = state[12];
         x13 = state[13];
         x14 = state[14];
         x15 = state[15];
        /* Column rounds */
        quarter_round(x0, x4, x8,  x12);
        quarter_round(x1, x5, x9,  x13);
        quarter_round(x2, x6, x10, x14);
        quarter_round(x3, x7, x11, x15);

        /* Diagonal rounds */
        quarter_round(x0, x5, x10, x15);
        quarter_round(x1, x6, x11, x12);
        quarter_round(x2, x7, x8,  x13);
        quarter_round(x3, x4, x9,  x14);
    }

    x0 += state[0];
    x1 += state[1];
    x2 += state[2];
    x3 += state[3];
    x4 += state[4];
    x5 += state[5];
    x6 += state[6];
    x7 += state[7];
    x8 += state[8];
    x9 += state[9];
    x10 += state[10];
    x11 += state[11];
    x12 += state[12];
    x13 += state[13];
    x14 += state[14];
    x15 += state[15];
        
    store32(keystream + 0,  x0);
    store32(keystream + 4,  x1);
    store32(keystream + 8,  x2);
    store32(keystream + 12, x3);
    store32(keystream + 16, x4);
    store32(keystream + 20, x5);
    store32(keystream + 24, x6);
    store32(keystream + 28, x7);
    store32(keystream + 32, x8);
    store32(keystream + 36, x9);
    store32(keystream + 40, x10);
    store32(keystream + 44, x11);
    store32(keystream + 48, x12);
    store32(keystream + 52, x13);
    store32(keystream + 56, x14);
    store32(keystream + 60, x15);
}

void chacha20_encrypt(
    const uint8_t key[32],
    const uint8_t nonce[12],
    uint32_t counter,
    const uint8_t *input,
    uint8_t *output,
    size_t length)
{
    uint32_t state[16];
    uint8_t keystream[64];
    size_t i;

    /* Constants -> "expand 32-byte k" */
    state[0] = 0x61707865;
    state[1] = 0x3320646e;
    state[2] = 0x79622d32;
    state[3] = 0x6b206574;

    /* Key */
    for(i = 0; i < 8; i++)
        state[4+i] = load32(key + 4*i);

    /* Counter */
    state[12] = counter;

    /* Nonce */
    state[13] = load32(nonce);
    state[14] = load32(nonce + 4);
    state[15] = load32(nonce + 8);

    while(length > 0)
    {
        size_t block;

        chacha20_block(state, keystream);

        block = (length < 64) ? length : 64;

        for(i = 0; i < block; i++)
            output[i] = input[i] ^ keystream[i];

        input += block;
        output += block;
        length -= block;

        state[12]++;
    }
}