#ifndef EMBED_H
#define EMBED_H

#include "main.h"

#define BYTES_IN_HEADER 54

void start_embedding(void);
uint8_t * get_bytes_to_embed(unsigned long * length_bytes_to_embed);
void embed_LSB1(uint8_t * bytes_to_embed, unsigned long length_bytes_to_embed);
void embed_LSB4(const uint8_t * bytes_to_embed, unsigned long length_bytes_to_embed);
void embed_LSBI(uint8_t * bytes_to_embed, unsigned long length_bytes_to_embed);

#endif //EMBED_H
