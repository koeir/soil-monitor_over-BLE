## How To Use
1. Make a new project with `esp-idf`
2. Add `espressif/arduino-esp32` as a dependency
3. Build; then change `CONFIG_FREERTOS_HZ` to `1000` or whatever the previous error from build says
4. In the `menuconfig`, turn on `Autostart Arduino setup and loop on boot`
    - Also turn on `Component config/Bluetooth`
5. Replace `<project>/main/<project>.c` with this repos's src file
6. Make sure that the extensions for the main src file is `.cpp`, since we are using Arduino as a component
    - Also make sure to adjust the src file in `<project>/main/CMakeLists.txt`
7. Should be ready to flash

> [!NOTE]
> **Latest ESP-IDF version might be unable to build this as the latest espressif/arduino-esp32 repo depends on <5.3.something.**
> 
> Might not work on all ESP32 models (this specific source code was made for NodeMCU ESP-32S)
