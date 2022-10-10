#include <esp_log.h>
#include <inttypes.h>
#include "rc522.h"

static const char* TAG = "rc522-demo";
static rc522_handle_t scanner;

static void rc522_handler(void* arg, esp_event_base_t base, int32_t event_id, void* event_data)
{
    rc522_event_data_t* data = (rc522_event_data_t*) event_data;

    switch(event_id) {
        case RC522_EVENT_TAG_SCANNED: {
                rc522_tag_t* tag = (rc522_tag_t*) data->ptr;
                ESP_LOGI(TAG, "Tag scanned (sn: %" PRIu64 ")", tag->serial_number);
                if (tag->serial_number == 867619030035){
                    printf("Access successfully\n");
                }
                else
                {
                    printf("Access denied\n");
                }
                
            }
            break;
    }
}

void app_main()
{
    rc522_config_t config = {
        .spi.host = VSPI_HOST,
        .spi.mosi_gpio = 23,        
        .spi.miso_gpio = 19,
        .spi.sck_gpio = 18,
        .spi.sda_gpio = 5,
    };

    rc522_create(&config, &scanner);
    rc522_register_events(scanner, RC522_EVENT_ANY, rc522_handler, NULL);
    rc522_start(scanner);
}