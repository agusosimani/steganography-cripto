#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <openssl/evp.h>

#include "main.h"

#define N 256

unsigned char * encrypt_(uint8_t * plain_text, unsigned long length_plain_text, int * cipher_length);
unsigned char * decrypt (uint8_t * cipher, unsigned long length_cipher, int * length_plain_text);
uint8_t * encrypt_rc4(const uint8_t * plain_text, unsigned long length_plain_text, const uint8_t * key, unsigned long length_key);
uint8_t * decrypt_rc4(const uint8_t * cipher, unsigned long length_cipher, const uint8_t * key, unsigned long length_key);

#endif //ENCRYPTION_H
