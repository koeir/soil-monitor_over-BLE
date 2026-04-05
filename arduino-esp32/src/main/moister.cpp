#include "HardwareSerial.h"
#include "ble_helpers.hpp"
#include "esp32-hal-adc.h"
#include "esp32-hal.h"
#include "soc/gpio_num.h"
#include <Arduino.h>
#include <BLE2901.h>
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <cstdint>
#include <ph4502c_sensor.h>

#define BAUDRATE 115200

// BLE -----------------------------------
#define SERVICE_UUID        "23a6cc2f-19ce-4108-a792-65d17bc0c958"
#define CHARACTERISTIC_UUID "4ba5b87d-bcd6-4e13-beb1-b52c1fcc604f"
// --------------------------------------

// Moisture sensor ----
#define MOIST_SIG GPIO_NUM_36
uint16_t readings_cache[CACHE_MAX];
uint16_t average = 0;
uint8_t  package[sizeof(uint16_t) * 2];
uint8_t  count       = 0;
uint8_t  cache_index = 0;
uint32_t sum         = 0;
// --------------------

// pH + temp sensor ----
/* Pinout: https://cdn.awsli.com.br/969/969921/arquivos/ph-sensor-ph-4502c.pdf */
#define TEMPERATURE_PIN 34
#define PH_PIN 35
#define PH_TRIGGER_PIN 14
#define CALIBRATION 14.8f
#define READING_INTERVAL 100
#define READING_COUNT 10
// NOTE: The ESP32 ADC has a 12-bit resolution (while most arduinos have 10-bit)
#define ADC_RESOLUTION 4096.0f

// Create an instance of the Sensor
PH4502C_Sensor pH_sensor_module(
  PH_PIN,
  TEMPERATURE_PIN,
  CALIBRATION,
  READING_INTERVAL,
  READING_COUNT,
  ADC_RESOLUTION
);

int temperature = 0;
int pH_level_single = 0;
int pH_level_average = 0;
// ---------------------

void setup() {
    Serial.begin(BAUDRATE);

    setup_ble_notify("ESP32", SERVICE_UUID, CHARACTERISTIC_UUID);
    pH_sensor_module.init();

    analogSetAttenuation(ADC_11db); // ~3.3V input
    Serial.println("Waiting a client connection to notify...");

    // Headers for the moisture readings lol
    Serial.println(
"_______________________________________________________________\n"
"|        MOIST        |         pH         |       TEMP        |\n"
"---------------------------------------------------------------\n"
"| avg     | current   | avg     | current  |      value        |\n"
"---------------------------------------------------------------");
}

void loop() {
    count++;
    cache_index = INDEX_WRAP(count, CACHE_MAX);

    // Read moisture sensor
    readings_cache[cache_index] = analogRead(MOIST_SIG);

    sum += readings_cache[cache_index];

    // If certain number of readings have been made
    if (count == CACHE_MAX) {
        // Get average of last 20 reads
        average = sum / count;

        // Reset
        count = 0;
        sum   = 0;
    }

    temperature = pH_sensor_module.read_temp();
    pH_level_single = pH_sensor_module.read_ph_level_single();
    pH_level_average = pH_sensor_module.read_ph_level();

    // Put readings in byte array in little-endian
    u16_bytepack(package, sizeof(package), 0, average);
    u16_bytepack(package, sizeof(package), 1, readings_cache[cache_index]);

    Serial.printf(
"| %7d | %9d | %7d | %9d | %15d |\n",
average,
readings_cache[cache_index],
pH_level_average,
pH_level_single,
temperature
);
    Serial.print("\033[1A");

    if (deviceConnected) {
        pCharacteristic->setValue(package, sizeof(package));
        pCharacteristic->notify();
    }

    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }

    delay(500);
}
