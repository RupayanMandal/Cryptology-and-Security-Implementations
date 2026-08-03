#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <x86intrin.h>

#include "chacha.h"

typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;


#define iter 100000ULL
#define min_input_bytes 64

int main(void)
{

    FILE *fp_cpb = fopen("cycles_per_byte.csv", "w");
    if (!fp_cpb) {
        fprintf(stderr, "cycles_per_byte.csv\n");
        return 1;
    }
    fprintf(fp_cpb, "Bytes,Cycles per Byte\n");
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

    uint64_t total_bytes=0;

    /*For Benchmark*/
    struct timespec start_time, end_time;
    uint64_t total_time_ns=0;
  
    uint32_t cpu_id;
    uint64_t start_cycles, end_cycles;
    uint64_t total_cycles=0;

    /* Seed the random number generator only once */
    srand((unsigned)time(NULL));

    for (unsigned int i = 0; i < iter; i++)
    {
        /* Random plaintext length */
        size_t plaintext_size = (rand() % file_size) + 1;
        if (plaintext_size<min_input_bytes) plaintext_size=min_input_bytes;

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

        total_bytes+=bytesRead;

        /* Read random key */
        fseek(fp, key_start_pos, SEEK_SET);
        fread(key, 1, 32, fp);

        /* Read random nonce */
        fseek(fp, nonce_start_pos, SEEK_SET);
        fread(nonce, 1, 12, fp);

        /* Measure encryption */
        clock_gettime(CLOCK_MONOTONIC, &start_time);

        start_cycles = __rdtscp(&cpu_id);

        chacha20_encrypt(key,
                         nonce,
                         1,
                         plaintext,
                         ciphertext,
                         bytesRead);

        end_cycles = __rdtscp(&cpu_id);
        
        clock_gettime(CLOCK_MONOTONIC, &end_time);


        total_cycles+=end_cycles-start_cycles;
        
        fprintf( fp_cpb, "%zu,%14.4f\n", bytesRead, (double)(end_cycles - start_cycles) / bytesRead);
        
        total_time_ns += (uint64_t)(end_time.tv_sec - start_time.tv_sec) * 1000000000ULL + (uint64_t)(end_time.tv_nsec - start_time.tv_nsec);
        
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
    fclose(fp_cpb);

    // Cycles Per Iteration
   double cycles_per_iteration = (double)total_cycles / iter;
   //Cycles Per Byte
   double cycles_per_byte = (double) total_cycles / total_bytes;
  

    // average execution time for 64 bytes long message text
    double ns_per_iteration = (double) total_time_ns / iter;
    // Time per byte
    double ns_per_byte = (double) total_time_ns / ( total_bytes );
  
    printf("\n");
    printf("-------------------------------------------------------------------------------------------");
    printf("\niter            : %llu\n", iter);
    printf("\nTotal Bytes            : %lu\n", total_bytes);
    printf("\nTotal Cycles           : %lu\n", total_cycles);
    printf("\nCycles / Iteration     : %.2f\n", cycles_per_iteration);
    printf("\nCycles / Byte          : %.4f\n", cycles_per_byte);
    printf("\nTotal Time             : %lu ns\n", total_time_ns);
    printf("\nAverage Time / Iteration : %.2f ns\n", ns_per_iteration);
    printf("\nAverage Time / Byte    : %.2f ns\n", ns_per_byte);  
    printf("\nCPU frequency          : %.2f GHz",(double)total_cycles / total_time_ns);  // multiply 10^9 for ns -> s and divide by 10^9 to get GHz
         
    printf("\n-------------------------------------------------------------------------------------------");


    /*Plotting Bytes vs Cycles per byte to get any anomaly*/
    FILE *gp = popen("gnuplot -persist", "w");

    if (!gp) {
        fprintf(stderr, "could not open gnuplot (is it installed?)\n");
        return 1;
    }

    fprintf(gp, "set datafile separator ','\n");
    fprintf(gp, "set key autotitle columnhead\n");
    fprintf(gp, "set terminal windows\n");
    fprintf(gp, "set mouse\n");
    fprintf(gp, "set grid\n");
    fprintf(gp, "set key outside\n");
    fprintf(gp, "set xlabel 'Input Size (Bytes)'\n");
    fprintf(gp, "set ylabel 'Cycles per Byte'\n");
    fprintf(gp, "set title 'ChaCha20 Benchmark'\n");
    fprintf(gp, "plot 'cycles_per_byte.csv' using 1:2 with points pt 7 ps 0.25 title 'CPB'\n");

    /* fprintf(gp, "set datafile separator ','\n");
    fprintf(gp, "set key autotitle columnhead\n");
    fprintf(gp, "set multiplot layout 2,1\n");

    fprintf(gp, "set xlabel 'Bytes'\n");
    fprintf(gp, "set ylabel 'Cycles per byte'\n");
    fprintf(gp, "set title 'Bytes v/s Cycles per Byte'\n");

    fprintf(gp,
            "plot 'cycles_per_byte.csv' using 1:2 with dots title 'CPB'\n "); */

    
    //fprintf(gp, "unset multiplot\n");
    fflush(gp);
    pclose(gp);



    return 0;



}

/*gcc test_file.c chacha.c  -o test.exe

.\test.exe

*/