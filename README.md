# Capacitive Soil Moisture Sensor thru BLE
- Source code for wireless notification from soil moisture sensor module on ESP32 to a mobile device with BLE capabilities.
- For ESP-IDF using Arduino as a component.

## Hardware
- x1 NodeMCU ESP-32S
- x1 Capacitive Soil Moisture Sensor v2.0
- Some jumper wires

## Software
- ESP-IDF
- MIT App Inventor

## How To Use
1. Set-up BLE server on the ESP-32
2. Attach Soil Moisture Sensor module
    - The `SVP` pin (`GPIO 36`) is used here for the AOUT
3. Power ESP-32 on and connect with the APK.

> [!NOTE]
> **Latest ESP-IDF version might be unable to build this as the latest espressif/arduino-esp32 repo depends on <5.3.something.**
> 
> Might not work on all ESP32 models (this specific repo was made for NodeMCU ESP-32S)

## Suggested resources:

[Arduino as an ESP-IDF component](https://docs.espressif.com/projects/arduino-esp32/en/latest/esp-idf_component.html)

[espressif/arduino-esp/libraries/BLE/examples/Notify](https://github.com/espressif/arduino-esp32/tree/master/libraries/BLE/examples/Notify)

[ESP32 - Capacitive Soil Moisture Sensor Tutorial](https://esp32io.com/tutorials/esp32-soil-moisture-sensor) 

[MIT App Inventor: BLE Extension](https://iot.appinventor.mit.edu/iot/reference/bluetoothle) 
