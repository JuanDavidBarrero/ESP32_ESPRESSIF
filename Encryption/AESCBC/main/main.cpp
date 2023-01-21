#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "mbedtls/aes.h"

#define BLOCK_SIZE 16

extern "C" void app_main(void)
{

    unsigned char key[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    unsigned char iv[16] = {0xff, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
    unsigned char iv1[] = {0xff, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

    unsigned char encrypt_output[BLOCK_SIZE * 4] = {0};
    unsigned char decrypt_output[BLOCK_SIZE * 4] = {0};
    unsigned char input[BLOCK_SIZE * 4] = "Hades el mistico lobo blanco y juan david barrero lizcano 12345";
    int input_len = strlen((char *)input);
    int n_blocks = (input_len + BLOCK_SIZE - 1) / BLOCK_SIZE;

    mbedtls_aes_context aes;

    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_enc(&aes, key, 128);

    for (int i = 0; i < n_blocks; i++)
    {
        mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_ENCRYPT, BLOCK_SIZE, iv, input + i * BLOCK_SIZE, encrypt_output + i * BLOCK_SIZE);
    }

    ESP_LOG_BUFFER_HEX("CBC encrypt", encrypt_output, BLOCK_SIZE * 4);

    for (int i = 0; i < n_blocks; i++)
    {
        mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_DECRYPT, BLOCK_SIZE, iv1, encrypt_output+ i*BLOCK_SIZE, decrypt_output+ i*BLOCK_SIZE);
    }

    ESP_LOG_BUFFER_HEX("CBC decrypt", decrypt_output, BLOCK_SIZE * 4);
    ESP_LOGI("CBC plain text", "%s", decrypt_output);

    mbedtls_aes_free(&aes);
}