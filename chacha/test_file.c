#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <x86intrin.h>

#include "chacha.h"

int main(void)
{
    uint64_t start, end;
    unsigned long long total_cycles_per_byte = 0;

    FILE *fp = fopen("input.txt", "rb");
    if (fp == NULL)
    {
        printf("Error opening input.txt\n");
        return 1;
    }

    /* Get file size */
    fseek(fp, 0, SEEK_END);
    size_t file_size = ftell(fp);
    rewind(fp);

    if (file_size < 32)
    {
        printf("Input file is too small.\n");
        fclose(fp);
        return 1;
    }

    uint8_t key[32];
    uint8_t nonce[12];

    const unsigned int iter = 100000;

    /* Seed the random number generator only once */
    srand((unsigned)time(NULL));

    for (unsigned int i = 0; i < iter; i++)
    {
        /* Random plaintext length */
        size_t plaintext_size = (rand() % file_size) + 1;

        /* Random positions for key and nonce */
        size_t key_start_pos = rand() % (file_size - 32 + 1);
        size_t nonce_start_pos = rand() % (file_size - 12 + 1);

        uint8_t *plaintext = malloc(plaintext_size);
        uint8_t *ciphertext = malloc(plaintext_size);
        uint8_t *recovered = malloc(plaintext_size);

        if (plaintext == NULL ||
            ciphertext == NULL ||
            recovered == NULL)
        {
            printf("Memory allocation failed.\n");

            free(plaintext);
            free(ciphertext);
            free(recovered);

            fclose(fp);
            return 1;
        }

        /* Read plaintext from beginning of file */
        rewind(fp);

        size_t bytesRead = fread(plaintext, 1, plaintext_size, fp);

        if (bytesRead == 0)
        {
            printf("Nothing read from file.\n");

            free(plaintext);
            free(ciphertext);
            free(recovered);

            break;
        }

        /* Read random key */
        fseek(fp, key_start_pos, SEEK_SET);
        fread(key, 1, 32, fp);

        /* Read random nonce */
        fseek(fp, nonce_start_pos, SEEK_SET);
        fread(nonce, 1, 12, fp);

        /* Measure encryption */
        start = __rdtsc();

        chacha20_encrypt(key,
                         nonce,
                         1,
                         plaintext,
                         ciphertext,
                         bytesRead);

        end = __rdtsc();

        total_cycles_per_byte += (end - start) / bytesRead;
        printf("Clock Cycles per Byte = %.2f\n",
           (double)(end - start) / bytesRead); 
        /* Decrypt */
        chacha20_encrypt(key,
                         nonce,
                         1,
                         ciphertext,
                         recovered,
                         bytesRead);

        /* Verify correctness */
        if (memcmp(plaintext, recovered, bytesRead) != 0)
        {
            printf("Encryption/Decryption failed on iteration %u\n", i + 1);

            free(plaintext);
            free(ciphertext);
            free(recovered);

            fclose(fp);
            return 1;
        }

        free(plaintext);
        free(ciphertext);
        free(recovered);
    }

    fclose(fp);

    printf("Average Clock Cycles per Byte = %.2f\n",
           (double)total_cycles_per_byte / iter);

    return 0;
}

/*gcc test_file.c chacha.c  -o test.exe

.\test.exe

*/