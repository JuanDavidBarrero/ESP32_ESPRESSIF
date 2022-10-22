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

// 30:AE:A4:88:9F:BC
uint8_t broadcastAddress[] = {0x30, 0xAE, 0xA4, 0x88, 0x9F, 0xBC};

typedef struct
{
    char a[32];
    int b;
    float c;
    bool d;
} message_sent_t;

esp_now_peer_info_t peerInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    printf("\r\nLast Packet Send Status:\t");
    printf(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success\n" : "Delivery Fail\n");
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

    vTaskDelay(2000 / portTICK_PERIOD_MS);
    printf("WIFI was initiated ...........\n\n");

    if (esp_now_init() != ESP_OK)
    {
        printf("Error initializing ESP-NOW");
        return;
    }

    esp_now_register_send_cb(OnDataSent);

    message_sent_t myData;

    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.ifidx = WIFI_IF_STA;
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
        printf("Failed to add peer");
    }

    while (true)
    {
        strcpy(myData.a, "Hello Juan David");
        myData.b = 15;
        myData.c = 1.2;
        myData.d = false;

        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));

        if (result == ESP_OK)
        {
            printf("Sent with success\n");
        }
        else
        {
            printf("Error sending the data\n");
        }
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}