#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "malicious.h"

int main(int argc, char *argv[]) {
	encrypt_data_with_memory_stored_keys();

	encrypt_data_with_sgx_stored_keys();

	getchar();

    return 0;
}
