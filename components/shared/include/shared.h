#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

typedef struct {
    float temperature;
    float humidity;
    float pressure;
    uint16_t lux;
} shared_data_t;

extern shared_data_t shared_data;
extern SemaphoreHandle_t data_mutex;
