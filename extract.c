#include "include/extract.h"

void start_extraction(void) {
    unsigned long length_embeded_bytes = 0;
    uint8_t * embeded_bytes = 0;

    switch(stegobmp_config.steg) {
        case LSB1:
            embeded_bytes = extract_LSB1(&length_embeded_bytes);
            break;
        case LSB4:
            embeded_bytes = extract_LSB4(&length_embeded_bytes);
            break;
        case LSBI:
            embeded_bytes = extract_LSBI(&length_embeded_bytes);
            break;
        case steg_undefined:
            fprintf(stderr, "Debe indicar el algoritmo de esteganografiado utilizado.\n");
            exit(EXIT_FAILURE);
    }

    if (stegobmp_config.encrypt) {
        int length_plain_text;
        unsigned char * plain_text = decrypt(embeded_bytes, length_embeded_bytes, &length_plain_text);
        free(embeded_bytes);
    }
    // TODO write embeded_bytes to stegobmp_config.out_bitmapfile
}

uint8_t * extract_LSB1(unsigned long * length_embeded_bytes) {
    // TODO
    return NULL;
}


uint8_t * extract_LSB4(unsigned long * length_embeded_bytes) {

    unsigned long index_embed = 0;
    unsigned long index_bearer = 0;
    unsigned long size_bearer = strlen(stegobmp_config.bearer);
    uint8_t mask = 0xF;
    uint8_t * embeded_bytes = calloc(BLOCK, sizeof(uint8_t));
    int blocks = 1;

    while (index_bearer < size_bearer) {

        /* para extraer 1 byte, necesitamos 2 bytes del bearer */

        uint8_t byte_to_extract_1 = stegobmp_config.bearer[index_bearer++];
        uint8_t embeded_byte_1 = mask & byte_to_extract_1;
        embeded_byte_1 = embeded_byte_1 << 4;

        uint8_t byte_to_extract_2, embeded_byte_2 = 0x0;
        if (index_bearer < size_bearer) {
            byte_to_extract_2 = stegobmp_config.bearer[index_bearer++];
            embeded_byte_2 = mask & byte_to_extract_2;
        }

        embeded_bytes[index_embed] = embeded_byte_1 | embeded_byte_2;
        index_embed++;

        if (index_embed >= BLOCK) {
            blocks++;
            uint8_t * embeded_bytes_aux = calloc(BLOCK*blocks, sizeof(uint8_t));
            memcpy(embeded_bytes_aux, embeded_bytes, index_embed);
            embeded_bytes = embeded_bytes_aux;
        }
    }

    *length_embeded_bytes = index_embed;

    return embeded_bytes;
}

uint8_t * extract_LSBI(unsigned long * length_embeded_bytes) {
    // TODO
    return NULL;
}
