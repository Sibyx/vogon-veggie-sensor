#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/ble_gatt.h"
#include "host/util/util.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "shared.h"

// Tasks
_Noreturn void update_sensor_values_task(void *pvParameters);

// BLE shit
void bleprph_on_reset(int reason);
void bleprph_on_sync(void);
void bleprph_host_task(void *param);


// Helpers
void update_advertisement_data(protocol_parameter_t *parameter);

