#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

void FFT(double complex P[], unsigned int n, double complex y[])
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

    double complex omega =
        cos(angle) + I * sin(angle);

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

int main()
{
    double complex P[4] = {1,2,3,4};

    unsigned int n = 4;

    double complex y[4];

    FFT(P, n, y);

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

    return 0;
}