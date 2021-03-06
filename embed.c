#include "include/embed.h"

const uint8_t mask_array[8] = {MASK_BIT_0, MASK_BIT_1, MASK_BIT_2, MASK_BIT_3, MASK_BIT_4, MASK_BIT_5, MASK_BIT_6, MASK_BIT_7};
uint64_t select_output_byte (uint8_t hoop, uint64_t jump_from, int * cycles, uint64_t size);
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

    uint64_t size_of_bearer = size_of_file(bearer_file);
    char * bearer_to_embed = malloc(size_of_bearer);
    size_t number_of_bytes_read = fread(bearer_to_embed, sizeof(char), size_of_bearer, bearer_file);
    if (size_of_bearer != number_of_bytes_read) {
        fprintf(stderr, "ERROR: La lectura del archivo portador falló.\n");
        return;
    }

    if (bearer_file == NULL) {
        fprintf(stderr, "ERROR: El archivo portador no es válido.\n");
        return;
    }

    if (out_file == NULL) {
        fprintf(stderr, "ERROR: El path del archivo de salida es inválido.\n");
        return;
    }

     /*para que se pueda ocultar el mensaje en el bmp, por cada byte del mensaje se necesitan 2 bytes del bmp*/
    if ((size_of_bearer - BYTES_IN_HEADER) / 2 < length_bytes_to_embed) {
        fprintf(stderr, "ERROR: No hay capacidad para ocultar el mensaje, el tamaño disponible es %llu.\n", (size_of_bearer - BYTES_IN_HEADER) / 2);
        return;
    }

    uint64_t index_embed = 0;
    uint8_t mask = 0xF0;
    uint64_t index_bearer = BYTES_IN_HEADER;

    while (index_embed < length_bytes_to_embed) {

        uint8_t byte_to_embed = bytes_to_embed[index_embed];
        /* para esconder este byte, necesitamos 2 bytes del bearer */
        for (int i = 1; i >= 0; i--) {

            uint8_t new_byte = byte_to_embed & (0xF << i * 4);
            new_byte = new_byte >> (i * 4);

            /* 1/3 pixel = unsigned char de 1 byte (0 a 255) */
            uint8_t new_pixel = bearer_to_embed[index_bearer];
            new_pixel = (new_pixel & mask) | new_byte;
            bearer_to_embed[index_bearer] = new_pixel;
            index_bearer++;
        }

        index_embed++;
    }

    fwrite(bearer_to_embed, sizeof(char), size_of_bearer, out_file);
    free(bearer_to_embed);
}

void embed_LSBI(FILE* bearer_file, FILE* out_file, uint8_t * bytes_to_embed, unsigned long length_bytes_to_embed) {

    if (bearer_file == NULL) {
        fprintf(stderr, "ERROR: El archivo portador no es válido.\n");
        return;
    }

    if (out_file == NULL) {
        fprintf(stderr, "ERROR: El path del archivo de salida es inválido.\n");
        return;
    }

    uint64_t size_of_bearer = size_of_file(bearer_file);
    char * bearer_to_embed = malloc(size_of_bearer);
    size_t number_of_bytes_read = fread(bearer_to_embed, sizeof(char), size_of_bearer, bearer_file);
    if (size_of_bearer != number_of_bytes_read) {
        fprintf(stderr, "ERROR: La lectura del archivo portador falló.\n");
        return;
    }

    /* usaremos como clave los 6 primeros bytes (48 bits) de la imagen portadora */
    uint8_t * key = malloc(BYTES_IN_KEY);
    uint8_t key_bytes = 0;
    while (key_bytes < BYTES_IN_KEY ) {
        uint8_t byte = bearer_to_embed[key_bytes + BYTES_IN_HEADER];
        key[key_bytes] = byte;
        key_bytes++;
    }

    uint32_t hoop = key[0];
    uint32_t aux = 0;
    bool found = false;
    for (int i = 0; i < 8 && !found; i++) {
        if (!found && ((hoop << i) & MASK_BIT_7) != 0) {
            aux = mask_array[7-i];
            found = true;
        }
    }
    hoop = aux;
    if (hoop == 0) {
        hoop = mask_array[7] * 2;
    }
    /* hoop puede ser cualquiera de [2, 4, 8, 16, 32, 64, 128, 256] */

    uint8_t * bytes_to_embed_encrypted = encrypt_rc4(bytes_to_embed, length_bytes_to_embed, key, BYTES_IN_KEY);
    free(key);

    /* para ocultar el mensaje en el bmp, para 1 byte del mensaje necesito 8 del bearer */
    if ((size_of_bearer - FIRST_READ_BYTE) / 8 < length_bytes_to_embed) {
        fprintf(stderr, "ERROR: No hay capacidad para ocultar el mensaje, el tamaño disponible es %llu.\n", (size_of_bearer - FIRST_READ_BYTE) / 8);
        return;
    }

    /* con el mensaje encriptado, lo ocultamos en el archivo bmp */

    uint64_t index_bytes_bearer_modified = 0;
    uint64_t index_bytes_embed = 0;                         /* cuenta la cantidad de bytes embebidos */
    uint64_t index_bits_embed = 7;                          /* posicion del bit a embeber */
    uint64_t jump_to = 0;                                   /* indice del out_file donde se ocultará el bit, inicializado en 0 para luego modificarlo a FIRST_READ_BYTE */
    int cycles = 0;
    while (index_bytes_embed < length_bytes_to_embed) {

        /* buscamos el bit mas significativo del byte a ocultar y lo ubicamos en el lugar menos significativo */
        uint8_t byte_to_embed = bytes_to_embed_encrypted[index_bytes_embed];
        uint8_t bit_to_embed = (byte_to_embed & mask_array[index_bits_embed]) >> index_bits_embed;
        /* segun el ciclo, posicionamos el bit donde no pise bits ya ocultados dentro del byte de out_file */
        /*if (index_bytes_bearer_modified > size_of_bearer - FIRST_READ_BYTE) {
            bit_to_embed = bit_to_embed << cycles;
        }*/
        index_bits_embed--;
        if (index_bits_embed < 0 || index_bits_embed > 7) {
            index_bits_embed = 7;
            index_bytes_embed++;
        }

        /* buscamos el indice del out_file donde queremos esconder el bit */
        jump_to = select_output_byte(hoop, jump_to, &cycles, size_of_bearer);
        uint8_t new_pixel = bearer_to_embed[jump_to];
        new_pixel = (new_pixel & UNMASK_BIT_0) | bit_to_embed;
        bearer_to_embed[jump_to] = new_pixel;
        /*index_bytes_bearer_modified++;
        if (index_bytes_bearer_modified % (size_of_bearer - FIRST_READ_BYTE)  == 0) {
            cycles = cycles + 1;
        }*/

    }

    fwrite(bearer_to_embed, sizeof(char), size_of_bearer, out_file);
    free(bearer_to_embed);
    free(bytes_to_embed_encrypted);
}

uint64_t select_output_byte (uint8_t hoop, uint64_t jump_from, int * cycles, uint64_t size) {

    if (jump_from == 0) {
        return FIRST_READ_BYTE;
    }

    uint64_t jump = jump_from + hoop;
    if (jump >= size) {

        *cycles = *cycles + 1;
        jump = *cycles + FIRST_READ_BYTE;
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
