#include <stdio.h>
#include "esp_spiffs.h"
#include "esp_log.h"

#define TAG "spiffs"

extern "C" void app_main(void)
{
    esp_vfs_spiffs_conf_t config = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true,
    };
    esp_vfs_spiffs_register(&config);

    ESP_LOGE(TAG, "Creating New file: hello.txt");
    FILE *f = fopen("/spiffs/hello.txt", "w");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return;
    }

    ESP_LOGE(TAG, "Writing data to file: hello.txt");
    fprintf(f, "Welcome to this test from Hades the white wolf\n"); 
    fclose(f);
    ESP_LOGI(TAG, "File written");

    ESP_LOGE(TAG, "Reading data from file: hello.txt");
    FILE *file = fopen("/spiffs/hello.txt", "r");
    if (file == NULL)
    {
        ESP_LOGE(TAG, "File does not exist!");
    }
    else
    {
        char line[256];
        while (fgets(line, sizeof(line), file) != NULL)
        {
            printf(line);
        }
        fclose(file);
    }
    esp_vfs_spiffs_unregister(NULL);
}
