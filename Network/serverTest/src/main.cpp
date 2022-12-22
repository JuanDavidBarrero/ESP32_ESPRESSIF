#include <stdio.h>
#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "esp_http_server.h"

static httpd_handle_t http_server_handle = NULL;

uint16_t counter = 0;

extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_index_html_end");
extern const uint8_t app_css_start[] asm("_binary_style_css_start");
extern const uint8_t app_css_end[] asm("_binary_style_css_end");
extern const uint8_t app_js_start[] asm("_binary_script_js_start");
extern const uint8_t app_js_end[] asm("_binary_script_js_end");

#define SSID CONFIG_MY_SSID
#define PASS CONFIG_MY_PASSWORD
#define MAXIMUM_RETRY CONFIG_MAXIMUM_RETRY

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

static EventGroupHandle_t s_wifi_event_group;
static int s_retry_num = 0;

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

static esp_err_t http_postInfo(httpd_req_t *req)
{

    char receiveBuffer[100];
    size_t recv_size = std::min(req->content_len, sizeof(receiveBuffer));
    int ret = httpd_req_recv(req, receiveBuffer, recv_size);

    printf("\nPOST content: %s\n", receiveBuffer);

    if (ret <= 0)
    {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT)
        {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    const char resp[] = "{ \"status\":\"esp POST exitoso\" }";
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}

static esp_err_t http_getInfo(httpd_req_t *req)
{

    char sendBuffer[100];

    sprintf(sendBuffer, "{\"nombre\":\"%s\", \"temp\": %f,  \"version\": \"%s\", \"status\":%i, \"counter\":%i}", "JuanDavid", 26.54, "V1.0.1", 1, counter++);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_send(req, sendBuffer, HTTPD_RESP_USE_STRLEN);

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

        httpd_uri_t getEsp32 = {
            .uri = "/info",
            .method = HTTP_GET,
            .handler = http_getInfo,
            .user_ctx = NULL};
        httpd_register_uri_handler(http_server_handle, &getEsp32);

        httpd_uri_t postEsp32 = {
            .uri = "/user",
            .method = HTTP_POST,
            .handler = http_postInfo,
            .user_ctx = NULL};
        httpd_register_uri_handler(http_server_handle, &postEsp32);
    }
    return http_server_handle;
}

extern "C" void app_main(void)
{
    nvs_flash_init();
    wifi_connection();

    printf("Server was initiated ...........\n\n");

    startServer();
}