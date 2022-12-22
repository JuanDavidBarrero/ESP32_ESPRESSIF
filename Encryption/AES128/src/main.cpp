#include "stdio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <string>
#include "Cipher.h"

#define TAG "ENCRYPT"

extern "C" void app_main()
{
    Cipher *cipher = new Cipher();

    char *key = "HadesLLavePrueba";
    cipher->setKey(key);

    std::string data = "Hades el mistio lobo !";

    ESP_LOGW(TAG,"Encrypt data");
    std::string cipherString = cipher->encryptstring(data);
    printf("%s \n",cipherString.c_str());

    vTaskDelay(1000/portTICK_PERIOD_MS);

    ESP_LOGE(TAG,"Decrypt data");
    std::string decipheredString = cipher->decryptstring(cipherString);
    printf("%s \n",decipheredString.c_str());
}