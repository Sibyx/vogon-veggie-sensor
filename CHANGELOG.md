# Changelog

This changelog suppose to follow rules defined in the [changelog.md](https://changelog.md)

## 0.4.0 - 2024-07-04

BLE Advertisement contains always only single measurement to comply with maximum manufacturer payload (31 bytes).

- **Added**: VOG1 protocol implementation
- **Added**: menuconfig configurations

## 0.3.0 - 2024-06-20

- **Added**: SEN0605 support (experimental - not working...)
- **Added**: Analog soil moisture sensor support

## 0.2.0 - 2024-06-18

- **Added**: VCNL4040 support (experimental)
- **Changed**: `VOGON` device name to `VOG1`

## 0.1.0 - 2024-06-10

Initial release with cute structure for broadcasting using simple BLE advertisements.

- **Added**: BPM280 support
- **Added**: BLE Advertisement support
- **Added**: Shared memory section with mutex access
