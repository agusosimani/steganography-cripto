#include "include/embed.h"

void start_embedding(void) {
    char * bytes_to_embed = get_bytes_to_embed();
    switch(stegobmp_config.steg) {
        case LSB1:
            embed_LSB1(bytes_to_embed);
            break;
        case LSB4:
            embed_LSB4(bytes_to_embed);
            break;
        case LSBI:
            embed_LSBI(bytes_to_embed);
            break;
    }
}

char * get_bytes_to_embed(void) {
    if (stegobmp_config.encrypt) {
        // TODO encrypt with functions in encryption.c
        // return Tamaño cifrado || encripcion(tamaño real || datos archivo || extensión)
    } else {
        // return Tamaño real || datos archivo || extensión
    }
    return NULL;
}

void embed_LSB1(char * bytes_to_embed) {
    // TODO
}

void embed_LSB4(char * bytes_to_embed) {
    // TODO
}

void embed_LSBI(char * bytes_to_embed) {
    // TODO
}



