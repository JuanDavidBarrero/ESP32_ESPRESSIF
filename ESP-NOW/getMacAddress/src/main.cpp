#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"


extern "C" void app_main()
{
    unsigned char mac_base[6] = {0};
    esp_efuse_mac_get_default(mac_base);
    esp_read_mac(mac_base, ESP_MAC_WIFI_STA);
    unsigned char mac_local_base[6] = {0};
    unsigned char mac_uni_base[6] = {0};
    esp_derive_local_mac(mac_local_base, mac_uni_base);

    ESP_LOGW("LOCAL","%02X:%02X:%02X:%02X:%02X:%02X", mac_local_base[0], mac_local_base[1], mac_local_base[2], mac_local_base[3], mac_local_base[4], mac_local_base[5]);
    
    ESP_LOGE("UNI","%02X:%02X:%02X:%02X:%02X:%02X", mac_uni_base[0], mac_uni_base[1], mac_uni_base[2], mac_uni_base[3], mac_uni_base[4], mac_uni_base[5]);

    ESP_LOGI("MAC","%02X:%02X:%02X:%02X:%02X:%02X", mac_base[0], mac_base[1], mac_base[2], mac_base[3], mac_base[4], mac_base[5]);

    
}   