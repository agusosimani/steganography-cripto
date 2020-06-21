#ifndef EXTRACT_H
#define EXTRACT_H

#include "encryption.h"

#define BYTES_IN_HEADER 54
#define FLENGTH_WORD_SIZE 4

void start_extraction(void);
//uint8_t * extract_LSB1(unsigned long * length_embeded_bytes);
void embeded_bytes_to_file(unsigned long bytes, uint8_t *bytes1);
uint32_t  extract_size_LSB1(FILE* bearer_file);
uint8_t * extract_LSB1(FILE* bearer_file, uint32_t  embeded_bytes_size);
char* extract_extension_LSB1(FILE *pFile, uint8_t* extension_length);

void generate_output_file(uint8_t *data, uint32_t data_size, char *extension, uint8_t extension_length);
char *extract_extension(FILE *pFile, uint8_t *length);

/* LSB4 */
uint8_t * extract_LSB4(FILE * , uint32_t );
char * extract_extension_LSB4(FILE * , uint8_t * );

uint8_t * extract_LSBI(FILE* bearer_file, unsigned long * length_embeded_bytes);

#endif //EXTRACT_H
