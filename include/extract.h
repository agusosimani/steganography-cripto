#ifndef EXTRACT_H
#define EXTRACT_H

#include "encryption.h"

#define BLOCK 50

void start_extraction(void);
uint8_t * extract_LSB1(unsigned long * length_embeded_bytes);
uint8_t * extract_LSB4(unsigned long * length_embeded_bytes);
uint8_t * extract_LSBI(unsigned long * length_embeded_bytes);

#endif //EXTRACT_H
