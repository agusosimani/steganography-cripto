#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <openssl/evp.h>

#include "main.h"

unsigned char * encrypt_ (uint8_t * plain_text, unsigned long length_plain_text, int * cipher_length);
unsigned char * decrypt (uint8_t * cipher, unsigned long length_cipher, int * length_plain_text);

#endif //ENCRYPTION_H
