#include "core/cryptex.h"
#include "util/log.h"

#include <iostream>
#include <string>
#include <fstream>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

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

    cout << "enter password:\n";

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

BYTES read_stdin(PLAINTEXT *argv, PLAINTEXT *argv_end, SIZE &inlen, ENCRYPTION_CONTEXT *ctx)
{
    BYTES in = (BYTES)calloc(4086, sizeof(char));
    
    ssize_t bytes_read = read(0, in, 4086);
    
    if(bytes_read <=0)
    {
        return NULL;
    }

    size_t keylen = strlen((PLAINTEXT)in);

    ctx->aes_encr = AES_create_encrypt_ctx(in, keylen, NULL, 15);
    ctx->aes_decr = AES_create_decrypt_ctx(in, keylen, NULL, 15);

    inlen = bytes_read - keylen - 1;
    memcpy(in, in + keylen + 1, inlen);

    return in;
}

bool read_password(PLAINTEXT *argv, PLAINTEXT *argv_end, ENCRYPTION_CONTEXT *ctx)
{
    SIZE keylen;
    if (cmd_one_exists(argv, argv_end, "-encrypt", "-decrypt"))
    {
        BYTES key;

        key = (BYTES)keyboard_read_key(keylen);

        if (!key)
        {
            return 0;
        }

        ctx->aes_encr = AES_create_encrypt_ctx(key, keylen, NULL, 15);
        ctx->aes_decr = AES_create_decrypt_ctx(key, keylen, NULL, 15);
    }
    else if (cmd_one_exists(argv, argv_end, "-rsa_encrypt", "-rsa_decrypt", "-sign", "-verify"))
    {
        PLAINTEXT keyfile = get_cmd_option(argv, argv_end, "-key");

        if (not keyfile)
        {
            return 0;
        }

        PLAINTEXT *arg = new PLAINTEXT[2];
        int argc;

        parse_command(arg, argc, "-in", keyfile);

        PLAINTEXT key = (PLAINTEXT)read_file(arg, arg + argc, keylen);

        delete[] arg;

        if (cmd_one_exists(argv, argv_end, "-rsa_decrypt", "-sign"))
        {
            ctx->private_key = RSA_create_private_key(key);
            ctx->sign_ctx = RSA_create_sign_ctx(ctx->private_key);
        }
        else
        {
            ctx->public_key = RSA_create_public_key(key);
            ctx->verify_ctx = RSA_create_verify_ctx(ctx->public_key);
        }
    }

    return true;
}

bool write_file(BYTES data, SIZE data_size, PLAINTEXT *argv, PLAINTEXT *argv_end)
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

BYTES read_file(PLAINTEXT *argv, PLAINTEXT *argv_end, SIZE &file_size)
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

int encrypt(BYTES in, SIZE inlen, ENCRYPT_CONTEXT ctx, BYTES *out, SIZE &outlen, PLAINTEXT *argv, PLAINTEXT *argv_end)
{
    int result;
    bool encode = cmd_option_exists(argv, argv_end, "-encode");

    if (encode)
    {
        result = AES_encrypt(ctx, in, inlen, (BASE64 *)out);
        outlen = strlen((PLAINTEXT)(*out));
    }
    else
    {
        result = AES_encrypt(ctx, in, inlen, out, outlen);
    }

    if (result < 0)
    {
        log(argv, argv_end, "[-] ERROR: encrypt: AES_encrypt error code: ", result, ".");
        return result;
    }

    return result;
}

int decrypt(BYTES in, SIZE inlen, DECRYPT_CONTEXT ctx, BYTES *out, SIZE &outlen, PLAINTEXT *argv, PLAINTEXT *argv_end)
{
    int result;
    bool decode = cmd_option_exists(argv, argv_end, "-decode");

    if (decode)
    {
        result = AES_decrypt(ctx, (BASE64)in, out, outlen);
    }
    else
    {
        result = AES_decrypt(ctx, in, inlen, out, outlen);
    }

    if (result < 0)
    {
        log(argv, argv_end, "[-] ERROR: decrypt: AES_decrypt error code: ", result, ".");
        return result;
    }

    return result;
}

int rsa_encrypt(BYTES in, SIZE inlen, RSA_ENCRYPT_CONTEXT ctx, BYTES *out, SIZE &outlen, PLAINTEXT *argv, PLAINTEXT *argv_end)
{
    int result;

    if (cmd_option_exists(argv, argv_end, "-encode"))
    {
        result = RSA_encrypt(ctx, in, inlen, (BASE64 *)out);
        outlen = strlen((PLAINTEXT)(*out));
    }
    else
    {
        result = RSA_encrypt(ctx, in, inlen, (BYTES *)out, outlen);
    }

    if (result < 0)
    {
        log(argv, argv_end, "[-] ERROR: rsa_encrypt: RSA_encrypt error code: ", result, ".");
    }

    return result;
}

int rsa_decrypt(BYTES in, SIZE inlen, RSA_DECRYPT_CONTEXT ctx, BYTES *out, SIZE &outlen, PLAINTEXT *argv, PLAINTEXT *argv_end)
{
    int result;

    if (cmd_option_exists(argv, argv_end, "-decode"))
    {
        result = RSA_decrypt(ctx, (BASE64)in, out, outlen);
    }
    else
    {
        result = RSA_decrypt(ctx, in, inlen, out, outlen);
    }

    if (result < 0)
    {
        log(argv, argv_end, "[-] ERROR: rsa_decrypt: RSA_decrypt error code: ", result, ".");
    }

    return result;
}

int rsa_sign(BYTES in, SIZE inlen, SIGN_CONTEXT ctx, BYTES *out, SIZE &outlen, PLAINTEXT *argv, PLAINTEXT *argv_end)
{
    int result;
    if (cmd_option_exists(argv, argv_end, "-encode"))
    {
        result = RSA_sign(ctx, in, inlen, (BASE64 *)out);
        outlen = strlen((PLAINTEXT)(*out));
    }
    else
    {
        result = RSA_sign(ctx, in, inlen, out, outlen);
    }

    if (result < 0)
    {
        log(argv, argv_end, "[-] ERROR: rsa_sign: RSA_sign error code: ", result, ".");
    }

    return result;
}

int rsa_verify_signature(BYTES in, SIZE inlen, BYTES signature, SIZE signlen, VERIFY_CONTEXT ctx, bool &authentic, PLAINTEXT *argv, PLAINTEXT *argv_end)
{
    int result;
    if (cmd_option_exists(argv, argv_end, "-decode"))
    {
        result = RSA_verify_signature(ctx, in, inlen, (BASE64)signature, authentic);
    }
    else
    {
        result = RSA_verify_signature(ctx, signature, signlen, in, inlen, authentic);
    }

    if (result < 0)
    {
        log(argv, argv_end, "[-] ERROR: rsa_verify_signature: RSA_verify_signature error code: ", result, ".");
    }

    return result;
}

int cryptography(BYTES in, SIZE inlen, ENCRYPTION_CONTEXT *ctx, BYTES *out, SIZE &outlen, PLAINTEXT *argv, PLAINTEXT *argv_end)
{
    int result = 0;

    if (not inlen)
    {
        log(argv, argv_end, "[-] WARNING: cryptography: input buffer empty.");
        return result;
    }

    if (cmd_option_exists(argv, argv_end, "-encrypt"))
    {
        result = encrypt(in, inlen, ctx->aes_encr, out, outlen, argv, argv_end);
    }
    else if (cmd_option_exists(argv, argv_end, "-decrypt"))
    {
        result = decrypt(in, inlen, ctx->aes_decr, out, outlen, argv, argv_end);
    }
    else if (cmd_option_exists(argv, argv_end, "-rsa_encrypt"))
    {
        result = rsa_encrypt(in, inlen, ctx->rsa_encr_ctx, out, outlen, argv, argv_end);
    }
    else if (cmd_option_exists(argv, argv_end, "-rsa_decrypt"))
    {
        result = rsa_decrypt(in, inlen, ctx->rsa_decr_ctx, out, outlen, argv, argv_end);
    }
    else if (cmd_option_exists(argv, argv_end, "-sign"))
    {
        result = rsa_sign(in, inlen, ctx->sign_ctx, out, outlen, argv, argv_end);
    }
    else if (cmd_option_exists(argv, argv_end, "-verify"))
    {
        bool authentic;
        PLAINTEXT *arg = new PLAINTEXT[2];
        int argc;

        PLAINTEXT signfile = get_cmd_option(argv, argv_end, "-signature");

        if(not signfile)
        {
            cout << "[-] Error: Signature is missing.\n";
            return result;
        }

        add_command(arg, argc, "-in", signfile);

        SIZE signlen;
        BYTES signature = read_file(arg, arg + argc, signlen);

        if(not signature)
        {
            cout << "[-] Error: Cannot read signature file.\n";
            return result;
        }

        result = rsa_verify_signature(in, inlen, signature, signlen, ctx->verify_ctx, authentic, argv, argv_end);
        outlen = 0;

        cout << "[+] Valid: " << authentic << ".\n";

        delete[] arg;
        free(signature);
    }

    return result;
}