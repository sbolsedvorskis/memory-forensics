#ifndef OPENSSL_H__
#define OPENSSL_H__

int encrypt_data(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *encrypted);
int decrypt_data(unsigned char *encrypted, int encrypted_len, unsigned char *key, unsigned char *iv, unsigned char *plaintext);

#endif

