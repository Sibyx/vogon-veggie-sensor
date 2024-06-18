/**
 * @file vcnl4040.h
 * @defgroup vcnl4040 vcnl4040
 *
 * ESP-IDF driver for VCNL4040 Proximity and Lux Sensor.
 * Based on the BMP280/BME280 driver for ESP-IDF
 *
 * Datasheet: https://www.vishay.com/docs/84274/vcnl4040.pdf
 * Design manual: https://www.vishay.com/docs/84307/designingvcnl4040.pdf
 *
 * Copyright (c) 2016 sheinz <https://github.com/sheinz>\n
 * Copyright (c) 2018 Ruslan V. Uss <unclerus@gmail.com>
 * Copyright (c) 2024 Jakub Dubec <https://github.com/Sibyx>
 *
 * MIT Licensed as described in the file LICENSE
 */

#ifndef VCNL4040_DRIVER_H
#define VCNL4040_DRIVER_H

#include "i2cdev.h"

#define VCNL4040_I2C_ADDRESS        (0x60)

#define VCNL4040_WR_REG_ALS_CONF    (0x00)
#define VCNL4040_WR_REG_ALS_THDH    (0x01)
#define VCNL4040_WR_REG_ALS_THDL    (0x02)
#define VCNL4040_WR_REG_PS_CONF     (0x03)
#define VCNL4040_WR_REG_PS_MS       (0x04)
#define VCNL4040_WR_REG_PS_CANC     (0x05)
#define VCNL4040_WR_REG_PS_THDL     (0x06)
#define VCNL4040_WR_REG_PS_THDH     (0x07)
#define VCNL4040_RD_REG_PS_DATA     (0x08)
#define VCNL4040_RD_REG_ALS_DATA    (0x09)
#define VCNL4040_RD_REG_WHITE_DATA  (0x0A)
#define VCNL4040_RD_REG_INT_FLAG    (0x0B)
#define VCNL4040_RD_REG_DEVICE      (0x0C)

#define VCNL4040_ALS_IT_80MS        (0b00000000)
#define VCNL4040_ALS_IT_160MS       (0b01000000)
#define VCNL4040_ALS_IT_320MS       (0b10000000)
#define VCNL4040_ALS_IT_640MS       (0b11000000)


/*
 * ALS interrupt persistence setting
 * bits: 3-2
 */
#define VCNL4040_ALS_PERS_1         (0b00000000)
#define VCNL4040_ALS_PERS_2         (0b00000100)
#define VCNL4040_ALS_PERS_4         (0b00001000)
#define VCNL4040_ALS_PERS_8         (0b00001100)

#define VCNL4040_ALS_INT_ENABLE     (0b00000010)
#define VCNL4040_ALS_INT_DISABLE    (0b00000000)

#define VCNL4040_ALS_SD_POWER_ON    (0b00000000)
#define VCNL4040_ALS_SD_POWER_OFF   (0b00000001)

/*
 * PS IRED on / off duty ratio setting
 * bits: 7-6
 */
#define VCNL4040_PS_DUTY_1_40       (0b00000000)
#define VCNL4040_PS_DUTY_1_80       (0b01000000)
#define VCNL4040_PS_DUTY_1_160      (0b10000000)
#define VCNL4040_PS_DUTY_1_320      (0b11000000)

/*
 * PS interrupt persistence setting
 * bits: 5-4
 */
#define VCNL4040_PS_PERS_1          (0b00000000)
#define VCNL4040_PS_PERS_2          (0b00010000)
#define VCNL4040_PS_PERS_3          (0b00100000)
#define VCNL4040_PS_PERS_4          (0b00110000)

/*
 *  PS integration time setting
 * bits: 3-1
 */
#define VCNL4040_PS_IT_1T           (0b00000000)
#define VCNL4040_PS_IT_1_5T         (0b00000010)
#define VCNL4040_PS_IT_2T           (0b00000100)
#define VCNL4040_PS_IT_2_5T         (0b00000110)
#define VCNL4040_PS_IT_3T           (0b00001000)
#define VCNL4040_PS_IT_3_5T         (0b00001010)
#define VCNL4040_PS_IT_4T           (0b00000110)
#define VCNL4040_PS_IT_8T           (0b00001110)

/*
 * PS power
 * bits: 0
 */
#define VCNL4040_PS_SD_OFF          (0b00000001)
#define VCNL4040_PS_SD_ON           (0b00000000)

/*
 * PS output size
 * bits: 0
 */
#define VCNL4040_PS_HD_12BIT        (0b00000000)
#define VCNL4040_PS_HD_16BIT        (0b00001000)

/*
 * PS interrupt
 * bits: 1-0
 */
#define VCNL4040_PS_INT_DISABLE             (0b00000000)
#define VCNL4040_PS_INT_WHEN_CLOSE          (0b00000001)
#define VCNL4040_PS_INT_WHEN_AWAY           (0b00000010)
#define VCNL4040_PS_INT_WHEN_CLOSE_OR_AWAY  (0b00000011)

/*
 * Proximity multi pulse numbers
 * bits: 6-5
 */
#define VCNL4040_PS_MPS_1           (0b00000000)
#define VCNL4040_PS_MPS_2           (0b00100000)
#define VCNL4040_PS_MPS_4           (0b01000000)
#define VCNL4040_PS_MPS_8           (0b01100000)

/*
 * PS smart persistence
 * bits: 4
 */
#define VCNL4040_PS_SMART_PERS_DISABLE      0b00000000
#define VCNL4040_PS_SMART_PERS_ENABLE       0b00010000

/*
 * PS active force mode
 * bits: 3
 */
#define VCNL4040_PS_NORMAL_MODE     (0b00000000)
#define VCNL4040_PS_FORCE_MODE      (0b00001000)

/*
 * PS active force mode trigger
 * bits: 2
 */
#define VCNL4040_PS_TRIG_INACTIVE   (0b00000000)
#define VCNL4040_PS_TRIG_ACTIVE     (0b00000100)

/*
 * PS sunlight cancel setting
 * bits: 0
 */
#define VCNL4040_PS_SC_DISABLE      (0b00000000)
#define VCNL4040_PS_SC_ENABLE       (0b00000001)

/*
 * White channel
 * bits: 7
 */
#define VCNL4040_WHITE_ENABLED      (0b00000000)
#define VCNL4040_WHITE_DISABLED     (0b10000000)

/*
 * 0 = proximity normal operation with interrupt function
 * 1 = proximity detection logic output mode enable
 * bits: 6
 */
#define VCNL4040_PS_MS_NORMAL       (0b00000000)
#define VCNL4040_PS_MS_OUTPUT_MODE  (0b01000000)

/*
 * LED current selection setting (mA)
 * bits: 2-0
 */
#define VCNL4040_LED_I_50           (0b00000000)
#define VCNL4040_LED_I_75           (0b00000001)
#define VCNL4040_LED_I_100          (0b00000010)
#define VCNL4040_LED_I_120          (0b00000011)
#define VCNL4040_LED_I_140          (0b00000100)
#define VCNL4040_LED_I_160          (0b00000101)
#define VCNL4040_LED_I_180          (0b00000110)
#define VCNL4040_LED_I_200          (0b00000111)

typedef struct {
    // ALS
    uint8_t ALS_CONF;
    uint16_t ALS_THDH;
    uint16_t ALS_THDL;

    // PS
    uint8_t PS_CONF1;
    uint8_t PS_CONF2;
    uint8_t PS_CONF3;
    uint8_t PS_MS;
    uint16_t PS_CANC;
    uint16_t PS_THDL;
    uint16_t PS_THDH;
} vcnl4040_params_t;

typedef struct {
    uint16_t id;
    i2c_dev_t i2c_dev;

    uint16_t ps;
    uint16_t als;
    uint16_t white;
} vcnl4040_t;

/**
 * @brief Initialize default parameters
 *
 * Default configuration:
 *
 *  - mode: NORMAL
 *  - filter: OFF
 *  - oversampling: x4
 *  - standby time: 250ms
 *
 * @param[out] params Default parameters
 * @return `ESP_OK` on success
 */
esp_err_t vcnl4040_init_default_params(vcnl4040_params_t *params);

/**
 * @brief Initialize device descriptor
 *
 * @param dev Device descriptor
 * @param addr VCNL4040 address
 * @param port I2C port number
 * @param sda_gpio GPIO pin for SDA
 * @param scl_gpio GPIO pin for SCL
 * @return `ESP_OK` on success
 */
esp_err_t vcnl4040_init_desc(vcnl4040_t *dev, uint8_t addr, i2c_port_t port, gpio_num_t sda_gpio, gpio_num_t scl_gpio);

/**
 * @brief Function to initialize the VCNL4040 driver
 *
 * @param dev
 * @param params
 * @return
 */
esp_err_t vcnl4040_init(vcnl4040_t *dev, vcnl4040_params_t *params);

esp_err_t vcnl4040_read_proximity(vcnl4040_t *dev, uint16_t *data);
esp_err_t vcnl4040_read_lux(vcnl4040_t *dev, uint16_t *data);
esp_err_t vcnl4040_read_white(vcnl4040_t *dev, uint16_t *data);


#endif // VCNL4040_DRIVER_H
