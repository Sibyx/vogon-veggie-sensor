#include <sys/cdefs.h>
#include "sensors.h"

static const char* TAG = "BPM280_MODULE";

/**
 * Datasheet: https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bme280-ds002.pdf
 * @param pvParameters
 */
_Noreturn void bmp280_task(void *pvParameters)
{
    bmp280_params_t params;
    bmp280_init_default_params(&params);
    bmp280_t dev;
    memset(&dev, 0, sizeof(bmp280_t));

    ESP_ERROR_CHECK(bmp280_init_desc(&dev, BMP280_I2C_ADDRESS_0, 0, CONFIG_SENSORS_I2C_MASTER_SDA, CONFIG_SENSORS_I2C_MASTER_SCL));
    ESP_ERROR_CHECK(bmp280_init(&dev, &params));

    bool bme280p = dev.id == BME280_CHIP_ID;
    ESP_LOGI(TAG, "Found %s", bme280p ? "BME280" : "BMP280");

    float pressure = 0, temperature = 0, humidity = 0;

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(CONFIG_SENSORS_SLEEP_PERIOD));
        if (bmp280_read_float(&dev, &temperature, &pressure, &humidity) != ESP_OK)
        {
            ESP_LOGW(TAG, "Temperature/pressure reading failed");
            continue;
        }

        ESP_LOGI(TAG, "Pressure: %.2f Pa", pressure);
        ESP_LOGI(TAG, "Temperature: %.2f C", temperature);

        if (bme280p) {
            ESP_LOGI(TAG, "Humidity: %.2f%%", humidity);
        }

        // Store measurements to shared memory
        if (xSemaphoreTake(data_mutex, portMAX_DELAY) == pdTRUE) {
            shared_data.pressure = pressure;
            shared_data.temperature = temperature;
            shared_data.humidity = humidity;

            xSemaphoreGive(data_mutex);
        }
    }
}