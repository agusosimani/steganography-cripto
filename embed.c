#include "include/embed.h"

void start_embedding(void) {
    // Check if file_to_hide fits in bearer
    // TODO
    // cambia dependiendo del algoritmo, asique hagámoslo en los métodos lsb1, lsb4 y lsbi
    unsigned long length_bytes_to_embed;
    uint8_t * bytes_to_embed = get_bytes_to_embed(&length_bytes_to_embed);

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

void embed_LSB4(const uint8_t * bytes_to_embed, unsigned long length_bytes_to_embed) {

    unsigned long size_bearer = strlen(stegobmp_config.bearer);

    /* para que se pueda ocultar el mensaje en el bmp, por cada byte del mensaje se necesitan 2 bytes del bmp */
    if (size_bearer*2 < length_bytes_to_embed) {
        fprintf(stderr, "ERROR: No hay capacidad para ocultar el mensaje, el tamaño disponible es %lu", size_bearer / 2);
        return;
    }

    unsigned long index_embed = 0;
    unsigned long index_bearer = 0;
    uint8_t mask = 0xF0;

    while (index_embed < length_bytes_to_embed) {

        uint8_t byte_to_embed = bytes_to_embed[index_embed];
        /* para esconder este byte, necesitamos 2 bytes del bearer */
        for (int i = 1; i >= 0; i--) {

            uint8_t new_byte = byte_to_embed & (0xF << i * 4);
            new_byte = new_byte >> (i * 4);

            /* pixel = unsigned char de 1 byte (0 a 255) */
            uint8_t new_pixel = stegobmp_config.bearer[index_bearer];
            new_pixel = (new_pixel & mask) | new_byte;
            stegobmp_config.bearer[index_bearer] = new_pixel;
            index_bearer++;
        }

        index_embed++;
    }
}

/*int main (void) {
    // Default values
    stegobmp_config.operation = operation_undefined;
    stegobmp_config.file_to_hide = "abcd";
    stegobmp_config.bearer = malloc(13);
    strcpy(stegobmp_config.bearer, "aaaaaaaaaaaa");
    stegobmp_config.out_bitmapfile = "";
    stegobmp_config.steg = steg_undefined;
    stegobmp_config.encrypt = false;
    stegobmp_config.enc_algorithm = "aes128";
    stegobmp_config.enc_mode = "cbc";

    uint8_t bytes_to_embed[] = {'a', 'b', 'c', 'd'};
    unsigned long length_bytes_to_embed = 4;

    embed_LSB4(bytes_to_embed, length_bytes_to_embed);

    printf("%s\n", stegobmp_config.bearer);
    *//*for (int j = 0; j < strlen(stegobmp_config.bearer); j++ ) {
        char a = stegobmp_config.bearer[j];
        for (int i = 0; i < 8; i++) {
            printf("%d", !!((a << i) & 0x80));
        }
        printf("\n");
    }*//*

    unsigned long * lenght = malloc(sizeof(unsigned long));
    uint8_t * embeded_bytes = extract_LSB4(lenght);
    printf("%s\n", embeded_bytes);
}*/

void embed_LSBI(uint8_t * bytes_to_embed, unsigned long length_bytes_to_embed) {
    // TODO
}



