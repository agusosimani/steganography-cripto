#include "include/embed.h"

const uint8_t unmask_array[8] = {UNMASK_BIT_0, UNMASK_BIT_1, UNMASK_BIT_2, UNMASK_BIT_3, UNMASK_BIT_4, UNMASK_BIT_5, UNMASK_BIT_6, UNMASK_BIT_7};
const uint8_t mask_array[8] = {MASK_BIT_0, MASK_BIT_1, MASK_BIT_2, MASK_BIT_3, MASK_BIT_4, MASK_BIT_5, MASK_BIT_6, MASK_BIT_7};
uint64_t select_output_byte (FILE * bearer_file, uint8_t hoop, uint64_t jump_from, int * cycles,
                             uint64_t * index_out_bytes, uint64_t size);
long size_of_file (FILE * fp);

void start_embedding(void) {
    // Check if file_to_hide fits in bearer
    // TODO
    // cambia dependiendo del algoritmo, asique hagámoslo en los métodos lsb1, lsb4 y lsbi
    unsigned long length_bytes_to_embed;
    uint8_t * bytes_to_embed = get_bytes_to_embed(&length_bytes_to_embed);

    FILE* bearer_file = fopen(stegobmp_config.bearer, "rb");
    FILE* out_file = fopen(stegobmp_config.out_bitmapfile, "wb");

    switch(stegobmp_config.steg) {
        case LSB1:
            embed_LSB1(bearer_file, out_file, bytes_to_embed, length_bytes_to_embed);
            break;
        case LSB4:
            embed_LSB4(bearer_file, out_file, bytes_to_embed, length_bytes_to_embed);
            break;
        case LSBI:
            embed_LSBI(bearer_file, out_file, bytes_to_embed, length_bytes_to_embed);
            break;
        case steg_undefined:
            fprintf(stderr, "Debe indicar el algoritmo de esteganografiado a utilizar.\n");
            exit(EXIT_FAILURE);
    }

    fclose(bearer_file);
    fclose(out_file);
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
    char * extension = strrchr(stegobmp_config.file_to_hide, '.');
    if (!extension) {
        fprintf(stderr, "El archivo %s no tiene extensión.\n", stegobmp_config.file_to_hide);
        exit(EXIT_FAILURE);
    }

    // Get bytes_to_embed length (real size || data from file_to_hide || extension)
    *length_bytes_to_embed = sizeof(uint32_t) + length_file_to_hide + strlen(extension) + 1;
    uint8_t * bytes_to_embed = malloc((size_t) *length_bytes_to_embed);

    // Transform length_file_to_hide from decimal to hexa in Big Endian 32 bits representation
    uint32_t block_size = bswap_32((unsigned)length_file_to_hide);

    // Generate bytes_to_embed (real size || data from file_to_hide || extension)
    memcpy(bytes_to_embed, &block_size, sizeof(uint32_t));
    memcpy(bytes_to_embed + sizeof(uint32_t), buffer_file_to_hide, (size_t)length_file_to_hide);
    memcpy(bytes_to_embed + sizeof(uint32_t) + length_file_to_hide, extension, strlen(extension) + 1);
    free(buffer_file_to_hide);

    if (stegobmp_config.encrypt) {
        // Get cipher
        int length_cipher;
        unsigned char * cipher = encrypt_ (bytes_to_embed, *length_bytes_to_embed, &length_cipher);
        free(bytes_to_embed);

        // Transform length_cipher from decimal to hexa in Big Endian 32 bits representation
        uint32_t cipher_block_size = bswap_32((unsigned) length_cipher);

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

void embed_LSB1(FILE* bearer_file, FILE* out_file, uint8_t* bytes_to_embed, unsigned long length_bytes_to_embed) {
    validate_sizes(bearer_file, length_bytes_to_embed, 8);
    char c;
    uint8_t embeded_bits_in_byte = 0;
    unsigned long embeded_bytes = 0;
    unsigned long header_bytes = 0;
    while (((c = fgetc(bearer_file)) || 1) && !feof(bearer_file)) {
        if(embeded_bytes < length_bytes_to_embed && header_bytes >= BYTES_IN_HEADER) {
            uint8_t new_byte = ((c & ~0x1) | (uint8_t)((bytes_to_embed[embeded_bytes] >> (7 - embeded_bits_in_byte)) & 1));
            fputc(new_byte, out_file);
            embeded_bits_in_byte++;
            if(embeded_bits_in_byte % 8 == 0) {
                embeded_bytes++;
                embeded_bits_in_byte = 0;
            }
        } else {
            fputc(c, out_file);
        }
        header_bytes++;
    }
}

void validate_sizes(FILE *bearer_file, unsigned long length_bytes_to_embed, int bytes_per_byte) {
    fseek(bearer_file, 0L, SEEK_END);
    long int bearer_file_size = ftell(bearer_file);
    fseek(bearer_file, 0L, SEEK_SET);

    if(length_bytes_to_embed > (bearer_file_size - BYTES_IN_HEADER)/bytes_per_byte) {
        fprintf(stderr, "El tamaño del archivo portador no es suficiente para embeber al mensaje.\n");
        exit(EXIT_FAILURE);
    }
}

void embed_LSB4 (FILE* bearer_file, FILE* out_file, const uint8_t * bytes_to_embed, unsigned long length_bytes_to_embed) {

    uint64_t size_bearer = size_of_file(bearer_file);

     /*para que se pueda ocultar el mensaje en el bmp, por cada byte del mensaje se necesitan 2 bytes del bmp*/
    if ((size_bearer - BYTES_IN_HEADER)*2 < length_bytes_to_embed) {
        fprintf(stderr, "ERROR: No hay capacidad para ocultar el mensaje, el tamaño disponible es %lu", (size_bearer - BYTES_IN_HEADER) / 2);
        return;
    }

    uint64_t index_embed = 0;
    uint8_t mask = 0xF0;

    int header_bytes = 0;
    while (header_bytes < BYTES_IN_HEADER) {
        uint8_t unmodified_pixel = fgetc(bearer_file);
        fputc(unmodified_pixel, out_file);
        header_bytes++;
    }

    while (index_embed < length_bytes_to_embed) {

        uint8_t byte_to_embed = bytes_to_embed[index_embed];
        /* para esconder este byte, necesitamos 2 bytes del bearer */
        for (int i = 1; i >= 0; i--) {

            uint8_t new_byte = byte_to_embed & (0xF << i * 4);
            new_byte = new_byte >> (i * 4);

            /*uint8_t new_pixel = stegobmp_config.bearer[index_bearer];
            new_pixel = (new_pixel & mask) | new_byte;
            stegobmp_config.bearer[index_bearer] = new_pixel;
            index_bearer++;*/

            /* 1/3 pixel = unsigned char de 1 byte (0 a 255) */
            uint8_t new_pixel = fgetc(bearer_file);
            new_pixel = (new_pixel & mask) | new_byte;
            fputc(new_pixel, out_file);
        }

        index_embed++;
    }

    while (!feof(bearer_file)) {
        uint8_t unmodified_pixel = fgetc(bearer_file);
        fputc(unmodified_pixel, out_file);
    }
}

void embed_LSBI(FILE* bearer_file, FILE* out_file, uint8_t * bytes_to_embed, unsigned long length_bytes_to_embed) {

    /* salteamos los bytes del header */
    int header_bytes = 0;
    while (header_bytes < BYTES_IN_HEADER) {
        uint8_t unmodified_pixel = fgetc(bearer_file);
        fputc(unmodified_pixel, out_file);
        header_bytes++;
    }

    /* usaremos como clave los 6 primeros bytes (48 bits) de la imagen portadora */
    uint8_t key_bytes = 0;
    uint8_t * key = malloc(BYTES_IN_KEY);
    while (key_bytes < BYTES_IN_KEY) {
        uint8_t key_byte = fgetc(bearer_file);
        fputc(key_byte, out_file);                  /* not sure */
        key[key_bytes++] = key_byte;
    }

    uint8_t hoop = key[0];
    uint8_t aux = 0;
    bool found = false;
    for (int i = 0; i < 8 && !found; i++) {
        if (!found && ((hoop << i) & 0x80) != 0) {
            aux = mask_array[7-i];
            found = true;
        }
    }
    hoop = aux;
    if (hoop == 0) {
        hoop = mask_array[7] * 2;
    }
    /* hoop puede ser cualquiera de [2, 4, 8, 16, 32, 64, 128, 256] */

    hoop = 2;

    // TODO armar el mensaje a encriptar
    // TODO encriptar con RC4

    uint64_t size_bearer = size_of_file(bearer_file) - FIRST_READ_BYTE;

    /* para ocultar el mensaje en el bmp, puedo usar todos los bytes disponibles del bearer */
    if (size_bearer < length_bytes_to_embed) {
        fprintf(stderr, "ERROR: No hay capacidad para ocultar el mensaje, el tamaño disponible es %lu", size_bearer);
        return;
    }

    /* con el mensaje encriptado, lo ocultamos en el archivo bmp */

    uint64_t index_bytes_embed = 0;
    uint64_t index_bits_embed = 7;
    uint64_t jump_to = 0;
    uint64_t index_out_bytes = FIRST_READ_BYTE;
    int cycles = 0;
    while (index_bytes_embed < length_bytes_to_embed) {

        uint8_t byte_to_embed = bytes_to_embed[index_bytes_embed];
        uint8_t bit_to_embed = (byte_to_embed & mask_array[index_bits_embed]) >> index_bits_embed;
        index_bits_embed--;
        if (index_bits_embed < 0 || index_bits_embed > 7) {
            index_bits_embed = 7;
            index_bytes_embed++;
        }

        jump_to = select_output_byte(bearer_file, hoop, jump_to, &cycles, &index_out_bytes, size_bearer);
        uint8_t new_pixel;
        /* iteramos hasta el indice jump_to, donde modificaremos el pixel o byte */
        while (index_out_bytes < jump_to) {
            if (cycles == 0) {
                new_pixel = fgetc(bearer_file);
                fputc(new_pixel, out_file);
            } else {
                /* si no es el primer ciclo, itero por los bytes del out_file hasta buscar el pixel que quiero */
                fgetc(out_file);
            }
            index_out_bytes++;
        }
        new_pixel = fgetc(bearer_file);
        new_pixel = (new_pixel & unmask_array[cycles]) | bit_to_embed;
        fputc(new_pixel, out_file);

    }

    /* si hay bytes del bearer que no terminaron de copiarse */
    if (cycles == 0 && jump_to < size_bearer) {
        while (!feof(bearer_file)) {
            uint8_t unmodified_pixel = fgetc(bearer_file);
            fputc(unmodified_pixel, out_file);
        }
    }

}

uint64_t select_output_byte (FILE * bearer_file, uint8_t hoop, uint64_t jump_from, int * cycles,
                             uint64_t * index_out_bytes, uint64_t size) {

    if (jump_from == 0) {
        return FIRST_READ_BYTE;
    }

    uint64_t jump = jump_from + hoop;
    if (jump >= size) {

        jump = (jump - size) + FIRST_READ_BYTE;
        *cycles = *cycles + 1;
        *index_out_bytes = FIRST_READ_BYTE;
        int restart_bearer = 0;
        while (restart_bearer < FIRST_READ_BYTE) {
            fgetc(bearer_file);
            restart_bearer++;
        }
    }

    return jump;
}

long size_of_file (FILE * fp) {

    fseek(fp, 0L, SEEK_END);

    // calculating the size of the file
    long int res = ftell(fp);

    // go back to the beginning
    rewind(fp);

    return res;
}
