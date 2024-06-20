#include "sensors.h"
#include "vcnl4040.h"
#include <esp_log.h>
#include <string.h>

#define I2C_FREQ_HZ 1000000 // Max 1MHz for esp-idf

#define CHECK(x) do { esp_err_t __; if ((__ = x) != ESP_OK) return __; } while (0)
#define CHECK_ARG(VAL) do { if (!(VAL)) return ESP_ERR_INVALID_ARG; } while (0)

#define GET_LSB(value) ((uint8_t)((value) & 0xFF))
#define GET_MSB(value) ((uint8_t)(((value) >> 8) & 0xFF))
#define SET_LSB(value, lsb) ((value) = ((value) & 0xFF00) | (lsb))
#define SET_MSB(value, msb) ((value) = ((value) & 0x00FF) | ((msb) << 8))
#define CONCAT_BYTES(msb, lsb) (((uint16_t)(msb) << 8) | (lsb))

static const char* TAG = "VCNL4040";

static void print_binary(const void *value, size_t size) {
    const uint8_t *byte_ptr = (const uint8_t *)value;

    printf("0b");
    for (size_t i = 0; i < size; i++) {
        for (int bit = 7; bit >= 0; bit--) {
            printf("%c", (byte_ptr[size - 1 - i] & (1 << bit)) ? '1' : '0');
        }
    }
    printf("\n");
}

static esp_err_t vcnl4040_write(i2c_dev_t *dev, uint8_t command, uint16_t data) {
    return i2c_dev_write(dev, &command, 1, &data, 2);
}

static esp_err_t vcnl4040_read(i2c_dev_t *dev, uint8_t command, uint16_t *data) {
    return i2c_dev_read(dev, &command, 1, data, 2);
}


esp_err_t vcnl4040_init_desc(vcnl4040_t *dev, uint8_t addr, i2c_port_t port, gpio_num_t sda_gpio, gpio_num_t scl_gpio)
{
    CHECK_ARG(dev);

    if (addr != VCNL4040_I2C_ADDRESS)
    {
        ESP_LOGE(TAG, "Invalid I2C address");
        return ESP_ERR_INVALID_ARG;
    }

    dev->i2c_dev.port = port;
    dev->i2c_dev.addr = addr;
    dev->i2c_dev.cfg.sda_io_num = sda_gpio;
    dev->i2c_dev.cfg.scl_io_num = scl_gpio;
#if HELPER_TARGET_IS_ESP32
    dev->i2c_dev.cfg.master.clk_speed = I2C_FREQ_HZ;
#endif

    return i2c_dev_create_mutex(&dev->i2c_dev);
}

esp_err_t vcnl4040_init_default_params(vcnl4040_params_t *params)
{
    CHECK_ARG(params);

    // ALS
    params->ALS_CONF = VCNL4040_ALS_IT_640MS | VCNL4040_ALS_PERS_1 | VCNL4040_ALS_INT_ENABLE | VCNL4040_ALS_SD_POWER_ON;
    params->ALS_THDH = 0;
    params->ALS_THDL = 0;

    // PS
    params->PS_CONF1 = VCNL4040_PS_DUTY_1_40 | VCNL4040_PS_PERS_1 | VCNL4040_PS_IT_1T | VCNL4040_PS_SD_ON;
    params->PS_CONF2 = VCNL4040_PS_HD_12BIT | VCNL4040_PS_INT_WHEN_CLOSE;
    params->PS_CONF3 = VCNL4040_PS_MPS_1 | VCNL4040_PS_SMART_PERS_ENABLE | VCNL4040_PS_NORMAL_MODE | VCNL4040_PS_TRIG_ACTIVE | VCNL4040_PS_SC_ENABLE;
    params->PS_MS = VCNL4040_WHITE_ENABLED | VCNL4040_PS_MS_NORMAL | VCNL4040_LED_I_75;
    params->PS_CANC = 0;
    params->PS_THDL = 0;
    params->PS_THDH = 0;

    return ESP_OK;
}

esp_err_t vcnl4040_init(vcnl4040_t *dev, vcnl4040_params_t *params) {
    CHECK_ARG(dev && params);

    I2C_DEV_TAKE_MUTEX(&dev->i2c_dev);

    vcnl4040_read(&dev->i2c_dev, VCNL4040_RD_REG_DEVICE, &dev->id);
    if (dev->id == 0) {
        ESP_LOGE(TAG, "Unable to read from device!");
        return ESP_FAIL;
    }

    // Write configuration
    vcnl4040_write(&dev->i2c_dev,VCNL4040_WR_REG_ALS_CONF,CONCAT_BYTES(params->ALS_CONF, 0));
    vcnl4040_write(&dev->i2c_dev,VCNL4040_WR_REG_ALS_THDH,params->ALS_THDH);
    vcnl4040_write(&dev->i2c_dev,VCNL4040_WR_REG_ALS_THDL,params->ALS_THDL);
    vcnl4040_write(&dev->i2c_dev,VCNL4040_WR_REG_PS_CONF,CONCAT_BYTES(params->PS_CONF1, params->PS_CONF2));
    vcnl4040_write(&dev->i2c_dev,VCNL4040_WR_REG_PS_MS,CONCAT_BYTES(params->PS_CONF3, params->PS_MS));
    vcnl4040_write(&dev->i2c_dev,VCNL4040_WR_REG_PS_CANC,params->PS_CANC);
    vcnl4040_write(&dev->i2c_dev,VCNL4040_WR_REG_PS_THDL,params->PS_THDL);
    vcnl4040_write(&dev->i2c_dev,VCNL4040_WR_REG_PS_THDH,params->PS_THDH);

    return ESP_OK;
}

esp_err_t vcnl4040_read_proximity(vcnl4040_t *dev, uint16_t *data) {
    esp_err_t rs = vcnl4040_read(&dev->i2c_dev, VCNL4040_RD_REG_PS_DATA, data);
    ESP_LOGD(TAG, "Proximity: %d", *data);
    return rs;
}

esp_err_t vcnl4040_read_lux(vcnl4040_t *dev, uint16_t *data) {
    esp_err_t rs = vcnl4040_read(&dev->i2c_dev, VCNL4040_RD_REG_ALS_DATA, data);
    ESP_LOGD(TAG, "Lux: %d", *data);
    return rs;
}

esp_err_t vcnl4040_read_white(vcnl4040_t *dev, uint16_t *data) {
    esp_err_t rs = vcnl4040_read(&dev->i2c_dev, VCNL4040_RD_REG_WHITE_DATA, data);
    ESP_LOGD(TAG, "White: %d", *data);
    return rs;
}

_Noreturn void vcnl4040_task(void *pvParameters)
{
    vcnl4040_params_t params;
    memset(&params, 0, sizeof(vcnl4040_params_t));
    vcnl4040_init_default_params(&params);

    params.ALS_CONF = VCNL4040_ALS_IT_640MS | VCNL4040_ALS_PERS_1 | VCNL4040_ALS_INT_ENABLE | VCNL4040_ALS_SD_POWER_ON;
    params.PS_CONF1 = VCNL4040_PS_DUTY_1_40 | VCNL4040_PS_PERS_1 | VCNL4040_PS_IT_1T | VCNL4040_PS_SD_OFF;
    params.PS_CONF2 = VCNL4040_PS_HD_12BIT | VCNL4040_PS_INT_WHEN_CLOSE;
    params.PS_CONF3 = VCNL4040_PS_MPS_1 | VCNL4040_PS_SMART_PERS_DISABLE | VCNL4040_PS_NORMAL_MODE | VCNL4040_PS_TRIG_INACTIVE | VCNL4040_PS_SC_DISABLE;
    params.PS_MS = VCNL4040_WHITE_DISABLED | VCNL4040_PS_MS_NORMAL | VCNL4040_LED_I_50;

    vcnl4040_t dev;
    memset(&dev, 0, sizeof(vcnl4040_t));

    ESP_ERROR_CHECK(vcnl4040_init_desc(&dev, VCNL4040_I2C_ADDRESS, 0, CONFIG_SENSORS_I2C_MASTER_SDA, CONFIG_SENSORS_I2C_MASTER_SCL));
    ESP_ERROR_CHECK(vcnl4040_init(&dev, &params));

    uint16_t proximity = 0, lux = 0, white = 0;

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(CONFIG_SENSORS_SLEEP_PERIOD));

        if (vcnl4040_read_proximity(&dev, &proximity) != ESP_OK)
        {
            ESP_LOGW(TAG, "Proximity reading failed");
        }

        if (vcnl4040_read_lux(&dev, &lux) != ESP_OK)
        {
            ESP_LOGW(TAG, "Lux reading failed");
        }

        if (vcnl4040_read_white(&dev, &white) != ESP_OK)
        {
            ESP_LOGW(TAG, "White reading failed");
        }

        // Store measurements to shared memory
        if (xSemaphoreTake(data_mutex, portMAX_DELAY) == pdTRUE) {
            shared_data.lux = lux;

            xSemaphoreGive(data_mutex);
        }
    }
}