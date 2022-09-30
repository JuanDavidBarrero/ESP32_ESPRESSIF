#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/i2c.h"

static const char *TAG = "SLAVE";

#define I2C_SLAVE_SCL_IO 22
#define I2C_SLAVE_SDA_IO 21
#define I2C_SLAVE_FREQ_HZ 100000
#define I2C_SLAVE_TX_BUF_LEN 255
#define I2C_SLAVE_RX_BUF_LEN 16
#define ESP_SLAVE_ADDR 0x0A

#define WRITE_BIT I2C_MASTER_WRITE
#define READ_BIT I2C_MASTER_READ
#define ACK_CHECK_EN 0x1
#define ACK_CHECK_DIS 0x0
#define ACK_VAL 0x0
#define NACK_VAL 0x1

int i2c_slave_port = 0;
uint8_t received_data[I2C_SLAVE_RX_BUF_LEN] = {0};
uint8_t transmit_data[I2C_SLAVE_RX_BUF_LEN] = {0};

esp_err_t i2c_slave_init(void);

void app_main()
{

    ESP_LOGI(TAG, "I2C Slave init");

    i2c_slave_init();

    while (true)
    {
        esp_err_t ret;
        ret = i2c_slave_read_buffer(i2c_slave_port, received_data, I2C_SLAVE_RX_BUF_LEN, portMAX_DELAY);
        i2c_reset_rx_fifo(i2c_slave_port);
        printf((char *)received_data);
        if (ret != ESP_FAIL)
        {
            sprintf(((char *)transmit_data), "test es buena\n");
            i2c_slave_write_buffer(i2c_slave_port, transmit_data, 16, 100 / portTICK_PERIOD_MS);
        }
        vTaskDelay(300 / portTICK_PERIOD_MS);
    }
}

esp_err_t i2c_slave_init(void)
{
    i2c_config_t conf_slave = {
        .sda_io_num = I2C_SLAVE_SDA_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .mode = I2C_MODE_SLAVE,
        .slave.addr_10bit_en = 0,
        .slave.slave_addr = ESP_SLAVE_ADDR,
        .clk_flags = 0,
    };

    esp_err_t err = i2c_param_config(i2c_slave_port, &conf_slave);

    if (err != ESP_OK)
    {
        return err;
    }

    return i2c_driver_install(i2c_slave_port, conf_slave.mode, I2C_SLAVE_RX_BUF_LEN, I2C_SLAVE_TX_BUF_LEN, 0);
}