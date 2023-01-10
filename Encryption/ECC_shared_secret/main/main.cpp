#include <stdio.h>
#include <time.h>
#include <string.h>
#include "uECC.h"
#include "esp_log.h"
#include "esp_system.h"

#define SECRET_SIZE 32
#define MESSAGE_SIZE 1024
#define TAG "uECC"

static int rng_func(uint8_t *dest, unsigned size)
{

    uint32_t rand = 0;
    unsigned pos = 0;
    uint8_t step = 0;
    uint8_t rand8 = 0;

    while (pos < size)
    {
        if (step >= 4)
        {
            step = 0;
        }
        if (step == 0)
        {
            rand = esp_random();
            // ESP_LOGI(TAG, "rand 0x%08X",rand);
        }
        // faster then 8*step ?
        switch (step)
        {
        case 0:
            rand8 = rand & 0xFF;
            break;
        case 1:
            rand8 = (rand >> 8) & 0xFF;
            break;
        case 2:
            rand8 = (rand >> 16) & 0xFF;
            break;
        case 3:
            rand8 = (rand >> 24) & 0xFF;
            break;
        }
        // ESP_LOGI(TAG, "%d) rand 8 0x%02X",pos,rand8);
        *dest++ = rand8;
        step++;
        pos++;
    }

    return 1; // random data was generated
}

extern "C" void app_main(void)
{
    uECC_Curve p_curve;            // eliptic curve
    uint8_t private_key1[32];      // local secret key
    uint8_t public_key1[64];       // local public key

    uint8_t private_key2[32];      // remote secret key
    uint8_t public_key2[64];       // remote public key

    uint8_t secret1[SECRET_SIZE];  // local shared secret
    uint8_t secret2[SECRET_SIZE];  // remote shared secret

    uint8_t message[MESSAGE_SIZE]; // public message
    unsigned char nonce[8];
    int ret;
    int privksize;
    int pubksize;

    p_curve = uECC_secp256r1();

    uECC_set_rng(rng_func);

    rng_func(message, MESSAGE_SIZE); // fill message with random
    rng_func(nonce, 8);              // fill nonce with random, the nonce have to be shared between local and remote
    ret = uECC_make_key(public_key1, private_key1, p_curve);
    ret = uECC_make_key(public_key2, private_key2, p_curve);

    privksize = uECC_curve_private_key_size(p_curve);
    pubksize = uECC_curve_public_key_size(p_curve);
    printf("\n");

    ESP_LOGW(TAG, "--- Private Key ---");

    printf("private_key = { ");
    for (int i = 0; i < privksize; i++)
    {
        printf("0x%02X,", private_key1[i]);
    }
    printf(" };\n");

    printf("\n");

    ESP_LOGW(TAG, "--- Public Key ---");

    printf("public_key = { ");
    for (int i = 0; i < pubksize; i++)
    {
        printf("0x%02X,", public_key2[i]);
    }
    printf(" };\n");

    ret = uECC_shared_secret(public_key2, private_key1, secret1, p_curve);
    ret = uECC_shared_secret(public_key1, private_key2, secret2, p_curve);

    ret = memcmp(secret1, secret2, SECRET_SIZE);
    printf("\n");

    if (ret == 0)
    {
        ESP_LOGI(TAG, "---Secret valid---");
    }
    else
    {
        ESP_LOGE(TAG, "---Secret void---");
    }
     printf("\n");

    ESP_LOGW(TAG, "---  Shared secret ---");
    printf("secret1 = {");
    for (int i = 0; i < SECRET_SIZE; i++)
    {
        printf("0x%02X,", secret1[i]);
    }
    printf("}\n");
}