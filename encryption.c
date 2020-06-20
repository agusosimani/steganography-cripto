#include "include/encryption.h"

const EVP_CIPHER * get_evp_cipher(void) {
    if (strcmp(stegobmp_config.enc_algorithm, "aes128") == 0) {

        if (strcmp(stegobmp_config.enc_mode, "ecb") == 0) {
            return EVP_aes_128_ecb();
        } else if (strcmp(stegobmp_config.enc_mode, "cfb") == 0) {
            return EVP_aes_128_cfb8();
        } else if (strcmp(stegobmp_config.enc_mode, "ofb") == 0) {
            return EVP_aes_128_ofb();
        } else if (strcmp(stegobmp_config.enc_mode, "cbc") == 0) {
            return EVP_aes_128_cbc();
        }
    } else if (strcmp(stegobmp_config.enc_algorithm, "aes192") == 0) {

        if (strcmp(stegobmp_config.enc_mode, "ecb") == 0) {
            return EVP_aes_192_ecb();
        } else if (strcmp(stegobmp_config.enc_mode, "cfb") == 0) {
            return EVP_aes_192_cfb8();
        } else if (strcmp(stegobmp_config.enc_mode, "ofb") == 0) {
            return EVP_aes_192_ofb();
        } else if (strcmp(stegobmp_config.enc_mode, "cbc") == 0) {
            return EVP_aes_192_cbc();
        }
    } else if (strcmp(stegobmp_config.enc_algorithm, "aes256") == 0) {

        if (strcmp(stegobmp_config.enc_mode, "ecb") == 0) {
            return EVP_aes_256_ecb();
        } else if (strcmp(stegobmp_config.enc_mode, "cfb") == 0) {
            return EVP_aes_256_cfb8();
        } else if (strcmp(stegobmp_config.enc_mode, "ofb") == 0) {
            return EVP_aes_256_ofb();
        } else if (strcmp(stegobmp_config.enc_mode, "cbc") == 0) {
            return EVP_aes_256_cbc();
        }

    } else if (strcmp(stegobmp_config.enc_algorithm, "des") == 0) {

        if (strcmp(stegobmp_config.enc_mode, "ecb") == 0) {
            return EVP_des_ecb();
        } else if (strcmp(stegobmp_config.enc_mode, "cfb") == 0) {
            return EVP_des_cfb8();
        } else if (strcmp(stegobmp_config.enc_mode, "ofb") == 0) {
            return EVP_des_ofb();
        } else if (strcmp(stegobmp_config.enc_mode, "cbc") == 0) {
            return EVP_des_cbc();
        }
    }
    return NULL;
}

void derive_from_password(const EVP_CIPHER * evp_cipher, unsigned char * key, unsigned char * IV) {
    // To derive key and IV from password, assume that hash function is sha256 and that SALT is not used
    const EVP_MD * dgst = EVP_sha256();
    const unsigned char * salt = NULL;

    EVP_BytesToKey(evp_cipher, dgst, salt, (unsigned char *)stegobmp_config.pass, (int)strlen(stegobmp_config.pass),1, key, IV);
}

unsigned char * encrypt_(uint8_t * plain_text, unsigned long length_plain_text, int * length_cipher) {
    // Get structs needed for symmetric cipher with evp.h (Initialize context)
    const EVP_CIPHER * evp_cipher = get_evp_cipher();
    EVP_CIPHER_CTX * evp_cipher_ctx = EVP_CIPHER_CTX_new();

    // Derive key and IV from password
    unsigned char key[EVP_MAX_KEY_LENGTH];
    unsigned char IV[EVP_MAX_IV_LENGTH];
    derive_from_password(evp_cipher, key, IV);

    // Set encryption parameters in the context
    EVP_EncryptInit_ex(evp_cipher_ctx, evp_cipher, NULL, key, IV);

    // Alloc memory for cipher
    unsigned char * cipher = malloc((length_plain_text / EVP_CIPHER_block_size(evp_cipher) + (length_plain_text%EVP_CIPHER_block_size(evp_cipher)!=0)) *  EVP_CIPHER_block_size(evp_cipher));

    // Encrypt
    EVP_EncryptUpdate(evp_cipher_ctx, cipher, length_cipher, plain_text, (int)length_plain_text);

    // Encrypt final part, remaining block + padding
    int templ;
    EVP_EncryptFinal(evp_cipher_ctx, cipher + *length_cipher, &templ);

    // Update length_cipher
    *length_cipher += templ;
    cipher = realloc(cipher, (size_t)*length_cipher);

    // Free context struct
    EVP_CIPHER_CTX_free(evp_cipher_ctx);

    return cipher;
}

unsigned char * gdecrypt(uint8_t * cipher, unsigned long length_cipher, int * length_plain_text) {
    // Get structs needed for symmetric cipher with evp.h (Initialize context)
    const EVP_CIPHER * evp_cipher = get_evp_cipher();
    EVP_CIPHER_CTX * evp_cipher_ctx = EVP_CIPHER_CTX_new();

    // Derive key and IV from password
    unsigned char key[EVP_MAX_KEY_LENGTH];
    unsigned char IV[EVP_MAX_IV_LENGTH];
    derive_from_password(evp_cipher, key, IV);

    // Set decryption parameters in the context
    EVP_DecryptInit_ex(evp_cipher_ctx, evp_cipher, NULL, key, IV);

    // Alloc memory for plain_text
    unsigned char * plain_text = malloc((length_cipher / EVP_CIPHER_block_size(evp_cipher) + (length_cipher%EVP_CIPHER_block_size(evp_cipher)!=0)) *  EVP_CIPHER_block_size(evp_cipher));

    // Decrypt
    EVP_DecryptUpdate(evp_cipher_ctx, plain_text, length_plain_text, cipher, (int)length_cipher);

    // Decrypt final part, remaining block + padding
    int templ;
    EVP_DecryptFinal_ex(evp_cipher_ctx, plain_text + *length_plain_text, &templ);

    // Update length_plain_text
    *length_plain_text += templ;
    plain_text = realloc(plain_text, (size_t)*length_plain_text);

    // Free context struct
    EVP_CIPHER_CTX_free(evp_cipher_ctx);

    return plain_text;
}

uint8_t * encrypt_rc4(const uint8_t * plain_text, unsigned long length_plain_text, const uint8_t * key, unsigned long length_key) {
    unsigned char T[N];
    unsigned char S[N];
    unsigned char aux_swap;
    int i=0, j=0, t=0;

    uint8_t * cipher = malloc(length_plain_text);

    for (i = 0; i < N; i++) {
        S[i] = i;
        T[i] = key[i % length_key];
    }

    for (i = 0 ; i < N; i++) {
        j = (j + S[i] + T[i] ) % N;

        aux_swap = S[j];
        S[j] = S[i];
        S[i] = aux_swap;
    }

    i=0, j=0;
    for(size_t n=0; n < length_plain_text ; n++) {
        i = (i+1) % N;
        j = (j+S[i]) % N;

        aux_swap = S[j];
        S[j] = S[i];
        S[i] = aux_swap;

        t = (S[i]+S[j]) % N;

        cipher[n] = plain_text[n] ^ S[t];
    }
    return cipher;
}

uint8_t * decrypt_rc4(const uint8_t * cipher, unsigned long length_cipher, const uint8_t * key, unsigned long length_key) {
    unsigned char T[N];
    unsigned char S[N];
    unsigned char aux_swap;
    int i=0, j=0, t=0;

    uint8_t * plain_text = malloc(length_cipher);

    for (i = 0; i < N; i++) {
        S[i] = i;
        T[i] = key[i % length_key];
    }

    for (i = 0 ; i < N; i++) {
        j = (j + S[i] + T[i] ) % N;

        aux_swap = S[j];
        S[j] = S[i];
        S[i] = aux_swap;
    }

    i=0, j=0;
    for(size_t n=0; n < length_cipher ; n++) {
        i = (i+1) % N;
        j = (j+S[i]) % N;

        aux_swap = S[j];
        S[j] = S[i];
        S[i] = aux_swap;

        t = (S[i]+S[j]) % N;

        plain_text[n] = cipher[n] ^ S[t];
    }
    return plain_text;
}
