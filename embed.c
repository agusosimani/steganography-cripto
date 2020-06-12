#include "include/embed.h"

void start_embedding(void) {
    unsigned long length_bytes_to_embed;
    uint8_t * bytes_to_embed = get_bytes_to_embed(&length_bytes_to_embed);

    // Check if length_bytes_to_embed fits in bearer
    // TODO

    switch(stegobmp_config.steg) {
        case LSB1:
            embed_LSB1(bytes_to_embed, length_bytes_to_embed);
            break;
        case LSB4:
            embed_LSB4(bytes_to_embed, length_bytes_to_embed);
            break;
        case LSBI:
            embed_LSBI(bytes_to_embed, length_bytes_to_embed);
            break;
        case steg_undefined:
            fprintf(stderr, "Debe indicar el algoritmo de esteganografiado a utilizar.\n");
            exit(EXIT_FAILURE);
    }

    free(bytes_to_embed);
}

uint8_t * get_bytes_to_embed(unsigned long * length_bytes_to_embed) {

    FILE * fd_file_to_hide = fopen(stegobmp_config.file_to_hide, "rb");
    if (fd_file_to_hide == NULL) {
        fprintf(stderr, "No se pudo abrir el archivo %s.\n", stegobmp_config.file_to_hide);
        exit(EXIT_FAILURE);
    }

    // Get file_to_hide length
    fseek(fd_file_to_hide, 0L, SEEK_END);
    long length_file_to_hide = ftell(fd_file_to_hide);
    rewind(fd_file_to_hide);

    // Get data from file_to_hide
    uint8_t * buffer_file_to_hide = malloc((size_t) length_file_to_hide);
    fread(buffer_file_to_hide, (size_t) length_file_to_hide, 1, fd_file_to_hide);
    fclose(fd_file_to_hide);

    // Get file_to_hide extension
    char * extension = strrchr(stegobmp_config.file_to_hide, '.') + 1;
    if (!extension) {
        fprintf(stderr, "El archivo %s no tiene extensión.\n", stegobmp_config.file_to_hide);
        exit(EXIT_FAILURE);
    }

    // Get bytes_to_embed length (real size || data from file_to_hide || extension)
    *length_bytes_to_embed = sizeof(uint32_t) + length_file_to_hide + strlen(extension) + 1;
    uint8_t * bytes_to_embed = malloc((size_t) *length_bytes_to_embed);

    // Transform length_file_to_hide from decimal to hexa in Big Endian 32 bits representation
    uint32_t block_size = __bswap_32((unsigned)length_file_to_hide);

    // Generate bytes_to_embed (real size || data from file_to_hide || extension)
    memcpy(bytes_to_embed, &block_size, sizeof(uint32_t));
    memcpy(bytes_to_embed + sizeof(uint32_t), buffer_file_to_hide, (size_t)length_file_to_hide);
    memcpy(bytes_to_embed + sizeof(uint32_t) + length_file_to_hide, extension, strlen(extension) + 1);
    free(buffer_file_to_hide);

    if (stegobmp_config.encrypt) {
        // Get cipher
        int length_cipher;
        unsigned char * cipher = encrypt(bytes_to_embed, *length_bytes_to_embed, &length_cipher);
        free(bytes_to_embed);

        // Transform length_cipher from decimal to hexa in Big Endian 32 bits representation
        uint32_t cipher_block_size = __bswap_32((unsigned) length_cipher);

        // Generate bytes_to_embed (cipher size || cipher(real size || data from file_to_hide || extension))
        uint8_t * cipher_bytes_to_embed = malloc(length_cipher + sizeof(uint32_t));
        memcpy(cipher_bytes_to_embed, &cipher_block_size, sizeof(uint32_t));
        memcpy(cipher_bytes_to_embed + sizeof(uint32_t), cipher, (size_t) length_cipher);
        free(cipher);
        free(buffer_file_to_hide);

        *length_bytes_to_embed = length_cipher + sizeof(uint32_t);
        return cipher_bytes_to_embed;

    } else {

        return bytes_to_embed;
    }
}

void embed_LSB1(uint8_t * bytes_to_embed, unsigned long length_bytes_to_embed) {
    // TODO
}

void embed_LSB4(uint8_t * bytes_to_embed, unsigned long length_bytes_to_embed) {
    // TODO
}

void embed_LSBI(uint8_t * bytes_to_embed, unsigned long length_bytes_to_embed) {
    // TODO
}



