#include "driver/adc.h"
#include "esp_log.h"
#include "math.h"
#include "sensors.h"

static const char* TAG = "MOISTURE";

_Noreturn void moisture_task(void *pvParameters) {
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_12);

    while (1) {
        // TODO: create MIN_TO_TICKS
        vTaskDelay(pdMS_TO_TICKS(CONFIG_SENSORS_MOISTURE_SLEEP_PERIOD * 60 * 1000));

        int analog = adc1_get_raw(ADC1_CHANNEL_6);
        double moisture_double = ceil(( 100 - ( (analog / 4095.00) * 100 ) ) * 100);
        uint8_t moisture;

        if (moisture_double > UINT8_MAX) {
            moisture = UINT8_MAX;
        } else if (moisture_double < 0) {
            moisture = 0;
        } else {
            moisture = (uint8_t) moisture_double;
        }

        ESP_LOGD(TAG, "Soil moisture: %d%%", moisture);

        // Store measurements to shared memory
        if (xSemaphoreTake(data_mutex, portMAX_DELAY) == pdTRUE) {
            shared_data.moisture = moisture;
            xSemaphoreGive(data_mutex);
        }
    }
}
