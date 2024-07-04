#ifndef SHARED_H
#define SHARED_H

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#define PROTOCOL_TYPE_UINT8 1
#define PROTOCOL_TYPE_UINT16 2
#define PROTOCOL_TYPE_UINT32 3
#define PROTOCOL_TYPE_FLOAT 4
#define PROTOCOL_TYPE_DOUBLE 5

typedef struct {
    float temperature; // Start byte: 0
    float humidity;  // Start byte: 4
    float pressure;  // Start byte: 8
    uint16_t moisture;  // Start byte: 12
    uint16_t moisture_analog;  // Start byte: 14
    uint16_t lux;  // Start byte: 16
    uint16_t nitrogen;  // Start byte: 18
    uint16_t phosphorous;  // Start byte: 20
    uint16_t potassium;  // Start byte: 22
} shared_data_t;

typedef struct {
    uint16_t sensor;
    uint8_t parameter;
    uint8_t type;
} protocol_message_t;

typedef struct {
    uint16_t sensor;
    uint8_t parameter;
    uint8_t position;
    uint8_t type;
} protocol_parameter_t;

typedef struct {
    uint8_t size;
    protocol_parameter_t *items;
} broadcasting_config_t;


extern const int SLEEP_TIMES_SIZE;
extern const int SLEEP_TIMES[];

/**
 * Registration for BLE broadcasting
 * @param config broadcasting_config_t instance
 * @param sensor Numeric code for sensor (e.g. DHT22, BPM280) - according to the configuration in cloud database
 * @param parameter Numeric code for parameter measured by sensor (e.g. temperature, humidity...) - according to the configuration in cloud database
 * @param position Position in the shared_data_t struct in bytes (used for pointer arithmetics)
 * @param type Data type of the measurement (according to the PROTOCOL_TYPE_ constants)
 */
esp_err_t register_protocol_parameter(broadcasting_config_t *config, uint16_t sensor, uint8_t parameter, uint8_t position, uint8_t type);

/**
 * Initialize broadcast config.
 * According to this the BLE Advertisement messages are build and changed.
 * @param config
 */
void init_broadcasting_config(broadcasting_config_t *config);

void create_mfg_data(protocol_parameter_t *parameter, shared_data_t *data, uint8_t **mfg_data, uint8_t *mfg_data_len);

int find_max(const int data[], int size);
int find_min(const int data[], int size);

extern shared_data_t shared_data;
extern SemaphoreHandle_t data_mutex;
extern broadcasting_config_t broadcasting_config;

#endif // SHARED_H
