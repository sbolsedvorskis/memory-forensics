CC := gcc
CFLAGS := -g -O2 -Wall -Werror -std=gnu99 -fno-strict-aliasing -fno-strict-overflow -D_FORTIFY_SOURCE=2 -fstack-protector-all -DHAVE_GNU_STACK -Wno-pointer-sign -fPIC -Wno-attributes -Wno-implicit-function-declaration 
INCLUDE := -I. 

SGX_SDK ?= /opt/intel/sgxsdk
SGX_MODE ?= SIM
SGX_COMMON_CFLAGS := -m64 -g -O2
SGX_LIBRARY_PATH := $(SGX_SDK)/lib64
SGX_ENCLAVE_SIGNER := $(SGX_SDK)/bin/x64/sgx_sign
SGX_EDGER8R := $(SGX_SDK)/bin/x64/sgx_edger8r
SGX_ENCLAVE_NAME := enclave.so
SGX_SIGNED_ENCLAVE_NAME := enclave.signed.so
SGX_ENCLAVE_CONFIG := enclave/enclave.config.xml

ifeq ($(SGX_MODE), HW)
	Urts_Library_Name := sgx_urts
	Trts_Library_Name := sgx_trts
	Service_Library_Name := sgx_tservice
else
	Urts_Library_Name := sgx_urts_sim
	Trts_Library_Name := sgx_trts_sim
	Service_Library_Name := sgx_tservice_sim
endif

App_Link_Flags := $(SGX_COMMON_CFLAGS) -L$(SGX_LIBRARY_PATH) -l$(Urts_Library_Name) 

ifeq ($(SGX_MODE), HW)
	App_Link_Flags += -lsgx_uae_service
else
	App_Link_Flags += -lsgx_uae_service_sim
endif

#################################

all: malicious signed_enclave

########## Intel SGX ############

##### App Objects #####

App_C_Flags := $(SGX_COMMON_CFLAGS) 

ifeq ($(SGX_DEBUG), 1)
	App_C_Flags += -DDEBUG -UNDEBUG -UEDEBUG
else
	App_C_Flags += -DNDEBUG -UEDEBUG -UDEBUG
endif

##### Enclave Objects #####

Enclave_Include_Paths := -Ienclave -I$(SGX_SDK)/include -I$(SGX_SDK)/include/tlibc -I$(SGX_SDK)/include/stlport

Enclave_C_Flags := $(SGX_COMMON_CFLAGS) -nostdinc -fvisibility=hidden -fpie -fstack-protector $(Enclave_Include_Paths)
Enclave_Cpp_Flags := $(Enclave_C_Flags) -nostdinc -nostdinc++
Enclave_Link_Flags := $(SGX_COMMON_CFLAGS) -Wl,--no-undefined -nostdlib -nodefaultlibs -nostartfiles -L$(SGX_LIBRARY_PATH) \
	-Wl,--whole-archive -l$(Trts_Library_Name) -Wl,--no-whole-archive \
	-Wl,--start-group -lsgx_tstdc -lsgx_tcmalloc -lsgx_tcrypto -l$(Service_Library_Name) -Wl,--end-group \
	-Wl,-Bstatic -Wl,-Bsymbolic -Wl,--no-undefined \
	-Wl,-pie,-eenclave_entry -Wl,--export-dynamic  \
	-Wl,--defsym,__ImageBase=0 \
	-Wl,--version-script=enclave/enclave.ou

enclave_t.c: $(SGX_EDGER8R) enclave/enclave.edl
	cd enclave && $(SGX_EDGER8R) --trusted ./enclave.edl --search-path . --search-path $(SGX_SDK)/include

enclave_t.o: enclave_t.c
	cd enclave && $(CC) $(Enclave_C_Flags) -c $< -o $@

enclave_attack.o: enclave/enclave_attack.c enclave_t.c
	$(CC) $(CFLAGS) $(Enclave_C_Flags) -I$(SGX_SDK)/include -fPIC -DPIC -c -o enclave/$@ $<

enclave: enclave_t.o  enclave_attack.o
	$(CC) enclave/enclave_t.o enclave/enclave_attack.o -o $(SGX_ENCLAVE_NAME) $(Enclave_Link_Flags)

signed_enclave: enclave
	$(SGX_ENCLAVE_SIGNER) sign -key enclave/enclave_private.pem -enclave $(SGX_ENCLAVE_NAME) -out $(SGX_SIGNED_ENCLAVE_NAME) -config $(SGX_ENCLAVE_CONFIG)

openssl.o: main/openssl.c 
	$(CC) $(App_C_Flags) -I$(SGX_SDK)/include -c $< -o main/$@ -lm -lssl -lcrypto

filesystem.o: main/filesystem.c
	$(CC) $(App_C_Flags) -I$(SGX_SDK)/include -c $< -o main/$@

malicious.o: main/malicious.c
	$(CC) $(App_C_Flags) -I$(SGX_SDK)/include -c $< -o main/$@

enclave_u.c: $(SGX_EDGER8R) enclave/enclave.edl
	cd main && $(SGX_EDGER8R) --untrusted ../enclave/enclave.edl --search-path ../enclave --search-path $(SGX_SDK)/include

enclave_u.o: enclave_u.c
	cd main && $(CC) $(App_C_Flags) -I$(SGX_SDK)/include -c $< -o $@

enclave_init.o: main/enclave_init.c 
	$(CC) $(App_C_Flags) -I$(SGX_SDK)/include -c $< -o main/$@

main.o: enclave_u.o malicious.o main/main.c
	cd main && $(CC) -g -O2 -fPIC -DPIC -Werror $(App_C_Flags) -I$(SGX_SDK)/include -c main.c -o $@ 

malicious: main.o enclave_u.o enclave_init.o malicious.o filesystem.o openssl.o
	$(CC) $(CFLAGS) -o $@ main/main.o main/enclave_u.o main/enclave_init.o main/malicious.o main/filesystem.o main/openssl.o $(App_Link_Flags) -lm -lssl -lcrypto -lpthread

clean:
	find . -iname "enclave_u.?" -delete
	find . -iname "enclave_t.?" -delete
	find . -iname "$(SGX_ENCLAVE_NAME)" -delete
	find . -iname "$(SGX_SIGNED_ENCLAVE_NAME)" -delete
	find . -iname "*\.o" -delete
	find . -iname "*\.i" -delete
	find . -iname "*~" -delete	
	rm malicious -f
