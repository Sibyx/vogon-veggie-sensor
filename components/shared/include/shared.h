#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

typedef struct {
    float temperature;
    float humidity;
    float pressure;
    uint8_t moisture;
    uint16_t lux;
    uint16_t nitrogen;
    uint16_t phosphorous;
    uint16_t potassium;
} shared_data_t;

extern shared_data_t shared_data;
extern SemaphoreHandle_t data_mutex;
