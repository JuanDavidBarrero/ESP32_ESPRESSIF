#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "mbedtls/md.h"

uint8_t shaResult[32];
char *payload = "Hades lobo blanco";

mbedtls_md_context_t ctx;
mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;

extern "C" void app_main(void)
{
    const size_t payloadLength = strlen(payload);

    mbedtls_md_init(&ctx);
    mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 0);
    mbedtls_md_starts(&ctx);
    mbedtls_md_update(&ctx, (const unsigned char *)payload, payloadLength);
    mbedtls_md_finish(&ctx, shaResult);
    mbedtls_md_free(&ctx);

    printf("Hash: ");
    for (int i = 0; i < sizeof(shaResult); i++)
    {
        char str[3];
        sprintf(str, "%02x", (int)shaResult[i]);
        printf(str);
    }
    ESP_LOGW("Exito","\nProceso resulto bien");
}
