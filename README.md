# Capacitive Soil Moisture Sensor thru BLE
- Source code for wireless notification from soil moisture sensor module on ESP32 to a mobile device with BLE capabilities.
- For ESP-IDF using Arduino as a component.

## How To Use
- Make a new project with `esp-idf`
- Add `espressif/arduino-esp32` as a dependency
- Build; then change `CONFIG_FREERTOS_HZ` to `1000` or whatever the previous error from build says
- In the `menuconfig`, turn on `Autostart Arduino setup and loop on boot`
    - Also turn on `Component config/Bluetooth`
- Replace `<project>/main/<project>.c` with this repos's src file
- Make sure that the extensions for the main src file is `.cpp`, since we are using Arduino as a component
    - Also make sure to adjust the src file in `<project>/main/CMakeLists.txt`
- Should be ready to flash

> [!NOTE]
> **Latest ESP-IDF version might be unable to build this as the latest espressif/arduino-esp32 repo depends on <5.3.something.**
> Might not work on all ESP32 models (this specific repo was made for NodeMCU ESP-32S)

Suggested resources:
[Arduino as an ESP-IDF component](https://docs.espressif.com/projects/arduino-esp32/en/latest/esp-idf_component.html) 
[espressif/arduino-esp BLE library](https://github.com/espressif/arduino-esp32/blob/master/libraries/BLE) 
[ESP32 - Capacitive Soil Moisture Sensor Tutorial](https://esp32io.com/tutorials/esp32-soil-moisture-sensor) 
