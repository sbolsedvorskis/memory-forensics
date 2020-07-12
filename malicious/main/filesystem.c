#include <stdio.h>
#include <stdlib.h>
#include "filesystem.h"

unsigned char* read_file(const char* filename) {
    FILE *f = fopen(filename, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    unsigned char *buffer = malloc(fsize + 1);
    fread(buffer, 1, fsize, f);
    fclose(f);

    buffer[fsize] = 0;
    return buffer;
}

int write_file(const char* filename, unsigned char* buffer, int buffer_len) {
    FILE *f = fopen(filename, "wb");

    fwrite(buffer, buffer_len, 1, f);
    fclose(f);

    return buffer_len;
}