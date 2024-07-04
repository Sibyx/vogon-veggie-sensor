#include "driver/adc.h"
#include "esp_log.h"
#include "math.h"
#include "sensors.h"

static const char* TAG = "MOISTURE";

_Noreturn void moisture_task(void *pvParameters) {
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_12);

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(CONFIG_SENSORS_MOISTURE_SLEEP_PERIOD * 60 * 1000));

        uint16_t analog = adc1_get_raw(ADC1_CHANNEL_6);

        double moisture_double = ceil(( 100 - ( (analog / 4095.00) * 100 ) ) * 100);
        uint16_t moisture;

        if (moisture_double > UINT16_MAX) {
            moisture = UINT16_MAX;
        } else if (moisture_double < 0) {
            moisture = 0;
        } else {
            moisture = (uint16_t) moisture_double;
        }

        ESP_LOGD(TAG, "Soil moisture (raw): %d", analog);
        ESP_LOGD(TAG, "Soil moisture: %d%%", moisture);

        // Store measurements to shared memory
        if (xSemaphoreTake(data_mutex, portMAX_DELAY) == pdTRUE) {
            shared_data.moisture = moisture;
            shared_data.moisture_analog = analog;
            xSemaphoreGive(data_mutex);
        }
    }
}
