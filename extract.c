#include "include/extract.h"

void start_extraction(void) {
    char * embeded_bytes;
    switch(stegobmp_config.steg) {
        case LSB1:
            embeded_bytes = extract_LSB1();
            break;
        case LSB4:
            embeded_bytes = extract_LSB4();
            break;
        case LSBI:
            embeded_bytes = extract_LSBI();
            break;
    }
    if (stegobmp_config.encrypt) {
        // TODO decrypt with functions in encryption.c
    }
    // TODO write embeded_bytes to stegobmp_config.out_bitmapfile
}

char * extract_LSB1(void) {
    // TODO
    return NULL;
}

char * extract_LSB4(void) {
    // TODO
    return NULL;
}

char * extract_LSBI(void) {
    // TODO
    return NULL;
}
