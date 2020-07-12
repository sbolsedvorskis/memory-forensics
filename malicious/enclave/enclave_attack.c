#include "enclave_t.h"

char * sgx_key = "drum-carve-waiter-traffic-broker";
char * sgx_iv = "welfare-starlike";

char * ecall_get_key() { 
	return sgx_key;
} 

char * ecall_get_iv() { 
	return sgx_iv;
} 
