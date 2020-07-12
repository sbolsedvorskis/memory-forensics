#ifndef FILESYSTEM_H__
#define FILESYSTEM_H__

unsigned char* read_file(const char* filename);
int write_file(const char* filename, unsigned char* buffer, int buffer_len);

#endif