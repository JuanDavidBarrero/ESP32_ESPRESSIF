#include <stdio.h>
#include "string.h"
#include "esp_log.h"
#include "mbedtls/error.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/md.h"
#include "mbedtls/pk.h"

int ret = 0;
mbedtls_pk_context pk;

mbedtls_ctr_drbg_context ctr_drbg;
mbedtls_entropy_context entropy;
char *personalization = "JuanDavidBarreroString";

uint8_t shaResult[32];
char *payload = "Hades information to be send";

mbedtls_md_context_t ctx;
mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;

extern const uint8_t privateKey[] asm("_binary_private_pem_start");
extern const uint8_t privateKey_end[] asm("_binary_private_pem_end");
extern const uint8_t publicKey[] asm("_binary_public_pub_start");
extern const uint8_t publicKey_end[] asm("_binary_public_pub_end");

extern "C" void app_main(void)
{
    unsigned char signedDocument[MBEDTLS_PK_SIGNATURE_MAX_SIZE];
    size_t olen = 0;
    size_t privateKeyLen = privateKey_end - privateKey;
    size_t publicKeyLen = publicKey_end - publicKey;
    const size_t payloadLength = strlen(payload);

    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_entropy_init(&entropy);
    mbedtls_pk_init(&pk);

    mbedtls_md_init(&ctx);
    mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 0);
    mbedtls_md_starts(&ctx);
    mbedtls_md_update(&ctx, (const unsigned char *)payload, payloadLength);
    mbedtls_md_finish(&ctx, shaResult);
    mbedtls_md_free(&ctx);

    ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, (const unsigned char *)personalization, strlen(personalization));
    if (ret != 0)
    {
        printf(" failed\n  mbedtls_ctr_drbg_seed ");
        return;
    }

    if ((ret = mbedtls_pk_parse_key(&pk, (const unsigned char *)privateKey, privateKeyLen, (const unsigned char *)"", 0)) != 0)
    {
        printf(" failed\n  ! mbedtls_pk_parse_public_keyfile returned -0x%04x\n", -ret);
        return;
    }

    if ((ret = mbedtls_pk_sign(&pk, MBEDTLS_MD_SHA256, shaResult, 0, signedDocument, &olen, mbedtls_ctr_drbg_random, &ctr_drbg)) != 0)
    {
        printf(" failed\n  ! mbedtls_pk_sign returned -0x%04x\n", (unsigned int)-ret);
        return;
    }

    ESP_LOGW("Success", "--- document signed successfully ---\n");

    mbedtls_pk_init(&pk);

    if ((ret = mbedtls_pk_parse_public_key(&pk, (const unsigned char *)publicKey, publicKeyLen)) != 0)
    {
        printf(" failed\n  ! mbedtls_pk_parse_public_keyfile returned -0x%04x\n", (unsigned int)-ret);
    }

    /* In this part is necessary calculate the hash of the message sent*/

    if ((ret = mbedtls_pk_verify(&pk, MBEDTLS_MD_SHA256, shaResult, 0, signedDocument, olen)) != 0)
    {
        printf(" failed\n  ! mbedtls_pk_verify returned -0x%04x\n", (unsigned int)-ret);
    }
    else
    {

        ESP_LOGI("Success", ".OK (the signature is valid) !\n\n");
    }

    ESP_LOGE("END", "Code ended");
}