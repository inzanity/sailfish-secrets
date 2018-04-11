/*
 * Copyright (C) 2017 Jolla Ltd.
 * Contact: Chris Adams <chris.adams@jollamobile.com>
 * All rights reserved.
 * BSD 3-Clause License, see LICENSE.
 */

#ifndef SAILFISHCRYPTO_PLUGIN_CRYPTO_OPENSSL_EVP_P_H
#define SAILFISHCRYPTO_PLUGIN_CRYPTO_OPENSSL_EVP_P_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/err.h>

#ifdef __cplusplus
extern "C" {
#endif

int osslevp_init();

int osslevp_pkcs5_pbkdf2_hmac(const char *pass, int passlen,
                              const unsigned char *salt, int saltlen,
                              int iter, int digestFunction,
                              int keylen, unsigned char *out);

int osslevp_aes_encrypt_plaintext(const EVP_CIPHER *evp_cipher,
                                  const unsigned char *init_vector,
                                  const unsigned char *key,
                                  int key_length,
                                  const unsigned char *plaintext,
                                  int plaintext_length,
                                  unsigned char **encrypted);

int osslevp_aes_decrypt_ciphertext(const EVP_CIPHER *evp_cipher,
                                   const unsigned char *init_vector,
                                   const unsigned char *key,
                                   int key_length,
                                   const unsigned char *ciphertext,
                                   int ciphertext_length,
                                   unsigned char **decrypted);

int osslevp_aes_auth_encrypt_plaintext(const EVP_CIPHER *evp_cipher,
                                       const unsigned char *init_vector,
                                       const unsigned char *key,
                                       int key_length,
                                       const unsigned char *auth,
                                       int auth_length,
                                       const unsigned char *plaintext,
                                       int plaintext_length,
                                       unsigned char **encrypted,
                                       unsigned char **authenticationTag,
                                       int authenticationTag_length);

int osslevp_aes_auth_decrypt_ciphertext(const EVP_CIPHER *evp_cipher,
                                        const unsigned char *init_vector,
                                        const unsigned char *key,
                                        int key_length,
                                        const unsigned char *auth,
                                        int auth_length,
                                        unsigned char *authenticationTag,
                                        int authenticationTag_length,
                                        const unsigned char *ciphertext,
                                        int ciphertext_length,
                                        unsigned char **decrypted,
                                        int *verified);

int osslevp_pkey_encrypt_plaintext(EVP_PKEY *pkey,
                                   int padding,
                                   const unsigned char *plaintext,
                                   size_t plaintext_length,
                                   uint8_t **encrypted,
                                   size_t *encrypted_length);

int osslevp_pkey_decrypt_ciphertext(EVP_PKEY *pkey,
                                    int padding,
                                    const unsigned char *ciphertext,
                                    size_t ciphertext_length,
                                    uint8_t **decrypted,
                                    size_t *decrypted_length);

int osslevp_digest(const EVP_MD *digestFunc,
                 const void *bytes,
                 size_t bytesCount,
                 uint8_t **digest,
                 size_t *digestLength);

int osslevp_sign(const EVP_MD *digestFunc,
                 EVP_PKEY *pkey,
                 const void *bytes,
                 size_t bytesCount,
                 uint8_t **signature,
                 size_t *signatureLength);

int osslevp_verify(const EVP_MD *digestFunc,
                   EVP_PKEY *pkey,
                   const void *bytes,
                   size_t bytesCount,
                   const uint8_t *signature,
                   size_t signatureLength);

int osslevp_generate_ec_key(int curveNid,
                            uint8_t **publicKeyBytes,
                            size_t *publicKeySize,
                            uint8_t **privateKeyBytes,
                            size_t *privateKeySize);

bool osslevp_key_is_rsa(EVP_PKEY *pkey);

#ifdef __cplusplus
}
#endif

#endif // SAILFISHCRYPTO_PLUGIN_CRYPTO_OPENSSL_EVP_P_H