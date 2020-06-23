#ifndef EXTRACT_H
#define EXTRACT_H

#include "encryption.h"

#define BYTES_IN_HEADER 54
#define FLENGTH_WORD_SIZE 4

void start_extraction(void);
void embeded_bytes_to_file(unsigned long bytes, uint8_t *bytes1);
uint32_t  extract_size_LSB1(FILE* bearer_file);
uint8_t * extract_LSB1(FILE* bearer_file, uint32_t  embeded_bytes_size, uint64_t * index_bearer);
char* extract_extension_LSB1(FILE *pFile, uint8_t* extension_length, uint64_t * index_bearer);

void generate_output_file(uint8_t *data, uint32_t data_size, char *extension, uint8_t extension_length);
char *extract_extension(FILE *pFile, uint8_t *length);

/* LSB4 */
uint8_t * extract_LSB4(FILE *, uint32_t, uint64_t *);
char * extract_extension_LSB4(FILE * , uint8_t *, uint64_t *);

uint8_t * extract_LSBI(FILE* bearer_file, uint32_t * length_embeded_bytes, char** embeded_bytes_extension, uint8_t* embeded_bytes_extension_size);
uint8_t *extract_LSBI_aux(uint8_t * bearer, uint64_t size_of_bearer,  uint32_t hop, uint64_t n_elems, size_t size_of_elem, int* cycle_pos, uint64_t* extracting_pos);
uint32_t get_msb_order( uint8_t byte);
uint32_t to_big_endian(uint8_t *aux);
long file_size (FILE * fp);

#endif //EXTRACT_H
