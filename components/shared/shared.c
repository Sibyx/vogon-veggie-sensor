#include <esp_log.h>
#include <string.h>
#include "shared.h"

static const char* TAG = "SHARED";

shared_data_t shared_data;
SemaphoreHandle_t data_mutex;
broadcasting_config_t broadcasting_config;

esp_err_t register_protocol_parameter(broadcasting_config_t *config, uint16_t sensor, uint8_t parameter, uint8_t position, uint8_t type) {
    // Allocate memory for the new item
    protocol_parameter_t *new_items = realloc(config->items, (config->size + 1) * sizeof(protocol_parameter_t));

    if (new_items == NULL) {
        // Handle memory allocation failure
        printf("Failed to allocate memory\n");
        ESP_LOGE(TAG, "Failed to allocate memory for protocol parameter");
        return ESP_FAIL;
    }

    // Update the items pointer to the new allocated memory
    config->items = new_items;

    // Populate the new protocol parameter
    protocol_parameter_t *new_item = &config->items[config->size];
    new_item->sensor = sensor;
    new_item->parameter = parameter;
    new_item->position = position;
    new_item->type = type;

    // Increment the size
    config->size += 1;

    return ESP_OK;
}

void init_broadcasting_config(broadcasting_config_t *config) {
    if (config->items != NULL) {
        free(config->items);
    }

    config->size = 0;
    config->items = NULL;

    // Sensor: BPM280(0x03), Parameter: BPM280 Temperature (0x01)
    ESP_ERROR_CHECK(
            register_protocol_parameter(
                    &broadcasting_config,
                    0x03,
                    0x1,
                    0,
                    PROTOCOL_TYPE_FLOAT
            ));

    // Sensor: BPM280(0x03), Parameter: BPM280 Humidity (0x02)
    ESP_ERROR_CHECK(
            register_protocol_parameter(
                    &broadcasting_config,
                    0x03,
                    0x2,
                    4,
                    PROTOCOL_TYPE_FLOAT
            ));

    // Sensor: BPM280(0x03), Parameter: BPM280 Pressure (0x03)
    ESP_ERROR_CHECK(
            register_protocol_parameter(
                    &broadcasting_config,
                    0x03,
                    0x3,
                    8,
                    PROTOCOL_TYPE_FLOAT
            ));

    // Sensor: ANALOG_SOIL_MOISTURE(0x06), Parameter: Soil moisture (0x01)
    ESP_ERROR_CHECK(
            register_protocol_parameter(
                    &broadcasting_config,
                    0x06,
                    0x1,
                    12,
                    PROTOCOL_TYPE_UINT8
            ));

    // Sensor: VCNL4040(0x05), Parameter: Lux (0x01)
    ESP_ERROR_CHECK(
            register_protocol_parameter(
                    &broadcasting_config,
                    0x05,
                    0x1,
                    13,
                    PROTOCOL_TYPE_UINT16
            ));

    // Sensor: SEN0605(0x04), Parameter: Nitrogen (0x01)
    ESP_ERROR_CHECK(
            register_protocol_parameter(
                    &broadcasting_config,
                    0x04,
                    0x1,
                    15,
                    PROTOCOL_TYPE_UINT16
            ));

    // Sensor: SEN0605(0x04), Parameter: Phosphorous (0x02)
    ESP_ERROR_CHECK(
            register_protocol_parameter(
                    &broadcasting_config,
                    0x04,
                    0x2,
                    17,
                    PROTOCOL_TYPE_UINT16
            ));

    // Sensor: SEN0605(0x04), Parameter: Potassium (0x03)
    ESP_ERROR_CHECK(
            register_protocol_parameter(
                    &broadcasting_config,
                    0x04,
                    0x3,
                    19,
                    PROTOCOL_TYPE_UINT16
            ));
}

void create_mfg_data(protocol_parameter_t *parameter, shared_data_t *data, uint8_t **mfg_data, uint8_t *mfg_data_len) {
    protocol_message_t message;

    message.parameter = parameter->parameter;
    message.sensor = parameter->sensor;
    message.type = parameter->type;

    size_t payload_size = 0;

    switch (message.type) {
        case PROTOCOL_TYPE_UINT8:
            payload_size = sizeof(uint8_t);
            break;
        case PROTOCOL_TYPE_UINT16:
            payload_size = sizeof(uint16_t);
            break;
        case PROTOCOL_TYPE_UINT32:
            payload_size = sizeof(uint32_t);
            break;
        case PROTOCOL_TYPE_FLOAT:
            payload_size = sizeof(float);
            break;
        case PROTOCOL_TYPE_DOUBLE:
            payload_size = sizeof(double);
            break;
    }

    *mfg_data_len = sizeof(protocol_message_t) + payload_size;
    *mfg_data = (uint8_t *) malloc(*mfg_data_len);

    // Copy header
    memcpy(*mfg_data, &message, sizeof(protocol_message_t));

    // Copy measurement value
    memcpy(*mfg_data + sizeof(protocol_message_t), data + parameter->position, payload_size);
}
