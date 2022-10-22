#include <stdio.h>
#include <string>
#include <cstring>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include <esp_now.h>

#define SSID "DANIELA"
#define PASS "25285602"

#define TAG "MAIN"

// 24:62:AB:E0:9D:80
uint8_t broadcastAddress[] = {0x24, 0x62, 0xAB, 0xE0, 0x9D, 0x80};

typedef struct
{
    char a[32];
    int b;
    float c;
    bool d;
} message_sent_t;

message_sent_t myData;
esp_now_peer_info_t peerInfo;

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
    char charVal[10];
    memcpy(&myData, incomingData, sizeof(myData));
    // printf("Bytes received: ");
    printf("\n Char: ");
    printf((char *)myData.a);
    // printf("\n Int: ");
    // printf((char *)myData.b);
    // printf("\n Float: ");
    // sprintf(charVal, "%f", myData.c);
    // printf(charVal);
    // printf("\n Bool: ");
    // printf((char *)myData.d);
    ESP_LOGI("\nRECEIVER","Mensaje recivido completo");
}


void wifi_connection()
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_start());
}

extern "C" void app_main(void)
{
    nvs_flash_init();
    wifi_connection();

    printf("WIFI was initiated ...........\n\n");

    if (esp_now_init() != ESP_OK)
    {
        printf("Error initializing ESP-NOW");
        return;
    }

    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
        printf("Failed to add peer");
    }

    esp_now_register_recv_cb(OnDataRecv);
}