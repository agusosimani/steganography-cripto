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
    // TODO
    return NULL;
}

uint8_t * extract_LSBI(unsigned long * length_embeded_bytes) {
    // TODO
    return NULL;
}
