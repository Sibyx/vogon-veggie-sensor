#include <sys/cdefs.h>

#pragma once

#include "string.h"
#include "esp_log.h"
#include "bmp280.h"
#include "shared.h"

_Noreturn
void bmp280_task(void *pvParameters);
void vcnl4040_task(void *pvParameters);
