#ifndef EMBED_H
#define EMBED_H

#include "encryption.h"
#include "byteswap.h"

#define UNMASK_BIT_0 0xFE               /* 11111110 */
#define UNMASK_BIT_1 0xFD               /* 11111101 */
#define UNMASK_BIT_2 0xFB               /* 11111011 */
#define UNMASK_BIT_3 0xF7               /* 11110111 */
#define UNMASK_BIT_4 0xEF               /* 11101111 */
#define UNMASK_BIT_5 0xDF               /* 11011111 */
#define UNMASK_BIT_6 0xBF               /* 10111111 */
#define UNMASK_BIT_7 0x7F               /* 01111111 */

#define MASK_BIT_0 0x1                  /* 00000001 */
#define MASK_BIT_1 0x2                  /* 00000010 */
#define MASK_BIT_2 0x4                  /* 00000100 */
#define MASK_BIT_3 0x8                  /* 00001000 */
#define MASK_BIT_4 0x10                 /* 00010000 */
#define MASK_BIT_5 0x20                 /* 00100000 */
#define MASK_BIT_6 0x40                 /* 01000000 */
#define MASK_BIT_7 0x80                 /* 10000000 */

#define BYTES_IN_HEADER 54

void start_embedding(void);
uint8_t * get_bytes_to_embed(unsigned long * length_bytes_to_embed);
void embed_LSB1(FILE *pFile, FILE *pFile1, uint8_t *string, unsigned long i);
void embed_LSB4(const uint8_t * bytes_to_embed, unsigned long length_bytes_to_embed);
void embed_LSBI(uint8_t * bytes_to_embed, unsigned long length_bytes_to_embed);
void validate_sizes(FILE *pFile, unsigned long embed);

#endif //EMBED_H
