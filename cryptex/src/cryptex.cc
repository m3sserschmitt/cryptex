#include "../include/cryptex.h"
#include "../include/log.h"

#include <fstream>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

static char getch()
{
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    return (buf);
}

static PLAINTEXT keyboard_read_key(SIZE &inlen)
{
    char c = 0;
    char *password = (char *)malloc(1024);
    char *ptr = password;

    memset(password, 0, 1024);

    printf("enter password:\n");

    do
    {
        c = getch();

        if (c == '\b' and ptr != password)
        {
            *ptr = 0;
            ptr--;
            continue;
        }

        *ptr = c;
        ptr++;

    } while (c != '\n');

    ptr--;
    *ptr = 0;

    inlen = strlen(password);
    return password;
}

/*
static PLAINTEXT read_key_file(PLAINTEXT *argv, PLAINTEXT *argv_end, SIZE &inlen)
{
    PLAINTEXT keyfile = get_cmd_option(argv, argv_end, "-key");

    if (not keyfile)
    {
        return (PLAINTEXT)0;
    }

    std::fstream file;
    file.open(keyfile, std::ios::in);

    if (not file.is_open())
    {
        return (PLAINTEXT)0;
    }

    inlen = 0;

    file.seekg(std::ios::end);
    inlen = file.tellg();
    file.seekg(std::ios::beg);

    printf("%zi\n", inlen);

    PLAINTEXT key = (PLAINTEXT)malloc(inlen + 1);
    memset(key, 0, inlen + 1);

    file.read(key, inlen);

    return key;
}
*/

PLAINTEXT read_password(PLAINTEXT *argv, PLAINTEXT *argv_end, SIZE &datalen)
{
    if (cmd_one_exists(argv, argv_end, "-encrypt", "-decrypt"))
    {
        return keyboard_read_key(datalen);
    }
    else if (cmd_one_exists(argv, argv_end, "-rsa_encrypt", "-rsa_decrypt", "-sign", "-verify"))
    {
        PLAINTEXT keyfile = get_cmd_option(argv, argv_end, "-key");

        if (not keyfile)
        {
            return (PLAINTEXT)0;
        }

        PLAINTEXT *arg = new PLAINTEXT[2];
        int argc;

        parse_command(arg, argc, "-in", keyfile);

        PLAINTEXT key = (PLAINTEXT)read_file(arg, arg + argc, datalen);
        
        delete[] arg;

        return key;
    }

    return (PLAINTEXT)0;
}

bool write_file(unsigned char *data, size_t data_size, char **argv, char **argv_end)
{
    std::fstream file;

    bool encoded = cmd_option_exists(argv, argv_end, "-encode");
    char *out = get_cmd_option(argv, argv_end, "-out");

    if (not out)
    {
        log(argv, argv_end, "[-] ERROR: write_file: No output file.");
        return false;
    }

    if (encoded)
    {
        file.open(out, std::ios::out);
    }
    else
    {
        file.open(out, std::ios::out | std::ios::binary);
    }

    if (not file.is_open())
    {
        log(argv, argv_end, "[-] ERROR: write_file: Cannot open output file.");
        return false;
    }

    file.write((const char *)data, data_size);
    file.close();

    return true;
}

unsigned char *read_file(char **argv, char **argv_end, size_t &file_size)
{
    file_size = 0;
    std::fstream file;

    char *in = get_cmd_option(argv, argv_end, "-in");

    if (not in)
    {
        log(argv, argv_end, "[-] ERROR: read_file: No input file.");
        return nullptr;
    }

    bool binary = cmd_option_exists(argv, argv_end, "-binary");

    if (binary)
    {
        file.open(in, std::ios::in | std::ios::binary);
    }
    else
    {
        file.open(in, std::ios::in);
    }

    if (not file.is_open())
    {
        log(argv, argv_end, "[-] ERROR: read_file: Cannot open input file.");
        return nullptr;
    }

    file.seekg(0, std::ios::end);
    file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    unsigned char *data_buffer = (unsigned char *)malloc(file_size + 1);

    if (not data_buffer)
    {
        log(argv, argv_end, "[-] ERROR: read_file: memory allocation error: malloc.");
        return nullptr;
    }

    memset(data_buffer, 0, file_size + 1);

    file.read((char *)data_buffer, file_size);
    file.close();

    return data_buffer;
}

int encrypt(unsigned char *in, size_t inlen, char *key, unsigned char **out, size_t &outlen, char **argv, char **argv_end)
{
    ENCRYPT_CTX encr;
    int result = AES_encrypt_init(&encr, (BYTES)key, strlen(key), nullptr, 15);

    if (result < 0)
    {
        log(argv, argv_end, "[-] ERROR: encrypt: AES_encrypt_init error code: ", result, ".");
        return result;
    }

    bool encode = cmd_option_exists(argv, argv_end, "-encode");

    if (encode)
    {
        result = AES_encrypt(encr, in, inlen, (BASE64 *)out);
        outlen = strlen((PLAINTEXT)(*out));
    }
    else
    {
        result = AES_encrypt(encr, in, inlen, out, outlen);
    }

    if (result < 0)
    {
        log(argv, argv_end, "[-] ERROR: encrypt: AES_encrypt error code: ", result, ".");
        return result;
    }

    AES_free_context(encr);

    return result;
}

int decrypt(unsigned char *in, size_t inlen, char *key, unsigned char **out, size_t &outlen, char **argv, char **argv_end)
{
    DECRYPT_CTX decr;
    int result = AES_decrypt_init(&decr, (BYTES)key, strlen(key), nullptr, 15);

    if (result < 0)
    {
        log(argv, argv_end, "[-] ERROR: decrypt: AES_decrypt_init error code: ", result, ".");
        return result;
    }

    bool decode = cmd_option_exists(argv, argv_end, "-decode");

    if (decode)
    {
        result = AES_decrypt(decr, (BASE64)in, out, outlen);
    }
    else
    {
        result = AES_decrypt(decr, in, inlen, out, outlen);
    }

    if (result < 0)
    {
        log(argv, argv_end, "[-] ERROR: decrypt: AES_decrypt error code: ", result, ".");
        return result;
    }

    AES_free_context(decr);

    return result;
}

int rsa_encrypt(BYTES in, SIZE inlen, PUBLIC_KEY key, BYTES *out, SIZE &outlen, PLAINTEXT *argv, PLAINTEXT *argv_end)
{
    int result;
    if (cmd_option_exists(argv, argv_end, "-encode"))
    {
        result = RSA_encrypt(in, inlen, (BASE64 *)out, key);
        outlen = strlen((PLAINTEXT)(*out));
    }
    else
    {
        result = RSA_encrypt(in, inlen, (BYTES *)out, outlen, key);
    }

    if (result < 0)
    {
        log(argv, argv_end, "[-] ERROR: rsa_encrypt: RSA_encrypt error code: ", result, ".");
    }

    return result;
}

int rsa_decrypt(BYTES in, SIZE inlen, PRIVATE_KEY key, BYTES *out, SIZE &outlen, PLAINTEXT *argv, PLAINTEXT *argv_end)
{
    int result;
    if (cmd_option_exists(argv, argv_end, "-decode"))
    {
        result = RSA_decrypt((BASE64)in, out, outlen, key);
    }
    else
    {
        result = RSA_decrypt(in, inlen, out, outlen, key);
    }

    if (result < 0)
    {
        log(argv, argv_end, "[-] ERROR: rsa_decrypt: RSA_decrypt error code: ", result, ".");
    }

    return result;
}

int rsa_sign(BYTES in, SIZE inlen, PRIVATE_KEY key, BYTES *out, SIZE &outlen, PLAINTEXT *argv, PLAINTEXT *argv_end)
{
    int result;
    if (cmd_option_exists(argv, argv_end, "-encode"))
    {
        result = RSA_sign(key, in, inlen, (BASE64 *)out);
        outlen = strlen((PLAINTEXT)(*out));
    }
    else
    {
        result = RSA_sign(key, in, inlen, out, outlen);
    }

    if (result < 0)
    {
        log(argv, argv_end, "[-] ERROR: rsa_sign: RSA_sign error code: ", result, ".");
    }

    return result;
}

int rsa_verify_signature(BYTES in, SIZE inlen, BYTES signature, SIZE signlen, PUBLIC_KEY key, bool &authentic, PLAINTEXT *argv, PLAINTEXT *argv_end)
{
    int result;
    if (cmd_option_exists(argv, argv_end, "-decode"))
    {
        result = RSA_verify_signature(key, in, inlen, (BASE64)signature, authentic);
    }
    else
    {
        result = RSA_verify_signature(key, signature, signlen, in, inlen, authentic);
    }

    if (result < 0)
    {
        log(argv, argv_end, "[-] ERROR: rsa_verify_signature: RSA_verify_signature error code: ", result, ".");
    }

    return result;
}

int cryptography(BYTES in, SIZE inlen, void *key, BYTES *out, SIZE &outlen, PLAINTEXT *argv, PLAINTEXT *argv_end)
{
    int result = 0;

    if (not inlen)
    {
        log(argv, argv_end, "[-] WARNING: cryptography: input buffer empty.");
        return result;
    }

    KEY rsa_key;
    if (cmd_one_exists(argv, argv_end, "-rsa_encrypt", "-verify"))
    {
        rsa_key = RSA_create_public_key((PLAINTEXT)key);
    }
    else if (cmd_one_exists(argv, argv_end, "-rsa_decrypt", "-sign"))
    {
        rsa_key = RSA_create_private_key((PLAINTEXT)key);
    }
    

    if (cmd_option_exists(argv, argv_end, "-encrypt"))
    {
        result = encrypt(in, inlen, (PLAINTEXT)key, out, outlen, argv, argv_end);
    }
    else if (cmd_option_exists(argv, argv_end, "-decrypt"))
    {
        result = decrypt(in, inlen, (PLAINTEXT)key, out, outlen, argv, argv_end);
    }
    else if (cmd_option_exists(argv, argv_end, "-rsa_encrypt"))
    {
        result = rsa_encrypt(in, inlen, rsa_key, out, outlen, argv, argv_end);
    }
    else if (cmd_option_exists(argv, argv_end, "-rsa_decrypt"))
    {
        result = rsa_decrypt(in, inlen, rsa_key, out, outlen, argv, argv_end);
    }
    else if (cmd_option_exists(argv, argv_end, "-sign"))
    {
        result = rsa_sign(in, inlen, rsa_key, out, outlen, argv, argv_end);
    }
    else if (cmd_option_exists(argv, argv_end, "-verify"))
    {
        bool authentic;
        PLAINTEXT *arg = new PLAINTEXT[2];
        int argc;

        PLAINTEXT signfile = get_cmd_option(argv, argv_end, "-signature");
        add_command(arg, argc, "-in", signfile);

        SIZE signlen;
        BYTES signature = read_file(arg, arg + argc, signlen);

        result = rsa_verify_signature(in, inlen, signature, signlen, rsa_key, authentic, argv, argv_end);
    
        outlen = 0;

        printf("[+] Valid: %i\n", authentic);

        delete[] arg;
        free(signature);
    }

    return result;
}