#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <stdint.h>

unsigned char * encrypt(uint8_t * plain_text, unsigned long length_plain_text, int * cipher_length);

#endif //ENCRYPTION_H
