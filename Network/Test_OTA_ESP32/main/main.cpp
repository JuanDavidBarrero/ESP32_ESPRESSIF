#include <stdio.h>
#include <string>
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include "sys/param.h"
#include "cJSON.h"
#include "driver/gpio.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"

#define FIRMWARE_VERSION 0.1;  //this should be store on eeprom 
#define UPDATE_JSON_URL "http://192.168.0.106:3000/update"
#define BLINK_GPIO GPIO_NUM_2

#define SSID CONFIG_MY_SSID
#define PASS CONFIG_MY_PASSWORD
#define MAXIMUM_RETRY CONFIG_MAXIMUM_RETRY
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1
static EventGroupHandle_t s_wifi_event_group;
static int s_retry_num = 0;

char rcv_buffer[200];

static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
  {
    esp_wifi_connect();
  }
  else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
  {
    if (s_retry_num < MAXIMUM_RETRY)
    {
      esp_wifi_connect();
      s_retry_num++;
      ESP_LOGI("WIFI CONNECTION", "retry to connect to the AP");
    }
    else
    {
      xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
    }
    ESP_LOGI("WIFI CONNECTION", "connect to the AP fail");
  }
  else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
  {
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    ESP_LOGI("WIFI CONNECTION", "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
    s_retry_num = 0;
    xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
  }
}

void wifi_connection()
{

  s_wifi_event_group = xEventGroupCreate();

  esp_netif_init();
  esp_event_loop_create_default();
  esp_netif_create_default_wifi_sta();
  wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&wifi_initiation);

  esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
  esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);

  wifi_config_t wifi_configuration = {};
  memcpy(wifi_configuration.sta.ssid, SSID, std::min(strlen(SSID), sizeof(wifi_configuration.sta.ssid)));
  memcpy(wifi_configuration.sta.password, PASS, std::min(strlen(PASS), sizeof(wifi_configuration.sta.password)));

  esp_wifi_set_mode(WIFI_MODE_STA);
  esp_wifi_set_config(WIFI_IF_STA, &wifi_configuration);
  esp_wifi_start();

  EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                         WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                         pdFALSE,
                                         pdFALSE,
                                         portMAX_DELAY);
  if (bits & WIFI_CONNECTED_BIT)
  {
    ESP_LOGI("WIFI STATUS", "connected to ap SSID:%s", SSID);
  }
  else if (bits & WIFI_FAIL_BIT)
  {
    ESP_LOGI("WIFI STATUS", "Failed to connect to SSID:%s", SSID);
  }
  else
  {
    ESP_LOGE("WIFI STATUS", "UNEXPECTED EVENT");
  }
  vEventGroupDelete(s_wifi_event_group);
}

void blink_task(void *pvParameter)
{

  gpio_pad_select_gpio(BLINK_GPIO);
  gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
  while (1)
  {
    gpio_set_level(BLINK_GPIO, 0);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    gpio_set_level(BLINK_GPIO, 1);
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{

  switch (evt->event_id)
  {
  case HTTP_EVENT_ERROR:
    break;
  case HTTP_EVENT_ON_CONNECTED:
    break;
  case HTTP_EVENT_HEADER_SENT:
    break;
  case HTTP_EVENT_ON_HEADER:
    break;
  case HTTP_EVENT_ON_DATA:
    if (!esp_http_client_is_chunked_response(evt->client))
    {
      printf("HTTP_EVENT_ON_DATA: %.*s\n", evt->data_len, (char *)evt->data);
      strncpy(rcv_buffer, (char *)evt->data, evt->data_len);
    }
    break;
  case HTTP_EVENT_ON_FINISH:
    break;
  case HTTP_EVENT_DISCONNECTED:
    break;
  }
  return ESP_OK;
}

void check_update_task(void *pvParameter)
{

  while (1)
  {

    ESP_LOGW("Iniciando","Verificando actualizacion");

    esp_http_client_config_t config = {
        .url = UPDATE_JSON_URL,
        .method = HTTP_METHOD_GET,
        .event_handler = _http_event_handler,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK)
    {

      cJSON *json = cJSON_Parse(rcv_buffer);
      if (json == NULL)
        printf("downloaded file is not a valid json, aborting...\n");
      else
      {
        cJSON *version = cJSON_GetObjectItemCaseSensitive(json, "version");
        cJSON *file = cJSON_GetObjectItemCaseSensitive(json, "file");

        if (!cJSON_IsNumber(version))
          printf("unable to read new version, aborting...\n");
        else
        {

          double new_version = version->valuedouble;
          if (new_version > FIRMWARE_VERSION)
          {

            printf("current firmware version (%.1f) is lower than the available one (%.1f), upgrading...\n", FIRMWARE_VERSION, new_version);
            if (cJSON_IsString(file) && (file->valuestring != NULL))
            {
              printf("downloading and installing new firmware (%s)...\n", file->valuestring);

              esp_http_client_config_t ota_client_config = {
                  .url = file->valuestring,
                  // .cert_pem = server_cert_pem_start, //this one if for TLS1.2
              };
              esp_err_t ret = esp_https_ota(&ota_client_config);
              if (ret == ESP_OK)
              {
                printf("OTA OK, restarting...\n");
                esp_restart();
              }
              else
              {
                printf("OTA failed...\n");
              }
            }
            else
              printf("unable to read the new file name, aborting...\n");
          }
          else
            printf("current firmware version (%.1f) is greater or equal to the available one (%.1f), nothing to do...\n", FIRMWARE_VERSION, new_version);
        }
      }
    }
    else
      printf("unable to download the json file, aborting...\n");

    // cleanup
    esp_http_client_cleanup(client);

    printf("\n");
    vTaskDelay(10000 / portTICK_PERIOD_MS);
  }
}

extern "C" void app_main(void)
{
  printf("HTTPS OTA, firmware %.1f\n\n", FIRMWARE_VERSION);

  xTaskCreate(&blink_task, "blink_task", 1024, NULL, 5, NULL);

  nvs_flash_init();
  wifi_connection();

  xTaskCreate(&check_update_task, "check_update_task", 8192, NULL, 5, NULL);

}