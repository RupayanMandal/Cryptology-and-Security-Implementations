#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ITERATIONS 1000000
#define MAX_VALUE 1000000

unsigned int gcd(unsigned int a, unsigned int b)
{
    while (a != 0 && b != 0)
    {
        if (a > b)
            a = a % b;
        else
            b = b % a;
    }
    return (a ^ b);
}

int main(void)
{
    clock_t start, end;
    unsigned int a, b, g = 0;

    srand((unsigned)time(NULL));

    start = clock();

    for (unsigned int i = 0; i < ITERATIONS; i++)
    {
        a = (rand() % MAX_VALUE) + 1;   // Random number in [1, MAX_VALUE]
        b = (rand() % MAX_VALUE) + 1;

        g = gcd(a, b);
    }

    end = clock();

    double total_time_ns = ((double)(end - start) * 1e9) / CLOCKS_PER_SEC;
    double avg_time_ns = total_time_ns / ITERATIONS;

    printf("Iterations          : %u\n", ITERATIONS);
    printf("Last GCD(%u,%u)            : %u\n", a,b);
    printf("Total Time          : %.2f ns\n", total_time_ns);
    printf("Average Time / GCD  : %.2f ns\n", avg_time_ns);

    return 0;
}

/*gcc gcd.c -o gcd_gmp*/