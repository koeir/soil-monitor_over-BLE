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

#define BAUDRATE  115200

// BLE -----------------------------------
// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "23a6cc2f-19ce-4108-a792-65d17bc0c958"
#define CHARACTERISTIC_UUID "4ba5b87d-bcd6-4e13-beb1-b52c1fcc604f"
// --------------------------------------

// Moisture sensor ----
#define SIG GPIO_NUM_36
uint16_t readings_cache[CACHE_MAX];
uint16_t average = 0;
uint8_t  package[sizeof(uint16_t) * 2];
uint8_t  count = 0;
// --------------------

void setup() {
    Serial.begin(BAUDRATE);

    setup_ble_notify(SERVICE_UUID, CHARACTERISTIC_UUID);

    analogSetAttenuation(ADC_11db); // ~3.3V input
    Serial.println("Waiting a client connection to notify...");

    // Headers for the moisture readings lol
    Serial.println("_________________________________");
    Serial.println("|   average    |      current   |");
    Serial.println("---------------------------------");
}

void loop() {
    count++;

    // Read moisture sensor
    readings_cache[count - 1] = analogRead(
        SIG); // No need to INDEX_WRAP because count would never be 0 here

    // If certain number of readings have been made
    if (count == CACHE_MAX) {

        // Headers for the moisture readings lol
        Serial.print("\033[3A");
        Serial.println("_________________________________");
        Serial.println("|   average    |      current   |");
        Serial.println("---------------------------------");

        // Get average of last 20 reads
        uint32_t sum = 0;
        for (int i = 0; i < count; i++) { sum += readings_cache[i]; }
        average = sum / count;

        // Reset
        count = 0;
    }

    // Put readings in byte array in little-endian
    u16_bytepack(package, sizeof(package), 0, average);
    u16_bytepack(package, sizeof(package), 1,
                 readings_cache[INDEX_WRAP(count)]);

    Serial.printf("|     %i       |     %i      |\n", average,
                  readings_cache[INDEX_WRAP(count)]);
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
