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

    FILE *file = fopen("/spiffs/public.pub", "r");
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

    printf("\n Now open the private.pem file \n\n");

    FILE *privFile = fopen("/spiffs/private.pem", "r");
    if (privFile == NULL)
    {
        ESP_LOGE(TAG, "privFile does not exist!");
    }
    else
    {
        char line[256];
        while (fgets(line, sizeof(line), privFile) != NULL)
        {
            printf(line);
        }
        fclose(privFile);
    }

    esp_vfs_spiffs_unregister(NULL);
}
