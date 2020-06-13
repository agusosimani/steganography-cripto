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

unsigned char * encrypt(uint8_t * plain_text, unsigned long length_plain_text, int * length_cipher) {
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
