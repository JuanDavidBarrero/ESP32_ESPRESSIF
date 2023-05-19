#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "sdkconfig.h"
#include "esp_int_wdt.h"
#include "esp_task_wdt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "TIMER";

typedef struct
{
    void (*callback)();
    uint32_t interval;
    uint32_t last_execution;
} TimerCallbackData;

class TimerManager
{
private:
    TimerCallbackData *callbacks;
    int numCallbacks;
    esp_timer_handle_t periodic_timer;

public:
    TimerManager()
    {
        callbacks = NULL;
        numCallbacks = 0;
    }

    ~TimerManager()
    {
        if (callbacks != NULL)
        {
            delete[] callbacks;
        }
    }

    void start()
    {
        esp_timer_create_args_t periodic_timer_args = {};
        periodic_timer_args.callback = TimerManager::periodic_timer_callback_wrapper;
        periodic_timer_args.arg = this;
        periodic_timer_args.name = "periodic";

        esp_timer_create(&periodic_timer_args, &periodic_timer);
        esp_timer_start_periodic(periodic_timer, 1000); // 1 millisecond interval
    }

    void addCallback(void (*callback)(), uint32_t interval)
    {
        TimerCallbackData *newCallbacks = new TimerCallbackData[numCallbacks + 1];

        // Copy existing callbacks
        for (int i = 0; i < numCallbacks; i++)
        {
            newCallbacks[i] = callbacks[i];
        }

        // Add new callback
        newCallbacks[numCallbacks].callback = callback;
        newCallbacks[numCallbacks].interval = interval;
        newCallbacks[numCallbacks].last_execution = 0;

        // Delete old callbacks array
        if (callbacks != NULL)
        {
            delete[] callbacks;
        }

        // Update callbacks array and count
        callbacks = newCallbacks;
        numCallbacks++;
    }

    void removeCallback(void (*callback)())
    {
        for (int i = 0; i < numCallbacks; i++)
        {
            if (callbacks[i].callback == callback)
            {
                // Shift elements to remove callback
                for (int j = i; j < numCallbacks - 1; j++)
                {
                    callbacks[j] = callbacks[j + 1];
                }
                numCallbacks--;
                break;
            }
        }
    }

private:
    static void periodic_timer_callback_wrapper(void *arg)
    {
        TimerManager *manager = static_cast<TimerManager *>(arg);
        manager->periodic_timer_callback();
    }

    void periodic_timer_callback()
    {
        uint32_t current_time = esp_timer_get_time() / 1000; // Convert to milliseconds

        for (int i = 0; i < numCallbacks; i++)
        {
            if (current_time - callbacks[i].last_execution >= callbacks[i].interval)
            {
                callbacks[i].callback();
                callbacks[i].last_execution = current_time;
            }
        }
        esp_int_wdt_init();

        esp_task_wdt_init(5, false);
        esp_task_wdt_add(NULL);
        esp_task_wdt_reset(); // Reset the task watchdog
    }
};

void callback1()
{
    ESP_LOGW(TAG, "\tCallback 1 called");
}

void callback2()
{
    ESP_LOGW(TAG, "\t\t\tCallback 2 called");
}

extern "C" void app_main()
{
    TimerManager timerManager;
    timerManager.addCallback(callback1, 2000);
    timerManager.addCallback(callback2, 5000);
    timerManager.start();

    uint16_t counter = 0;

    while (1)
    {
        printf("Hello world %i\n", counter);
        if (counter == 15)
        {
            timerManager.removeCallback(callback1);
        }
        counter++;
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
