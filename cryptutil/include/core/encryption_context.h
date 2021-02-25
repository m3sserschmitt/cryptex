#include <crypto/cryptography.h>

#ifndef ENCRYPTION_CONTEXT_H
#define ENCRYPTION_CONTEXT_H

typedef struct
{
    PUBLIC_KEY public_key;
    PRIVATE_KEY private_key;

    ENCRYPT_CONTEXT aes_encr;
    DECRYPT_CONTEXT aes_decr;

    SIGN_CONTEXT sign_ctx;
    VERIFY_CONTEXT verify_ctx;

    RSA_ENCRYPT_CONTEXT rsa_encr_ctx;
    RSA_DECRYPT_CONTEXT rsa_decr_ctx;
} ENCRYPTION_CONTEXT;

#endif