#include "include/extract.h"
#include<math.h>

void start_extraction(void) {
    uint32_t length_embeded_bytes = 0;
    uint8_t * embeded_bytes = 0;
    char * embeded_bytes_extension = 0;
    uint8_t embeded_bytes_extension_size = 0;
    uint64_t index_bearer = BYTES_IN_HEADER;

    //Se abre la foto portadora del mensaje y se avanza hasta después del header porque ahí no hay información escondida
    FILE* bearer_file = fopen(stegobmp_config.bearer, "rb");
    fseek(bearer_file, BYTES_IN_HEADER, SEEK_SET);

    switch(stegobmp_config.steg) {
        case LSB1:
            length_embeded_bytes = to_big_endian(extract_LSB1(bearer_file, FLENGTH_WORD_SIZE, &index_bearer));
            embeded_bytes = extract_LSB1(bearer_file, length_embeded_bytes, &index_bearer);
            if (!stegobmp_config.encrypt)
                embeded_bytes_extension = extract_extension_LSB1(bearer_file, &embeded_bytes_extension_size, &index_bearer);
            break;
        case LSB4:
            length_embeded_bytes = to_big_endian(extract_LSB4(bearer_file, FLENGTH_WORD_SIZE, &index_bearer));
            embeded_bytes = extract_LSB4(bearer_file, length_embeded_bytes, &index_bearer);
            if (!stegobmp_config.encrypt) {
                embeded_bytes_extension = extract_extension_LSB4(bearer_file,  &embeded_bytes_extension_size, &index_bearer);
            }
            break;
        case LSBI:
            embeded_bytes = extract_LSBI(bearer_file, &length_embeded_bytes, &embeded_bytes_extension, &embeded_bytes_extension_size);
            break;
        case steg_undefined:
            fprintf(stderr, "Debe indicar el algoritmo de esteganografiado utilizado.\n");
            exit(EXIT_FAILURE);
    }

    if (stegobmp_config.encrypt) {
        int length_plain_text;
        unsigned char * plain_text = decrypt(embeded_bytes, length_embeded_bytes, &length_plain_text);
        free(embeded_bytes);

        //Extraigo el tamaño
        memcpy(&length_embeded_bytes, plain_text, sizeof(uint32_t));
        length_embeded_bytes = bswap_32(length_embeded_bytes);

        //Extraigo la data
        embeded_bytes = plain_text + sizeof(uint32_t);

        //Extraigo la extensión
        embeded_bytes_extension = (char *) embeded_bytes + length_embeded_bytes;
        embeded_bytes_extension_size = strlen(embeded_bytes_extension);

        embeded_bytes_extension[embeded_bytes_extension_size] = 0;
    }

    generate_output_file(embeded_bytes, length_embeded_bytes, embeded_bytes_extension, embeded_bytes_extension_size);

    fclose(bearer_file);
    if (!stegobmp_config.encrypt) {
        free(embeded_bytes_extension);
        free(embeded_bytes);
    }
}

uint32_t to_big_endian(uint8_t *aux) {
    uint32_t ret = (aux[0] << 24) + (aux[1] << 16) + (aux[2] << 8) + aux[3];
    free(aux);
    return ret;
}

uint8_t* extract_LSB1(FILE* bearer_file, uint32_t  embeded_bytes_size, uint64_t * index_bearer){

    uint64_t size_of_bearer = size_of_file(bearer_file);
    char * bearer_to_read = malloc(size_of_bearer);
    size_t number_of_bytes_read = fread(bearer_to_read, sizeof(char), size_of_bearer, bearer_file);

    if (size_of_bearer != number_of_bytes_read) {
        fprintf(stderr, "ERROR: La lectura del archivo portador falló.\n");
        return NULL;
    }

    if (size_of_bearer < embeded_bytes_size) {
        fprintf(stderr, "ERROR: El tamaño del portador es menor al mensaje que se desea extraer.\n");
        return NULL;
    }

    uint8_t* hidden_message = calloc(embeded_bytes_size + 1 ,sizeof(uint8_t));
    uint32_t byte_counter32 = 0;
    uint8_t read_byte;

    while (byte_counter32 < embeded_bytes_size) {
        for (int i = 0; i < 8; i++) {
            read_byte = bearer_to_read[*index_bearer];
            *index_bearer = *index_bearer + 1;
            uint8_t new_bit = (uint8_t) ((read_byte  & 0x1) << (7 - i));
            hidden_message[byte_counter32] =  hidden_message[byte_counter32] | new_bit;
        }
        byte_counter32++;
    }

    hidden_message[byte_counter32] = 0;
    free(bearer_to_read);
    return hidden_message;
}

char * extract_extension_LSB1(FILE* bearer_file, uint8_t* extension_length, uint64_t * index_bearer) {

    uint64_t size_of_bearer = size_of_file(bearer_file);
    char * bearer_to_read = malloc(size_of_bearer);
    size_t number_of_bytes_read = fread(bearer_to_read, sizeof(char), size_of_bearer, bearer_file);

    uint8_t byte_counter8 = 0;
    uint8_t read_byte;
    char* extension = calloc(15, sizeof(char));
    bool end = false;
    while (!end && byte_counter8 < 15) {
        for (int i = 0; i < 8; i++) {
            read_byte = (uint8_t) bearer_to_read[*index_bearer];
            *index_bearer = *index_bearer + 1;
            uint8_t new_bit = (uint8_t) ((read_byte  & 0x1) << (7 - i));
            extension[byte_counter8] =  extension[byte_counter8] | new_bit;
        }
        if(extension[byte_counter8] == 0){
            end = true;
        }
        byte_counter8++;
    }
    *extension_length = byte_counter8;
    free(bearer_to_read);
    return extension;
}

void generate_output_file(uint8_t *data, uint32_t data_size, char *extension, uint8_t extension_length) {
    char* output_file_name  = malloc(strlen (stegobmp_config.out_bitmapfile) + 1 + extension_length);
    strcpy(output_file_name, stegobmp_config.out_bitmapfile);
    strcat(output_file_name, extension);

    FILE* output_file = fopen(output_file_name, "wb");
    if (output_file == NULL) {
        fprintf(stderr, "ERROR: No se puede extraer la información con los parámetros seleccionados.\n");
        return;
    }

    fwrite(data , 1, data_size, output_file);

    fclose(output_file);
    free(output_file_name);
}

uint8_t * extract_LSB4 (FILE* bearer_file, uint32_t length_embeded_bytes, uint64_t * index_bearer) {

    uint64_t size_of_bearer = size_of_file(bearer_file);
    char * bearer_to_read = malloc(size_of_bearer);
    size_t number_of_bytes_read = fread(bearer_to_read, sizeof(char), size_of_bearer, bearer_file);

    if (size_of_bearer != number_of_bytes_read) {
        fprintf(stderr, "ERROR: La lectura del archivo portador falló.\n");
        return NULL;
    }

    uint32_t index_embed = 0;
    uint8_t mask = 0xF;
    uint8_t * embeded_bytes = calloc(length_embeded_bytes + 1, sizeof(uint8_t));

    if (size_of_bearer < length_embeded_bytes) {
        fprintf(stderr, "ERROR: El tamaño del portador es menor al mensaje que se desea extraer.\n");
        return NULL;
    }

    while (index_embed < length_embeded_bytes) {

        /* para extraer 1 byte, necesitamos 2 bytes del bearer */

        uint8_t byte_to_extract_1 = bearer_to_read[*index_bearer];
        *index_bearer = *index_bearer + 1;
        uint8_t embeded_byte_1 = mask & byte_to_extract_1;
        embeded_byte_1 = embeded_byte_1 << 4;

        uint8_t byte_to_extract_2 = bearer_to_read[*index_bearer];
        *index_bearer = *index_bearer + 1;
        uint8_t embeded_byte_2 = mask & byte_to_extract_2;

        embeded_bytes[index_embed] = embeded_byte_1 | embeded_byte_2;
        index_embed++;
    }
    /* null al final del mensaje */
    embeded_bytes[index_embed] = 0;
    free(bearer_to_read);

    return embeded_bytes;
}

char * extract_extension_LSB4 (FILE* bearer_file, uint8_t * extension_length, uint64_t * index_bearer) {

    uint64_t size_of_bearer = size_of_file(bearer_file);
    char * bearer_to_read = malloc(size_of_bearer);
    fread(bearer_to_read, sizeof(char), size_of_bearer, bearer_file);

    uint8_t index_embed = 0;
    uint8_t mask = 0xF;
    char * embeded_bytes = calloc(15, sizeof(char));

    do {

        /* para extraer 1 byte, necesitamos 2 bytes del bearer */

        uint8_t byte_to_extract_1 = bearer_to_read[*index_bearer];
        *index_bearer = *index_bearer + 1;
        uint8_t embeded_byte_1 = mask & byte_to_extract_1;
        embeded_byte_1 = embeded_byte_1 << 4;

        uint8_t byte_to_extract_2 = bearer_to_read[*index_bearer];
        *index_bearer = *index_bearer + 1;
        uint8_t embeded_byte_2 = mask & byte_to_extract_2;

        embeded_bytes[index_embed] = embeded_byte_1 | embeded_byte_2;

    } while (embeded_bytes[index_embed++] != 0);

    *extension_length = index_embed == 0 ? index_embed : index_embed - 1;

    free(bearer_to_read);

    return embeded_bytes;
}

uint8_t * extract_LSBI(FILE* bearer_file, uint32_t * length_embeded_bytes, char** embeded_bytes_extension, uint8_t* embeded_bytes_extension_size) {
    //copio el bmp bearer. EMPROLIJAR
    uint64_t size_of_bearer = file_size(bearer_file);
    uint8_t * bearer1 = malloc(size_of_bearer);
    fread(bearer1, sizeof(uint8_t), size_of_bearer, bearer_file);
    uint8_t * bearer = malloc(size_of_bearer - BYTES_IN_HEADER);
    memcpy(bearer , bearer1 + BYTES_IN_HEADER, size_of_bearer - BYTES_IN_HEADER);
    free(bearer1);

    //levanto la clave
    uint8_t* key = malloc(BYTES_IN_KEY);
    memcpy(key, bearer, BYTES_IN_KEY);

    //levanto el hop
    uint32_t hop = get_msb_order(key[0]);

    //levanto el size del mensaje escondido y lo desencripto
    int cycle_pos = 1;
    uint64_t extracting_pos = BYTES_IN_KEY;
    uint8_t* message_size_enc = extract_LSBI_aux(bearer, size_of_bearer, hop, FLENGTH_WORD_SIZE, sizeof(uint8_t), &cycle_pos, &extracting_pos);
    uint8_t* message_size_dec = decrypt_rc4(message_size_enc, FLENGTH_WORD_SIZE, key, BYTES_IN_KEY, false);
    *length_embeded_bytes = to_big_endian(message_size_dec);

    //levanto el mensaje
    uint8_t* message_enc = extract_LSBI_aux(bearer, size_of_bearer, hop, *length_embeded_bytes, sizeof(uint8_t), &cycle_pos, &extracting_pos);
    uint8_t* message_dec = decrypt_rc4(message_enc, *length_embeded_bytes, key, BYTES_IN_KEY, true);
    free(message_enc);

    //extraigo la extension
    *embeded_bytes_extension = calloc(15, sizeof(char));
    bool end = false;
    int length_extension = 0;
    while(!end) {
        uint8_t* extension_enc = extract_LSBI_aux(bearer, size_of_bearer, hop, 1, sizeof(uint8_t), &cycle_pos, &extracting_pos);
        char* extension_dec = (char*) decrypt_rc4(extension_enc, 1, key, BYTES_IN_KEY, true);
        if(extension_dec[0] == 0){
            end = true;
        }
        (*embeded_bytes_extension)[length_extension] = extension_dec[0];
        length_extension++;
        free(extension_enc);
        free(extension_dec);
    }
    *embeded_bytes_extension_size = length_extension;

    free(bearer);
    free(key);
    free(message_size_enc);
    return message_dec;
}

uint8_t *extract_LSBI_aux(uint8_t * bearer, uint64_t size_of_bearer,  uint32_t hop, uint64_t n_elems, size_t size_of_elem, int* cycle_pos, uint64_t* extracting_pos) {
    uint8_t* message_enc = calloc(n_elems, size_of_elem);
    uint64_t bytes_extracted = 0;

    while (bytes_extracted < n_elems) {
        for(int i = 0; i < 8; i++) {
            uint8_t new_bit = (uint8_t) ((bearer[*extracting_pos]  & 0x1) << (7 - i));
            message_enc[bytes_extracted] = message_enc[bytes_extracted] | new_bit;
            *extracting_pos = *extracting_pos + hop;
            if(*extracting_pos >= size_of_bearer - BYTES_IN_HEADER) {
                *extracting_pos = BYTES_IN_KEY + *cycle_pos;
                (*cycle_pos)++;
            }
        }
        bytes_extracted++;
    }

    return message_enc;
}

uint32_t get_msb_order( uint8_t byte) {
    int ndx = 0;
    uint32_t hop;
    if(byte == 0) {
        hop = 256;
    } else {
        while (1 < byte) {
            byte = (byte >> 1);
            ndx++;
        }
        hop = pow(2,ndx);
    }
    return hop;
}

long file_size (FILE * fp) {
    fseek(fp, 0L, SEEK_END);
    long int res = ftell(fp);
    rewind(fp);
    return res;
}

