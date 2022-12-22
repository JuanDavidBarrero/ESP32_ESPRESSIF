#include <stdio.h>
#include "string.h"
#include "esp_log.h"
#include "mbedtls/md.h"

char *key = "secretKey";
char *payload = "Hades lobo blanco";
uint8_t hmacResult[32];

mbedtls_md_context_t ctx;
mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;

extern "C" void app_main(void)
{
    const size_t payloadLength = strlen(payload);
    const size_t keyLength = strlen(key);

    mbedtls_md_init(&ctx);
    mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 1);
    mbedtls_md_hmac_starts(&ctx, (const unsigned char *)key, keyLength);
    mbedtls_md_hmac_update(&ctx, (const unsigned char *)payload, payloadLength);
    mbedtls_md_hmac_finish(&ctx, hmacResult);
    mbedtls_md_free(&ctx);

    printf("Hash: ");
    for (int i = 0; i < sizeof(hmacResult); i++)
    {
        char str[3];
        sprintf(str, "%02x", (int)hmacResult[i]);
        printf(str);
    }

    ESP_LOGI("\nExito","\nMensaje completado");
}