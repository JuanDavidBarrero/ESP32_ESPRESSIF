#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include <string>
#include "esp_log.h"
#include "esp_http_client.h"
#include "esp_http_server.h"

static httpd_handle_t http_server_handle = NULL;

extern const uint8_t index_html_start[]   asm("_binary_index_html_start");
extern const uint8_t index_html_end[]     asm("_binary_index_html_end");
extern const uint8_t app_css_start[]      asm("_binary_style_css_start");
extern const uint8_t app_css_end[]        asm("_binary_style_css_end");
extern const uint8_t app_js_start[]       asm("_binary_script_js_start");
extern const uint8_t app_js_end[]         asm("_binary_script_js_end");

#define SSID ""
#define PASS ""

static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id)
    {
    case WIFI_EVENT_STA_START:
        printf("WiFi connecting ... \n");
        break;
    case WIFI_EVENT_STA_CONNECTED:
        printf("WiFi connected ... \n");
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        printf("WiFi lost connection ... \n");
        break;
    case IP_EVENT_STA_GOT_IP:
        printf("WiFi got IP ... \n\n");
        break;
    default:
        break;
    }
}

void wifi_connection()
{

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
    esp_wifi_set_config(WIFI_IF_STA, &wifi_configuration);

    esp_wifi_start();

    esp_wifi_connect();
}

static esp_err_t http_server_index_html_handler(httpd_req_t *req)
{
    ESP_LOGI("Server", "index.html requested");

    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, (const char *)index_html_start, index_html_end - index_html_start);

    return ESP_OK;
}

static esp_err_t http_server_app_css_handler(httpd_req_t *req)
{
    ESP_LOGI("Server", "app.css requested");

    httpd_resp_set_type(req, "text/css");
    httpd_resp_send(req, (const char *)app_css_start, app_css_end - app_css_start);

    return ESP_OK;
}

static esp_err_t http_server_app_js_handler(httpd_req_t *req)
{
    ESP_LOGI("Server", "app.js requested");

    httpd_resp_set_type(req, "application/javascript");
    httpd_resp_send(req, (const char *)app_js_start, app_js_end - app_js_start);

    return ESP_OK;
}

httpd_handle_t startServer()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    if (httpd_start(&http_server_handle, &config) == ESP_OK)
    {
        httpd_uri_t index_html = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = http_server_index_html_handler,
            .user_ctx = NULL};
        httpd_register_uri_handler(http_server_handle, &index_html);

        httpd_uri_t app_css = {
            .uri = "/style.css",
            .method = HTTP_GET,
            .handler = http_server_app_css_handler,
            .user_ctx = NULL};
        httpd_register_uri_handler(http_server_handle, &app_css);

        httpd_uri_t app_js = {
            .uri = "/script.js",
            .method = HTTP_GET,
            .handler = http_server_app_js_handler,
            .user_ctx = NULL};
        httpd_register_uri_handler(http_server_handle, &app_js);
    }
    return http_server_handle;
}

extern "C" void app_main(void)
{
    nvs_flash_init();
    wifi_connection();

    vTaskDelay(2000 / portTICK_PERIOD_MS);
    printf("WIFI was initiated ...........\n\n");

    startServer();
}