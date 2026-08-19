#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <complex.h>


#define ITERATIONS 10000


long long elapsed_ns(struct timespec start,
                     struct timespec end)
{
    return (end.tv_sec - start.tv_sec) * 1000000000LL
         + (end.tv_nsec - start.tv_nsec);
}
void FFT(double complex* P, unsigned int n, double complex* y)
{
    if (n == 1)
    {
        y[0] = P[0];
        return;
    }

    double complex Pe[n / 2];
    double complex Po[n / 2];

    double complex ye[n / 2];
    double complex yo[n / 2];

    // Split into even and odd coefficients
    for (unsigned int i = 0; i < n / 2; i++)
    {
        Pe[i] = P[2 * i];
        Po[i] = P[2 * i + 1];
    }

    // Recursive FFT
    FFT(Pe, n / 2, ye);
    FFT(Po, n / 2, yo);

    // omega = e^(2*pi*i/n)
    double angle = 2.0 * M_PI / n;

    double complex omega =cos(angle) + I * sin(angle);

    // Start with omega^0
    double complex temp = 1.0;

    // Combine
    for (unsigned int j = 0; j < n / 2; j++)
    {
        y[j] =
            ye[j] + temp * yo[j];

        y[j + n / 2] =
            ye[j] - temp * yo[j];

        // temp = omega^(j+1)
        temp *= omega;
    }
}

void populate_array(double complex* P, unsigned int n)
{
    int i; 
    for (int i = 0; i<n; i++) {
        P[i] = (rand() % 21) - 10;  // Range: -10 to 10

    }
}    

int main()
{
    //clock_t start, end;
    srand(time(NULL));

    struct timespec start, end;
    long long total_time;

    unsigned int pol_degree[]={64,128,256,512,1024,2048,4096,8192,16384};
    unsigned int pol_degree_len=sizeof(pol_degree) / sizeof(pol_degree[0]);
    
    int i,j;
    for(i=0;i<pol_degree_len;i++)
    {
        double complex* P=(double complex*)malloc((pol_degree[i]+1)*sizeof(double complex));
        double complex* y=(double complex*)malloc((pol_degree[i]+1)*sizeof(double complex));
        populate_array(P,pol_degree[i]+1);
        
        volatile double checksum = 0.0;
        //warmup
        FFT(P, pol_degree[i]+1, y);
        
        total_time=0;
        clock_gettime(CLOCK_MONOTONIC, &start);
        for ( j = 0; j < ITERATIONS; j++)
        {
            FFT(P, pol_degree[i]+1, y);
        }    
        clock_gettime(CLOCK_MONOTONIC, &end);
            
        total_time+= elapsed_ns(start,end);
        double nlogn=pol_degree[i] * log2(pol_degree[i]);
        unsigned int n2=pow(pol_degree[i],2);
        double avg_time=total_time/ITERATIONS;
        printf("\nPolynomial Degree         : %d\n", pol_degree[i]);
        printf("Total Time                : %lld ns\n", total_time);
        printf("Average Time(Tn)          : %.2f ns\n",avg_time);
        printf("nlogn                     : %.2f\n",nlogn);
        printf("Multiplier(Tn/nlogn)      : %.2f\n",(double)avg_time/nlogn);
        printf("Multiplier(Tn/n^2)        : %.2f\n",(double)avg_time/n2);
        free(P);
        free(y);
    }

    /*
    double complex P[4] = {1,2,3,4};

    unsigned int n = 4;

    double complex y[4];

    start=clock();
    FFT(P, n, y);
    end=clock();
    double total_time_ns = ((double)(end - start) * 1e9) / CLOCKS_PER_SEC;
    printf("Total Time          : %.2f ns\n", total_time_ns);
    

    printf("FFT:\n");

    for (unsigned int i = 0; i < n; i++)
    {
        printf(
            "y[%d] = %.6f %+.6fi\n",
            i,
            creal(y[i]),
            cimag(y[i])
        );
    }
    */
    return 0;
}

//gcc -o3 -march=native -lgmp FFT.c -o test
