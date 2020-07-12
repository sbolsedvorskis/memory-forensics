#include <stdio.h>
#include <string.h>
#include "filesystem.h"
#include "openssl.h"
#include "enclave_u.h"
#include "enclave_init.h"

unsigned char *ma1_key = (unsigned char *)"";
unsigned char *ma1_iv = (unsigned char *)"";
const char * original_file_name = "data/original.txt";

void encrypt_data_with_memory_stored_keys() {
    printf("\nMA1: Create cryptography key that is present only in the volatile memory\n");
    ma1_key = (unsigned char *)"module-tearoom-playbook-tattered";
                               
    printf(" - ma1_key: <hidden>\n");
    ma1_iv = (unsigned char *)"chattel-unbiased";
                              
    printf(" - ma1_iv: <hidden>\n");

    printf("\nMA2: Encrypt data on disk using cryptography key present only in the volatile memroy\n");
    const char * encrypted_file_name = "data/ma2_encrypted.bin";

    unsigned char *original = read_file(original_file_name);
    printf(" - Original %zu bytes from %s\n", strlen ((char *)original), original_file_name);

    unsigned char encrypted[1024 * 1024];
    int encrypted_len = encrypt_data(original, strlen ((char *)original), ma1_key, ma1_iv, encrypted);
    printf(" - Encrypted %d bytes to %s\n", encrypted_len, encrypted_file_name);
    write_file(encrypted_file_name, encrypted, encrypted_len);
}

extern sgx_enclave_id_t global_eid;

void encrypt_data_with_sgx_stored_keys() {
    printf("\nMA3: Create cryptography key that is stored only in the secure SGX Container\n");

    initialize_enclave();

    printf(" - enclave_id = %lu\n", global_eid);

	char * sgx_key;
	sgx_status_t result_key  = ecall_get_key(global_eid, &sgx_key);
	printf(" - ma3_sgx_key: <hidden>\n");

	char * sgx_iv;
	sgx_status_t result_iv  = ecall_get_iv(global_eid, &sgx_iv);
	printf(" - ma3_sgx_iv: <hidden>\n");

    printf("\nMA4: Data on disk that was encrypted using cryptography key from MA3\n");

    const char * encrypted_file_name = "data/ma4_encrypted.bin";

    unsigned char *original = read_file(original_file_name);
    printf(" - Original %zu bytes from %s\n", strlen ((char *)original), original_file_name);

    unsigned char encrypted[1024 * 1024];
    int encrypted_len = encrypt_data(original, strlen ((char *)original), sgx_key, sgx_iv, encrypted);
    printf(" - Encrypted %d bytes to %s\n", encrypted_len, encrypted_file_name);
    write_file(encrypted_file_name, encrypted, encrypted_len);
}
