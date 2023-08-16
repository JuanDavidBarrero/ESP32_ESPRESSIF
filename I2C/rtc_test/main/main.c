#include "esp_log.h"
#include "driver/i2c.h"

#define I2C_MASTER_SCL_IO 22
#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_TX_BUF_DISABLE 0
#define I2C_MASTER_RX_BUF_DISABLE 0
#define I2C_MASTER_FREQ_HZ 100000
#define dirRTC 0x68

static const char *TAG = "DS3231_RTC";

uint8_t bcdToDec(uint8_t val)
{
    return ((val / 16 * 10) + (val % 16));
}

uint8_t decToBcd(uint8_t val)
{
    return ((val / 10 * 16) + (val % 10));
}

void initI2C()
{
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

esp_err_t readDateTime(uint8_t *data)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (dirRTC << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x00, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (dirRTC << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, data, 7, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

void printDateTime(uint8_t *data)
{
    uint8_t second = bcdToDec(data[0] & 0x7F);
    uint8_t minute = bcdToDec(data[1] & 0x7F);
    uint8_t hour = bcdToDec(data[2] & 0x3F);
    uint8_t day = bcdToDec(data[4] & 0x7F);
    uint8_t month = bcdToDec(data[5] & 0x3F);
    uint8_t year = bcdToDec(data[6]);

    ESP_LOGI(TAG, "DateTime: %02d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, minute, second);
}

void setDateTime(uint8_t second, uint8_t minute, uint8_t hour, uint8_t day, uint8_t month, uint8_t year)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (dirRTC << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x00, true);
    i2c_master_write_byte(cmd, decToBcd(second), true);
    i2c_master_write_byte(cmd, decToBcd(minute), true);
    i2c_master_write_byte(cmd, decToBcd(hour), true);
    i2c_master_write_byte(cmd, 0, true); // Day of the week (not used)
    i2c_master_write_byte(cmd, decToBcd(day), true);
    i2c_master_write_byte(cmd, decToBcd(month), true);
    i2c_master_write_byte(cmd, decToBcd(year), true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Error setting DateTime on DS3231");
    }
}

void app_main()
{
    initI2C();

    while (1)
    {
        ESP_LOGI(TAG, "Reading DateTime from DS3231...");

        uint8_t data[7];
        esp_err_t ret = readDateTime(data);

        if (ret == ESP_OK)
        {
            printDateTime(data);
        }
        else
        {
            ESP_LOGE(TAG, "Error reading data from DS3231");
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
