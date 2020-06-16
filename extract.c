#include "include/extract.h"

uint32_t to_big_endian(uint8_t *aux);

void start_extraction(void) {
    unsigned long length_embeded_bytes = 0;
    uint8_t * embeded_bytes = 0;
    uint8_t * embeded_bytes_data = 0;
    uint32_t embeded_bytes_data_size = 0;
    char * embeded_bytes_extension = 0;
    uint8_t embeded_bytes_extension_size = 0;

    //Se abre la foto portadora del mensaje y se avanza hasta después del header porque ahí no hay información escondida
    FILE* bearer_file = fopen(stegobmp_config.bearer, "rb");
    fseek(bearer_file, BYTES_IN_HEADER, SEEK_SET);

    switch(stegobmp_config.steg) {
        case LSB1:
            embeded_bytes_data_size = to_big_endian(extract_LSB1(bearer_file, FLENGTH_WORD_SIZE));
            embeded_bytes_data = extract_LSB1(bearer_file, embeded_bytes_data_size);
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

    embeded_bytes_extension = extract_extension(bearer_file, &embeded_bytes_extension_size);
    generate_output_file(embeded_bytes_data, embeded_bytes_data_size, embeded_bytes_extension, embeded_bytes_extension_size);

    fclose(bearer_file);
    free(embeded_bytes_extension);
    free(embeded_bytes_data);
}

uint32_t to_big_endian(uint8_t *aux) {
    return (aux[0] << 24) + (aux[1] << 16) + (aux[2] << 8) + aux[3];
}

char *extract_extension(FILE* bearer_file, uint8_t* extension_length) {
    switch(stegobmp_config.steg) {
        case LSB1:
            return extract_extension_LSB1(bearer_file, extension_length);
        case LSB4:
            break;
        case LSBI:
            break;
    }
}

uint8_t* extract_LSB1(FILE* bearer_file, uint32_t  embeded_bytes_size){
    uint8_t* hidden_message = calloc(embeded_bytes_size + 1 ,sizeof(uint8_t));
    uint32_t byte_counter32 = 0;
    uint8_t read_byte;

    while (byte_counter32 < embeded_bytes_size) {
        for (int i = 0; i < 8; i++) {
            read_byte = (uint8_t) fgetc(bearer_file);
            uint8_t new_bit = (uint8_t) ((read_byte  & 0x1) << (7 - i));
            hidden_message[byte_counter32] =  hidden_message[byte_counter32] | new_bit;
        }
        byte_counter32++;
    }

    hidden_message[byte_counter32] = 0;
    return hidden_message;
}

char* extract_extension_LSB1(FILE* bearer_file, uint8_t* extension_length) {
    uint8_t byte_counter8 = 0;
    uint8_t read_byte;
    char* extension = calloc(15, sizeof(char));
    bool end = false;
    while (!end) {
        for (int i = 0; i < 8; i++) {
            read_byte = (uint8_t) fgetc(bearer_file);
            uint8_t new_bit = (uint8_t) ((read_byte  & 0x1) << (7 - i));
            extension[byte_counter8] =  extension[byte_counter8] | new_bit;
        }
        if(extension[byte_counter8] == 0){
            end = true;
        }
        byte_counter8++;
    }
    *extension_length = byte_counter8;
    return extension;
}

void generate_output_file(uint8_t *data, uint32_t data_size, char *extension, uint8_t extension_length) {
    char* output_file_name  = malloc(strlen (stegobmp_config.out_bitmapfile) + 1 + extension_length);
    memcpy(output_file_name, stegobmp_config.out_bitmapfile, strlen(stegobmp_config.out_bitmapfile) + 1);
    strcat(output_file_name, extension);

    FILE* output_file = fopen(output_file_name, "w");
    fwrite(data , 1, data_size, output_file);

    fclose(output_file);
    free(output_file_name);
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
