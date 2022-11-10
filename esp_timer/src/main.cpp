#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "sdkconfig.h"

static const char *TAG = "TIMER";

static void periodic_timer_callback(void *arg);
static void oneshot_timer_callback(void *arg);

extern "C" void app_main()
{

    esp_timer_create_args_t periodic_timer_args = {};
    periodic_timer_args.callback = &periodic_timer_callback;
    periodic_timer_args.name = "perdiodic";

    esp_timer_handle_t periodic_timer;

    esp_timer_create(&periodic_timer_args, &periodic_timer);

    esp_timer_create_args_t oneshot_timer_args = {};
    oneshot_timer_args.callback = &oneshot_timer_callback;
    oneshot_timer_args.name = "one-shot";
    oneshot_timer_args.arg = (void *)periodic_timer;

    esp_timer_handle_t oneshot_timer;
    esp_timer_create(&oneshot_timer_args, &oneshot_timer);


    esp_timer_start_once(oneshot_timer, 5000000);
    esp_timer_start_periodic(periodic_timer, 1000000);
}

static void periodic_timer_callback(void *arg)
{
    int64_t time_since_boot = esp_timer_get_time();
    ESP_LOGI(TAG, "Periodic timer called, time since boot: %lld us", time_since_boot);
}

static void oneshot_timer_callback(void* timer)
{
    int64_t time_since_boot = esp_timer_get_time();
    ESP_LOGI(TAG, "One-shot timer called, time since boot: %lld us", time_since_boot);
    esp_timer_handle_t periodic_timer_handle = (esp_timer_handle_t) timer;

    esp_timer_stop(periodic_timer_handle);
    esp_timer_start_periodic(periodic_timer_handle, 5000000);
    time_since_boot = esp_timer_get_time();
    ESP_LOGW(TAG, "Restarted periodic timer with 1s period, time since boot: %lld us",time_since_boot);
}