#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <x86intrin.h>

#define MAX_MSG_LEN 1000
#define STEPS 100



/* ---- cycle counting -------------------------------------------------*/

static inline unsigned long long start_tsc(void) 
{
    _mm_lfence();//It prevents the CPU out-of-order execution engine from speculatively executing instructions across the timing boundary, ensuring precise timing measurements.
    return __rdtsc();
}
static inline unsigned long long stop_tsc(void) 
{
    unsigned int aux;
    unsigned long long tsc = __rdtscp(&aux);
    _mm_lfence();
    return tsc;
}

void init_S(unsigned char* S)
{
    for (int i = 0; i < 256; i++)
        S[i] = i;
}
/*KEY SCHEDULING ALGORITHM (KSA)*/ 
double KSA(unsigned char* S,unsigned char key[], size_t key_len )
{
    int i=0;
    int j=0;
    unsigned char temp;
    unsigned char K[256];
    for(int i=0;i<256;i++)
    {
    K[i]=key[i%key_len];
    }
    unsigned long long start_cycle=start_tsc();
    for(i=0;i<256;i++)
    {
        j=(j+S[i]+K[i]) &0XFF;    // &0XFF -> mod 256
        
        temp=S[i];
        S[i]=S[j];
        S[j]=temp;  
        /*used 3 assignments instead of 3XORS as it is faster 
        in modern cpu 3 dependant XORs prevent parallelism */
    }
    unsigned long long end_cycle=stop_tsc();
    return (end_cycle-start_cycle);
}

double PRGA(unsigned char* S,unsigned char* keystream,size_t msglen)
{
    int i=0;
    int j=0;
    int k;
    unsigned char temp;

    unsigned long long start_cycle=start_tsc();
    for (k=0;k<msglen;k++)
    {
        i=(i+1) &0XFF;
        j=(j+S[i]) &0XFF;
        temp=S[i];
        S[i]=S[j];
        S[j]=temp;  
        keystream[k]=S[(S[i]+S[j]) &0XFF];
        //printf("\n%d 0x%X",k, keystream[k]);
    }
    unsigned long long end_cycle=stop_tsc();
    
    return (end_cycle-start_cycle)/(double)msglen;

}

int gen_msg(unsigned char*msg,size_t msg_len)
{
    for (int i = 0; i <msg_len; i++)
    {
        msg[i] = i & 0xFF;
    }
}

int rc4(unsigned char* S, unsigned char key[],size_t keylen,unsigned char* msg, size_t msg_len)
{
  unsigned long long start_cycle, end_cycle;
 
    double total_ksa_cycles, cycles_per_byte_prga;

    
    size_t msglen = msg_len;
    printf("\nKEYLEN %zu",keylen);

    unsigned char* keystream=(unsigned char*)malloc(msglen*sizeof(unsigned char));
    unsigned char* ciphertext=(unsigned char*)malloc(msglen*sizeof(unsigned char));
    unsigned char* plaintext=(unsigned char*)malloc(msglen*sizeof(unsigned char) +1);

    if (!keystream || !ciphertext || !plaintext) {
    fprintf(stderr, "Memory allocation failed\n");
    return 1;}
    
    int i;
    for (i=0;i<256;i++)
    {
        S[i]=i;
    }

    unsigned int temp;
    unsigned int j=0;

    /*KEY SCHEDULING ALGORITHM (KSA)*/         
    KSA(S,key,keylen);  // warmup
    init_S(S);
    total_ksa_cycles=KSA(S,key,keylen); //for benchmarking
    
    /*PSEUDO-RANDOM GENERATION ALGORITHM (PRGA)*/
    unsigned char* tempS=(unsigned char*)malloc(256*sizeof(unsigned char));
    memcpy(tempS,S,256);
    PRGA(S,keystream,msglen); //warmup
    PRGA(S,keystream,msglen);
    memcpy(S,tempS,256);
    cycles_per_byte_prga=PRGA(S,keystream,msglen); //for benchmarking
    free(tempS);
    int k;
    /* for(k=0;k<msglen;k++)
    {
        printf("\n%d 0x%X",k,keystream[k]);
    } */
    
    for(k=0;k<msglen;k++)
    {
        ciphertext[k]=keystream[k]^msg[k];
    }
    
    printf("\nNumber of Encrypted bytes: %d",k);

    
    
    free(keystream);
    free(ciphertext);
    free(plaintext);
    printf("\n===================================================\n");
    printf("KSA : Total cycles      :%.2f\n", (total_ksa_cycles));
    printf("KSA : cycles/byte       :%.2f CPB\n", (total_ksa_cycles/256.0)); 
    printf("PRGA: cycles/byte       :%.2f CPB\n", cycles_per_byte_prga);
    printf("===================================================\n");  
}
int main()
{
    
    
    /*INItTIALISATION*/
    unsigned char* S=(unsigned char*)malloc(256*sizeof(unsigned char));

    unsigned char key[]={
    0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07};
    
    size_t keylen=sizeof(key);
    
    size_t msg_len=1000;
    for(msg_len=100;msg_len<MAX_MSG_LEN;msg_len=msg_len+STEPS)
    {
        unsigned char* msg =malloc(msg_len);
        gen_msg(msg,msg_len);
        rc4(S,key,keylen,msg,msg_len);
        free(msg);
    }

    
    
    return 0;
}

/*TO COMPILE-> gcc rc4.c -o test -o1*/

