#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sensors.h"

#define TXD_PIN (GPIO_NUM_17) // Use GPIO17 for TX
#define RXD_PIN (GPIO_NUM_16) // Use GPIO16 for RX
#define RST_PIN (GPIO_NUM_06) // Use GPIO16 for RX
#define UART_BUFFER (128)

static const char* TAG = "SEN0605";

//TODO: Maybe recreate as struct
static const uint8_t command[] = {0x01,0x03, 0x00, 0x1e, 0x00, 0x03, 0x34, 0x0D};

static void print_hex(const uint8_t *data, size_t length)
{
    printf("Received data in hex: ");
    for (size_t i = 0; i < length; ++i)
    {
        printf("%02X ", data[i]);
    }
    printf("\n");
}

_Noreturn void sen0605_task(void *pvParameters) {
    const int uart_buffer_size = (1024 * 2);
    QueueHandle_t uart_queue;
    const uart_config_t uart_config = {
            .baud_rate = 9600,
            .data_bits = UART_DATA_8_BITS,
            .parity = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    ESP_ERROR_CHECK(uart_param_config(UART_NUM_2, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_2, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_2, uart_buffer_size, uart_buffer_size, 10, &uart_queue, 0));
//    ESP_ERROR_CHECK(uart_set_mode(UART_NUM_2, UART_MODE_RS485_HALF_DUPLEX));

    uint16_t nitrogen = 0;
    uint16_t phosphorous = 0;
    uint16_t potassium = 0;
    uint8_t data[128];
    memset(&data, 0, 128);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(5 * CONFIG_SENSORS_SLEEP_PERIOD));

        int len;

        // NPK
        uart_write_bytes(UART_NUM_2, (const char *)command, 8);
        ESP_ERROR_CHECK(uart_wait_tx_done(UART_NUM_2, 250));
        memset(&data, 0, 128);
        len = uart_read_bytes(UART_NUM_2, data, 11, pdMS_TO_TICKS(500));
        if (len > 0) {
            print_hex(data, len);
            nitrogen = *((uint16_t *)(data + 4));
            phosphorous = *((uint16_t *)(data + 6));
            potassium = *((uint16_t *)(data + 8));
            ESP_LOGI(TAG, "Nitrogen: %d; Phosphorous: %d; Potassium: %d", nitrogen, phosphorous, potassium);
        } else {
            ESP_LOGE(TAG, "Error reading response: %d", len);
        }

        // Store measurements to shared memory
        if (xSemaphoreTake(data_mutex, portMAX_DELAY) == pdTRUE) {
            shared_data.nitrogen = nitrogen;
            shared_data.phosphorous = phosphorous;
            shared_data.potassium = potassium;

            xSemaphoreGive(data_mutex);
        }
    }
}
