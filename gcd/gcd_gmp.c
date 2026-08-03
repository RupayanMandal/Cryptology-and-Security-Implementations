#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <gmp.h>


 #define ITERATIONS 1000000
 #define MAX_VALUE 1000000


int main()
{
    clock_t start, end;
    mpz_t a, b, gcd;

    /* Initialize GMP integers */
    mpz_init(a);
    mpz_init(b);
    mpz_init(gcd);

    srand((unsigned)time(NULL));

    start = clock();
    for (unsigned int i = 0; i < ITERATIONS; i++)
    {
        mpz_set_ui(a,(rand() % MAX_VALUE) + 1);   // Random number in [1, MAX_VALUE]
        mpz_set_ui(b,(rand() % MAX_VALUE) + 1);

        /* Read input */
        //printf("Enter the first integer: ");
        //gmp_scanf("%Zd", a);

        mpz_gcd(gcd, a, b);
    }

    end = clock();
    mpz_clear(a);
    mpz_clear(b);
    mpz_clear(gcd);

    double total_time_ns = ((double)(end - start) * 1e9) / CLOCKS_PER_SEC;
    double avg_time_ns = total_time_ns / ITERATIONS;

    printf("Iterations          : %u\n", ITERATIONS);
    gmp_printf("Last GCD(%Zd,%Zd)            : %Zd\n", a,b,gcd);
    printf("Total Time          : %.2f ns\n", total_time_ns);
    printf("Average Time / GCD  : %.2f ns\n", avg_time_ns);

    
    return 0;
}

/*gcc gcd_gmp.c -lgmp -o gcd_gmp*/
