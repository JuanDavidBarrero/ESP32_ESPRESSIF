#include <stdio.h>
#include "esp_spiffs.h"
#include "esp_log.h"
#include <string.h>
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/pk.h"
#include "mbedtls/entropy.h"

int ret = 0;
mbedtls_pk_context pk;

mbedtls_ctr_drbg_context ctr_drbg;
mbedtls_entropy_context entropy;
char *personalization = "JuanDavidBarreroString";

extern "C" void app_main(void)
{
    esp_vfs_spiffs_conf_t config = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true,
    };
    esp_vfs_spiffs_register(&config);

    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_entropy_init(&entropy);

    ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, (const unsigned char *)personalization, strlen(personalization));
    if (ret != 0)
    {
        printf(" failed\n  mbedtls_ctr_drbg_seed ");
        return;
    }

    mbedtls_ctr_drbg_set_prediction_resistance(&ctr_drbg, MBEDTLS_CTR_DRBG_PR_ON);

    mbedtls_pk_init(&pk);

    if ((ret = mbedtls_pk_parse_public_keyfile(&pk, "/spiffs/public.pub")) != 0)
    {
        printf(" failed\n  ! mbedtls_pk_parse_public_keyfile returned -0x%04x\n", -ret);
        return;
    }

    ESP_LOGI("SUCCESS", "Generating the encrypted value");

    unsigned char to_encrypt[] = "Hades lobo blanco";
    size_t to_encrypt_len = sizeof(to_encrypt);
    unsigned char encrypted[MBEDTLS_MPI_MAX_SIZE];
    unsigned char decrypted[MBEDTLS_MPI_MAX_SIZE];
    memset(encrypted, 0, sizeof(encrypted));
    memset(decrypted, 0, sizeof(decrypted));
    size_t olen = 0;

    if ((ret = mbedtls_pk_encrypt(&pk, to_encrypt, to_encrypt_len, encrypted, &olen, sizeof(encrypted), mbedtls_ctr_drbg_random, &ctr_drbg)) != 0)
    {
        printf(" failed\n  ! mbedtls_pk_encrypt returned -0x%04x\n", -ret);
    }

    printf("Ecrypted Text: %s \n", encrypted);

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    ESP_LOGI("Decrypt", "Decryption");

    mbedtls_pk_init(&pk);

    if ((ret = mbedtls_pk_parse_keyfile(&pk, "/spiffs/private.pem", "")) != 0)
    {
        printf("Private key load failed\nmbedtls_pk_parse_keyfile returned -0x%04x\n", -ret);
    }

    if ((ret = mbedtls_pk_decrypt(&pk, encrypted, olen, decrypted, &olen, sizeof(decrypted), mbedtls_ctr_drbg_random, &ctr_drbg)) != 0)
    {
        printf("Decrypted failed\nmbedtls_pk_decrypt returned -0x%04x\n", -ret);
    }

    printf("Decrypted Text: %s \n", decrypted);
}
