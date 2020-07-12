#include "openssl.h"
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

int encrypt_data(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *encrypted) {
    EVP_CIPHER_CTX *ctx;

    int len;

    int encrypted_len;

    if(!(ctx = EVP_CIPHER_CTX_new())) {
        ERR_print_errors_fp(stderr);
        abort();
    }

    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
        ERR_print_errors_fp(stderr);
        abort();
    }

    if(1 != EVP_EncryptUpdate(ctx, encrypted, &len, plaintext, plaintext_len)) {
        ERR_print_errors_fp(stderr);
        abort();
    }
    encrypted_len = len;

    if(1 != EVP_EncryptFinal_ex(ctx, encrypted + len, &len)) {
        ERR_print_errors_fp(stderr);
        abort();
    }
    encrypted_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return encrypted_len;
}

int decrypt_data(unsigned char *encrypted, int encrypted_len, unsigned char *key, unsigned char *iv, unsigned char *plaintext) {
    EVP_CIPHER_CTX *ctx;

    int len;

    int plaintext_len;

    if(!(ctx = EVP_CIPHER_CTX_new())) {
        ERR_print_errors_fp(stderr);
        abort();
    }

    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
        ERR_print_errors_fp(stderr);
        abort();
    }

    if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, encrypted, encrypted_len)) {
        ERR_print_errors_fp(stderr);
        abort();
    }
    plaintext_len = len;

    if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) {
        ERR_print_errors_fp(stderr);
        abort();
    }
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}
