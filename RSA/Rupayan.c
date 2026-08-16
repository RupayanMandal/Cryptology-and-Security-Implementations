#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<gmp.h>
#include<time.h>
#include <x86intrin.h>
#define BENCH_ITERS    1000000
#define PRIME_BITS 1024
#define MR_ROUNDS 25
static inline void serialize_cpuid(void) {
    unsigned int eax, ebx, ecx, edx;
    // Call CPUID with EAX=0 (leaves standard vendor string)
    __asm__ __volatile__ (
        "cpuid"
        : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
        : "a"(0)
        : "memory" // Memory clobber prevents compiler from reordering reads/writes across this block
    );
}

 struct RSA {
    mpz_t p;
    mpz_t q;
    mpz_t N;
    mpz_t p1;
    mpz_t q1;
    mpz_t phi;
    mpz_t e;
    mpz_t d;
};

typedef struct RSA RSA_par;

static inline int rdrand64(uint64_t *value)
{
    unsigned char ok;
    uint64_t tmp;

    __asm__ volatile(
        ".byte 0x48, 0x0F, 0xC7, 0xF0\n\t"  // RDRAND RAX
        "setc %1"
        : "=a"(tmp), "=qm"(ok)
        :
        : "cc"
    );

    *value = tmp;
    return ok;          // 1 = success, 0 = retry
}

void generate_prime(mpz_t p, gmp_randstate_t state)
{
    uint64_t value;
    mpz_t temp1;
    mpz_init(temp1);
    do {
        int i;
        mpz_set_ui(p,0);
        for(i=0;i<PRIME_BITS/64;i++)
        {
            //while(!rdrand64(&value));
            rdrand64(&value);
            mpz_import(temp1, 1, 1, sizeof(value), 0, 0, &value);
            mpz_mul_2exp(p, p, 64);
            mpz_ior(p, p, temp1);

        }
        // Generate a random 1024-bit integer
        //mpz_urandomb(p, state, PRIME_BITS);

        // Ensure it is exactly 1024 bits
        mpz_setbit(p, PRIME_BITS - 1);

        // Ensure it is odd
        mpz_setbit(p, 0);

    } while (mpz_probab_prime_p(p, MR_ROUNDS) == 0);
}

void generate_par_RSA(RSA_par *key){
	
	mpz_inits(key->N, key->p, key->q, NULL);

	

	mpz_inits(key->phi, key->p1, key->q1, NULL);

	gmp_randstate_t state;
    	gmp_randinit_default(state);
    	
    	gmp_randseed_ui(state, (unsigned long)time(NULL));

    
    	generate_prime(key->p, state);

    
    	do {
        	generate_prime(key->q, state);
    	} while (mpz_cmp(key->p, key->q) == 0); //(p != q)

    //gmp_printf("Prime p:\n%Zd\n\n", p);
    //gmp_printf("Prime q:\n%Zd\n", q);
    	mpz_mul(key->N, key->p, key->q);   
    	mpz_sub_ui(key->p1, key->p, 1);   // p1 = p - 1
    	mpz_sub_ui(key->q1, key->q, 1);   // q1 = q - 1
    	mpz_mul(key->phi, key->p1, key->q1);
    	
    	
    	gmp_randclear(state);
    	
	mpz_init_set_ui(key->e, 65537);
	
	mpz_t g;

	mpz_init(g);

	mpz_gcd(g, key->e, key->phi);

	
	mpz_init(key->d);

	mpz_invert(key->d, key->e, key->phi);
        printf("this is d");
        //return d;
    	
}

void rsa_encrypt(mpz_t ciphertext,mpz_t message, RSA_par *key)
{
    

    mpz_powm(ciphertext, message, key->e, key->N);
}

void rsa_decrypt(mpz_t message, mpz_t ciphertext, RSA_par *key)
{
    mpz_powm(message, ciphertext, key->d, key->N);
}

int main(){
        unsigned int aux_start, aux_end;
    	struct timespec start_time, end_time;
    	serialize_cpuid();
    	uint64_t start = __rdtscp(&aux_start);
    	clock_gettime(CLOCK_MONOTONIC, &start_time);
    
	//RSA_par *key;
	RSA_par *key = malloc(sizeof(RSA_par));
	generate_par_RSA(key);
	//mpz_t message;
	//mpz_t ciphertext;
	mpz_t message, ciphertext, decrypted;
	
	mpz_inits(message, ciphertext, decrypted, NULL);

	mpz_set_ui(message, 42);   // or mpz_set_str(message, "12345", 10);
        for (int i = 0; i < BENCH_ITERS; i++) {
		rsa_encrypt(ciphertext, message, key);
	}
	clock_gettime(CLOCK_MONOTONIC, &end_time);
    	uint64_t end = __rdtscp(&aux_end);
 
    	uint64_t total_cycles = end - start;
    	uint64_t total_time_ns = (uint64_t)(end_time.tv_sec - start_time.tv_sec) * 1000000000ULL + (uint64_t)(end_time.tv_nsec - start_time.tv_nsec);
	double cycles_per_iteration = (double)total_cycles / BENCH_ITERS;
	double ns_per_iteration = (double) total_time_ns / BENCH_ITERS;
	
	printf("Total cycles:        %llu\n", (unsigned long long)total_cycles);
    	printf("Iterations:          %d\n", BENCH_ITERS);
    	printf("Cycles per iteration:    %.2f\n", cycles_per_iteration);
    	printf("\n");
    	printf("Total Time            : %lu ns\n", total_time_ns);
    	printf("Average Time / Iteration     : %.2f ns\n", ns_per_iteration);
	
	//gmp_printf("Ciphertext:\n%Zd\n\n", ciphertext);

	//rsa_decrypt(decrypted, ciphertext, key);
	//gmp_printf("Decrypted:\n%Zd\n\n", decrypted);

	//if (mpz_cmp(message, decrypted) == 0)
    	//printf("Round-trip OK\n");

	//mpz_clears(message, ciphertext, decrypted, NULL);
	
	
    

}