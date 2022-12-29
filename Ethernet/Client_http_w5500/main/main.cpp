#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_netif.h"
#include "esp_eth.h"
#include "esp_event.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_http_client.h"
#include "driver/spi_master.h"

static const char *TAG = "eth_example";


#if CONFIG_EXAMPLE_USE_SPI_ETHERNET
#define INIT_SPI_ETH_MODULE_CONFIG(eth_module_config, num)                                  \
    do                                                                                      \
    {                                                                                       \
        eth_module_config[num].spi_cs_gpio = CONFIG_EXAMPLE_ETH_SPI_CS##num##_GPIO;         \
        eth_module_config[num].int_gpio = CONFIG_EXAMPLE_ETH_SPI_INT##num##_GPIO;           \
        eth_module_config[num].phy_reset_gpio = CONFIG_EXAMPLE_ETH_SPI_PHY_RST##num##_GPIO; \
        eth_module_config[num].phy_addr = CONFIG_EXAMPLE_ETH_SPI_PHY_ADDR##num;             \
    } while (0)

typedef struct
{
    uint8_t spi_cs_gpio;
    uint8_t int_gpio;
    int8_t phy_reset_gpio;
    uint8_t phy_addr;
} spi_eth_module_config_t;
#endif

static void eth_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    uint8_t mac_addr[6] = {0};
    esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;

    switch (event_id)
    {
    case ETHERNET_EVENT_CONNECTED:
        esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr);
        ESP_LOGI(TAG, "Ethernet Link Up");
        ESP_LOGI(TAG, "Ethernet HW Addr %02x:%02x:%02x:%02x:%02x:%02x",
                 mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
        break;
    case ETHERNET_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "Ethernet Link Down");
        break;
    case ETHERNET_EVENT_START:
        ESP_LOGI(TAG, "Ethernet Started");
        break;
    case ETHERNET_EVENT_STOP:
        ESP_LOGI(TAG, "Ethernet Stopped");
        break;
    default:
        break;
    }
}

static void got_ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    const esp_netif_ip_info_t *ip_info = &event->ip_info;

    ESP_LOGW(TAG, "Ethernet Got IP Address");
    ESP_LOGW(TAG, "~~~~~~~~~~~");
    ESP_LOGI(TAG, "ETHIP:" IPSTR, IP2STR(&ip_info->ip));
    ESP_LOGI(TAG, "ETHMASK:" IPSTR, IP2STR(&ip_info->netmask));
    ESP_LOGI(TAG, "ETHGW:" IPSTR, IP2STR(&ip_info->gw));
    ESP_LOGW(TAG, "~~~~~~~~~~~");
}

esp_err_t client_event_get_handler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:
        printf("HTTP_EVENT_ON_DATA: %.*s\n", evt->data_len, (char *)evt->data);
        break;

    default:
        break;
    }
    return ESP_OK;
}

esp_err_t client_event_post_handler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:
        printf("HTTP_EVENT_ON_DATA: %.*s\n", evt->data_len, (char *)evt->data);
        break;

    default:
        break;
    }
    return ESP_OK;
}

static void rest_get()
{
    esp_http_client_config_t config_get = {
        .url = "https://reqres.in/api/users/2",
        .method = HTTP_METHOD_GET,
        .event_handler = client_event_get_handler};

    esp_http_client_handle_t client = esp_http_client_init(&config_get);
    esp_http_client_perform(client);
    esp_http_client_cleanup(client);
}

static void post_rest_function()
{
    esp_http_client_config_t config_post = {
        .url = "https://reqres.in/api/users",
        .method = HTTP_METHOD_POST,
        .event_handler = &client_event_post_handler};

    esp_http_client_handle_t client = esp_http_client_init(&config_post);

    char post_data[40];

    sprintf(post_data,"{ \"name\":\"%s\",\"job\":\"%s\" }", "Juan","Engineer");

    esp_http_client_set_post_field(client, post_data, strlen(post_data));
    esp_http_client_set_header(client, "Content-Type", "application/json");

    esp_http_client_perform(client);
    esp_http_client_cleanup(client);
}

extern "C" void app_main(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_inherent_config_t esp_netif_config = ESP_NETIF_INHERENT_DEFAULT_ETH();

    esp_netif_config_t cfg_spi = {};

    cfg_spi.base = &esp_netif_config;
    cfg_spi.stack = ESP_NETIF_NETSTACK_DEFAULT_ETH;

    esp_netif_t *eth_netif_spi[CONFIG_EXAMPLE_SPI_ETHERNETS_NUM] = {NULL};

    char if_key_str[10];
    char if_desc_str[10];
    char num_str[3];
    for (int i = 0; i < CONFIG_EXAMPLE_SPI_ETHERNETS_NUM; i++)
    {
        itoa(i, num_str, 10);
        strcat(strcpy(if_key_str, "ETH_SPI_"), num_str);
        strcat(strcpy(if_desc_str, "eth"), num_str);
        esp_netif_config.if_key = if_key_str;
        esp_netif_config.if_desc = if_desc_str;
        esp_netif_config.route_prio = 30 - i;
        eth_netif_spi[i] = esp_netif_new(&cfg_spi);
    }

    eth_mac_config_t mac_config_spi = ETH_MAC_DEFAULT_CONFIG();
    eth_phy_config_t phy_config_spi = ETH_PHY_DEFAULT_CONFIG();

    gpio_install_isr_service(0);

    spi_device_handle_t spi_handle[CONFIG_EXAMPLE_SPI_ETHERNETS_NUM] = {NULL};

    spi_bus_config_t buscfg{};

    buscfg.miso_io_num = CONFIG_EXAMPLE_ETH_SPI_MISO_GPIO,
    buscfg.mosi_io_num = CONFIG_EXAMPLE_ETH_SPI_MOSI_GPIO,
    buscfg.sclk_io_num = CONFIG_EXAMPLE_ETH_SPI_SCLK_GPIO,
    buscfg.quadwp_io_num = -1,
    buscfg.quadhd_io_num = -1,

    spi_bus_initialize(HSPI_HOST, &buscfg, SPI_DMA_CH_AUTO);

    spi_eth_module_config_t spi_eth_module_config[CONFIG_EXAMPLE_SPI_ETHERNETS_NUM];
    INIT_SPI_ETH_MODULE_CONFIG(spi_eth_module_config, 0);

    esp_eth_mac_t *mac_spi[CONFIG_EXAMPLE_SPI_ETHERNETS_NUM];
    esp_eth_phy_t *phy_spi[CONFIG_EXAMPLE_SPI_ETHERNETS_NUM];
    esp_eth_handle_t eth_handle_spi[CONFIG_EXAMPLE_SPI_ETHERNETS_NUM] = {NULL};

    spi_device_interface_config_t devcfg{};

    devcfg.command_bits = 16;
    devcfg.address_bits = 8;
    devcfg.mode = 0;
    devcfg.clock_speed_hz = CONFIG_EXAMPLE_ETH_SPI_CLOCK_MHZ * 1000 * 1000;
    devcfg.queue_size = 20;

    for (int i = 0; i < CONFIG_EXAMPLE_SPI_ETHERNETS_NUM; i++)
    {
        devcfg.spics_io_num = spi_eth_module_config[i].spi_cs_gpio;

        spi_bus_add_device(HSPI_HOST, &devcfg, &spi_handle[i]);
        eth_w5500_config_t w5500_config = ETH_W5500_DEFAULT_CONFIG(spi_handle[i]);

        w5500_config.int_gpio_num = spi_eth_module_config[i].int_gpio;
        phy_config_spi.phy_addr = spi_eth_module_config[i].phy_addr;
        phy_config_spi.reset_gpio_num = spi_eth_module_config[i].phy_reset_gpio;

        mac_spi[i] = esp_eth_mac_new_w5500(&w5500_config, &mac_config_spi);
        phy_spi[i] = esp_eth_phy_new_w5500(&phy_config_spi);
    }

    for (int i = 0; i < CONFIG_EXAMPLE_SPI_ETHERNETS_NUM; i++)
    {
        esp_eth_config_t eth_config_spi = ETH_DEFAULT_CONFIG(mac_spi[i], phy_spi[i]);
        esp_eth_driver_install(&eth_config_spi, &eth_handle_spi[i]);

        uint8_t address[6] = {0x02, 0x00, 0x00, 0x12, 0x34, 0x56};
        address[5] += i;

        esp_eth_ioctl(eth_handle_spi[i], ETH_CMD_S_MAC_ADDR, address);

        esp_netif_attach(eth_netif_spi[i], esp_eth_new_netif_glue(eth_handle_spi[i]));
    }

    esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, NULL);

    for (int i = 0; i < CONFIG_EXAMPLE_SPI_ETHERNETS_NUM; i++)
    {
        esp_eth_start(eth_handle_spi[i]);
    }

    vTaskDelay(10000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "esp GET now");
    rest_get();

    vTaskDelay(3000/ portTICK_PERIOD_MS);
    ESP_LOGI(TAG, "esp POST now");
    post_rest_function();
}
