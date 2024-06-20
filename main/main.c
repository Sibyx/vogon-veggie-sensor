#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "sensors.h"
#include "esp_log.h"
#include "shared.h"
#include "bluetooth.h"
#include "nvs_flash.h"

static const char* TAG = "MAIN_MODULE";

void app_main(void)
{
    int rc;

    ESP_LOGI(TAG, "Booting VogonVeggieSensor");

    ESP_ERROR_CHECK(i2cdev_init());

    // Initialize the mutex
    data_mutex = xSemaphoreCreateMutex();
    if (data_mutex == NULL) {
        ESP_LOGE(TAG, "Unable to initialize shared memory mutex");
    }

    // Initialize shared memory
    // FIXME: probably not a good idea to use 0
    shared_data.humidity = 0;
    shared_data.temperature = 0;
    shared_data.pressure = 0;

    // Initialize NVS
    rc = nvs_flash_init();
    if (rc != ESP_OK && rc != ESP_ERR_NVS_NO_FREE_PAGES && rc != ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(rc);
    }

    // Initialize the NimBLE host
    nimble_port_init();

    // Configure the host stack
    ble_hs_cfg.sync_cb = bleprph_on_sync;
    ble_hs_cfg.reset_cb = bleprph_on_reset;

    ESP_LOGD(TAG, "VogonVeggie initialized. Pinning tasks to core.");

    xTaskCreatePinnedToCore(
            bmp280_task,
            "bmp280",
            configMINIMAL_STACK_SIZE * 8,
            NULL,
            10,
            NULL,
            APP_CPU_NUM
            );

    xTaskCreatePinnedToCore(
            vcnl4040_task,
            "vcnl4040",
            configMINIMAL_STACK_SIZE * 8,
            NULL,
            10,
            NULL,
            APP_CPU_NUM
    );

//    xTaskCreatePinnedToCore(
//            sen0605_task,
//            "sen0605",
//            configMINIMAL_STACK_SIZE * 8,
//            NULL,
//            10,
//            NULL,
//            APP_CPU_NUM
//    );

    xTaskCreatePinnedToCore(
            moisture_task,
            "moisture",
            configMINIMAL_STACK_SIZE * 8,
            NULL,
            10,
            NULL,
            APP_CPU_NUM
    );

    // Start the NimBLE host task
    nimble_port_freertos_init(bleprph_host_task);

    xTaskCreatePinnedToCore(
            update_sensor_values_task,
            "update_sensor_values_task",
            configMINIMAL_STACK_SIZE * 8,
            NULL,
            5,
            NULL,
            APP_CPU_NUM
    );

    ESP_LOGD(TAG, "All tasks are pinned!");
}
