#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_timer.h"
#include "mbedtls/gcm.h"

extern "C" void app_main(void)
{
    unsigned char key[32] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32};
    unsigned char iv[16] = {0xff, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
    char input[] = "Hades el mistico lobo blanco Juan David Barrero L";
    int input_len = strlen((char *)input);
    unsigned char encrypt_output[input_len] = {0};
    unsigned char decrypt_output[input_len] = {0};
    uint8_t tag[16];
    uint8_t tag1[16];

    mbedtls_gcm_context aes;

    mbedtls_gcm_init(&aes);
    mbedtls_gcm_setkey(&aes, MBEDTLS_CIPHER_ID_AES, (const unsigned char *)key, sizeof(key) * 8);
    mbedtls_gcm_starts(&aes, MBEDTLS_GCM_ENCRYPT, (const unsigned char *)iv, sizeof(iv), NULL, 0);
    mbedtls_gcm_update(&aes, strlen(input), (const unsigned char *)input, encrypt_output);
    mbedtls_gcm_finish(&aes, tag, 16);
    mbedtls_gcm_free(&aes);

    ESP_LOG_BUFFER_HEX("CBC encrypt", encrypt_output, input_len);

    mbedtls_gcm_init(&aes);
    mbedtls_gcm_setkey(&aes, MBEDTLS_CIPHER_ID_AES, (const unsigned char *)key, sizeof(key) * 8);
    mbedtls_gcm_starts(&aes, MBEDTLS_GCM_DECRYPT, (const unsigned char *)iv, sizeof(iv), NULL, 0);
    mbedtls_gcm_update(&aes, input_len, (const unsigned char *)encrypt_output, decrypt_output);
    mbedtls_gcm_finish(&aes, tag1, 16);
    mbedtls_gcm_free(&aes);

    if (strcmp((const char *)tag, (const char *)tag1) == 0)
    {
        ESP_LOGW("TAG", "the tags are the same !");
    }

    for (int i = 0; i < input_len; i++)
    {
        printf("%c", (char)decrypt_output[i]);
    }
    printf("\n");

    ESP_LOGI("CBC plain text", "All successfull");
}