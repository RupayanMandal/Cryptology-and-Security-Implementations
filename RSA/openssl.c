#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#include <x86intrin.h>

#define ITERATIONS 10000


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

/* ---- helpers ------------------------------------------------------- */


void handle_errors(void)
{
    ERR_print_errors_fp(stderr);
    exit(EXIT_FAILURE);
}

EVP_PKEY *load_public_key(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
        return NULL;

    EVP_PKEY *pkey = PEM_read_PUBKEY(fp, NULL, NULL, NULL);
    fclose(fp);
    return pkey;
}

EVP_PKEY *load_private_key(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
        return NULL;

    EVP_PKEY *pkey = PEM_read_PrivateKey(fp, NULL, NULL, NULL);
    fclose(fp);
    return pkey;
}

int rsa_encrypt(EVP_PKEY *pubkey,
                const unsigned char *plaintext,
                size_t plaintext_len,
                unsigned char **ciphertext,
                size_t *ciphertext_len)
{
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pubkey, NULL);
    if (!ctx)
        return 0;

    if (EVP_PKEY_encrypt_init(ctx) <= 0)
        handle_errors();

    if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING) <= 0)
        handle_errors();

    /* Determine output buffer size */
    if (EVP_PKEY_encrypt(ctx,
                         NULL,
                         ciphertext_len,
                         plaintext,
                         plaintext_len) <= 0)
        handle_errors();

    *ciphertext = malloc(*ciphertext_len);

    if (EVP_PKEY_encrypt(ctx,
                         *ciphertext,
                         ciphertext_len,
                         plaintext,
                         plaintext_len) <= 0)
        handle_errors();

    EVP_PKEY_CTX_free(ctx);
    return 1;
}


/* int rsa_decrypt(EVP_PKEY *privkey,
                const unsigned char *ciphertext,
                size_t ciphertext_len,
                unsigned char **plaintext,
                size_t *plaintext_len)
{
    
    if (EVP_PKEY_decrypt(ctx,
                         *plaintext,
                         plaintext_len,
                         ciphertext,
                         ciphertext_len) <= 0)
        handle_errors();

    (*plaintext)[*plaintext_len] = '\0';

    EVP_PKEY_CTX_free(ctx);
    return 1;
}
 */
int main(void)
{
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();

    EVP_PKEY *pub = load_public_key("public.pem");
    EVP_PKEY *priv = load_private_key("private.pem");

    if (!pub || !priv) {
        printf("Failed to load keys\n");
        return 1;
    }

    const char *msg = "All my roads to this wall. All my words to this silence. All my pride to this fall.";
    unsigned int rsaBytes = strlen(msg);

    unsigned char *cipher = NULL;
    size_t cipher_len;

    unsigned long long encCycles = 0;

    for (int i = 0; i < ITERATIONS; i++) 
    {
        unsigned long long t1 = start_tsc();
        if (!rsa_encrypt(pub,
                     (const unsigned char *)msg,
                     strlen(msg),
                     &cipher,
                     &cipher_len))
    {
        printf("Encryption failed\n");
        return 1;
    }

    
    //printf("Encrypted size = %zu bytes\n", cipher_len);

        unsigned long long t2 = stop_tsc();
        encCycles += (t2 - t1);
    }
    
    unsigned char *plain = NULL;
    size_t plain_len;
    
    
    unsigned long long decCycles = 0;

    EVP_PKEY_CTX* ctx;
    ctx = EVP_PKEY_CTX_new(priv, NULL);
    if (!ctx) return 0;

    if (EVP_PKEY_decrypt_init(ctx) <= 0) handle_errors();

    if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING) <= 0) handle_errors();
    
    if (EVP_PKEY_decrypt(ctx,
                     NULL,
                     &plain_len,
                     cipher,
                     cipher_len) <= 0)
    handle_errors();

    plain = malloc(plain_len + 1);

    size_t outlen;
    
    printf("cipher_len = %zu\n", cipher_len);
    printf("RSA size   = %d\n", EVP_PKEY_get_size(priv));
    for (int i = 0; i < ITERATIONS; i++) 
    {
        unsigned long long t1 = start_tsc();
        /* if (!rsa_decrypt(priv,
                     cipher,
                     cipher_len,
                     &plain,
                     &plain_len))
         */
        outlen = plain_len;
        if (EVP_PKEY_decrypt(ctx,
                         plain,
                         &outlen,
                         cipher,
                         cipher_len) <= 0)
        handle_errors();

    (plain)[outlen] = '\0';
    unsigned long long t2 = stop_tsc();
    //printf("Decrypted message: %s\n", plain);
    decCycles += (t2 - t1);
    }
    
    EVP_PKEY_CTX_free(ctx);
    printf("\nDecrypted message: %s\n", plain);
    
    free(cipher);
    free(plain);

    EVP_PKEY_free(pub);
    EVP_PKEY_free(priv);


    double avgEnc = (double)encCycles / ITERATIONS;
    double avgDec = (double)decCycles / ITERATIONS;

    printf("===================================================\n");
    printf("RSA block size:                %d bytes\n", rsaBytes);
    printf("Encrypt: avg cycles            %.2f\n", avgEnc);
    printf("Encrypt: cycles/byte (block)   %.2f CPB\n", avgEnc / rsaBytes);
    printf("Decrypt: avg cycles            %.2f\n", avgDec);
    printf("Decrypt: cycles/byte (block)   %.2f CPB\n", avgDec / rsaBytes);
    printf("===================================================\n");


    return 0;
}





    
    



/* 
- - - - - compile with -- - - - - - - - -
gcc rsa_example.c -o rsa_example -lcrypto

- - - - - generate the key with -- - - - - - - - -
openssl genpkey -algorithm RSA -out private.pem -pkeyopt rsa_keygen_bits:2048

openssl pkey -in private.pem -pubout -out public.pem */