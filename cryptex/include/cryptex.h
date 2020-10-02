#include "cmd.h"
#include "daemon.h"

#include <cryptography.h>

char *read_password(PLAINTEXT *argv, PLAINTEXT *argv_end, SIZE &datalen);

unsigned char *read_file(char **argv, char **argv_end, size_t &file_size);
bool write_file(unsigned char *data, size_t data_size, char **argv, char **argv_end);

int encrypt(unsigned char *in, size_t inlen, char *key, unsigned char **out, size_t &outlen, char **argv, char **argv_end);
int decrypt(unsigned char *in, size_t inlen, char *key, unsigned char **out, size_t &outlen, char **argv, char **argv_end);

int cryptography(BYTES in, SIZE inlen, void *key, BYTES *out, SIZE &outlen, PLAINTEXT *argv, PLAINTEXT *argv_end);