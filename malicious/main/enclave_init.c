/*
 * Copyright (C) 2011-2017 Intel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Intel Corporation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <pthread.h>

#include "sgx_error.h"      
#include "sgx_eid.h" 
#include "sgx_urts.h"
#include "enclave_init.h"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

# define TOKEN_FILENAME   "enclave.token"
# define ENCLAVE_FILENAME "enclave.signed.so"

#define MAX_PATH 256

/* Global EID shared by multiple threads */
sgx_enclave_id_t global_eid = 0;

typedef struct _sgx_errlist_t {
	sgx_status_t err;
	const char *msg;
} sgx_errlist_t;

static sgx_errlist_t sgx_errlist[] = {
	{
		SGX_ERROR_UNEXPECTED,
		"Unexpected error occurred."
	},
	{
		SGX_ERROR_INVALID_PARAMETER,
		"Invalid parameter."
	},
	{
		SGX_ERROR_OUT_OF_MEMORY,
		"Out of memory."
	},
	{
		SGX_ERROR_ENCLAVE_LOST,
		"Power transition occurred."
	},
	{
		SGX_ERROR_INVALID_ENCLAVE,
		"Invalid enclave image."
	},
	{
		SGX_ERROR_INVALID_ENCLAVE_ID,
		"Invalid enclave identification."
	},
	{
		SGX_ERROR_INVALID_SIGNATURE,
		"Invalid enclave signature."
	},
	{
		SGX_ERROR_OUT_OF_EPC,
		"Out of EPC memory."
	},
	{
		SGX_ERROR_NO_DEVICE,
		"Invalid SGX device, Please make sure SGX module is enabled in the BIOS, and install SGX driver afterwards."
	},
	{
		SGX_ERROR_MEMORY_MAP_CONFLICT,
		"Memory map conflicted."
	},
	{
		SGX_ERROR_INVALID_METADATA,
		"Invalid enclave metadata."
	},
	{
		SGX_ERROR_DEVICE_BUSY,
		"SGX device was busy."
	},
	{
		SGX_ERROR_INVALID_VERSION,
		"Enclave version was invalid."
	},
	{
		SGX_ERROR_INVALID_ATTRIBUTE,
		"Enclave was not authorized."
	},
	{
		SGX_ERROR_ENCLAVE_FILE_ACCESS,
		"Can't open enclave file."
	},
};

/* Check error conditions for loading enclave */
void print_error_message(sgx_status_t ret, const char* fn)
{
	size_t idx = 0;
	size_t ttl = sizeof sgx_errlist/sizeof sgx_errlist[0];

	for (idx = 0; idx < ttl; idx++) {
		if(ret == sgx_errlist[idx].err) {
			printf("Error: %s from %s\n", sgx_errlist[idx].msg, fn);
			break;
		}
	}

	if (idx == ttl)
		printf("Error: Unexpected error occurred: %d from %s.\n", ret, fn);
}

void initialize_enclave(void) {
	
	char token_path[MAX_PATH] = {'\0'};
	sgx_launch_token_t token = {0};
	sgx_status_t ret = SGX_ERROR_UNEXPECTED;
	int updated = 0;

	/* Step 1: try to retrieve the launch token saved by last transaction */
	const char *home_dir = getpwuid(getuid())->pw_dir;

	if (home_dir != NULL && 
			(strlen(home_dir)+strlen("/")+sizeof(TOKEN_FILENAME)+1) <= MAX_PATH) {
		/* compose the token path */
		strncpy(token_path, home_dir, strlen(home_dir));
		strncat(token_path, "/", strlen("/"));
		strncat(token_path, TOKEN_FILENAME, sizeof(TOKEN_FILENAME)+1);
	} else {
		/* if token path is too long or $HOME is NULL */
		strncpy(token_path, TOKEN_FILENAME, sizeof(TOKEN_FILENAME));
	}

	FILE *fp = fopen(token_path, "rb");
	if (fp == NULL && (fp = fopen(token_path, "wb")) == NULL) {
		printf("Warning: Failed to create/open the launch token file \"%s\".\n", token_path);
	}
	if (fp != NULL) {
		/* read the token from saved file */
		size_t read_num = fread(token, 1, sizeof(sgx_launch_token_t), fp);
		if (read_num != 0 && read_num != sizeof(sgx_launch_token_t)) {
			/* if token is invalid, clear the buffer */
			memset(&token, 0x0, sizeof(sgx_launch_token_t));
			printf("Warning: Invalid launch token read from \"%s\".\n", token_path);
		}
	}

	/* Step 2: call sgx_create_enclave to initialize an enclave instance */
	ret = sgx_create_enclave(ENCLAVE_FILENAME, SGX_DEBUG_FLAG, &token, &updated, &global_eid, NULL);
	if (ret != SGX_SUCCESS) {
		print_error_message(ret, __func__);
		if (fp != NULL) fclose(fp);
		return;
	}

	/* Step 3: save the launch token if it is updated */
	if (updated == FALSE || fp == NULL) {
		/* if the token is not updated, or file handler is invalid, do not perform saving */
		if (fp != NULL) fclose(fp);
		return;
	}

	/* reopen the file with write capablity */
	fp = freopen(token_path, "wb", fp);
	if (fp != NULL) {
		size_t write_num = fwrite(token, 1, sizeof(sgx_launch_token_t), fp);
		if (write_num != sizeof(sgx_launch_token_t)) {
			printf("Warning: Failed to save launch token to \"%s\".\n", token_path);
		}
		fclose(fp);
	}
}

void destroy_enclave(void) {
	if (global_eid != 0) {
		//printf("Destroying enclave %lu!\n", global_eid);
		sgx_destroy_enclave(global_eid);
	} else {
		printf("Cannot destroy a non-initialized enclave!\n");
	}
}
