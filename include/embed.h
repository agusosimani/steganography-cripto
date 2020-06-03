#ifndef EMBED_H
#define EMBED_H

#include "main.h"

void start_embedding(void);
char * get_bytes_to_embed(void);
void embed_LSB1(char * bytes_to_embed);
void embed_LSB4(char * bytes_to_embed);
void embed_LSBI(char * bytes_to_embed);

#endif //EMBED_H
