#include "HardwareSerial.h"
#include "WString.h"
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
#define CACHE_MAX 10
#define INDEX_WRAP(x)                                                          \
    ((x + CACHE_MAX - 1) %                                                     \
     CACHE_MAX) // makes sure than when the count resets to zero, the reading is
                // still read from the last index
#define LOWERBYTE(x) (x & 0xFF)
#define UPPERBYTE(x) ((x >> 8) & 0xFF)

// BLE -----------------------------------
BLEServer         *pServer         = NULL;
BLECharacteristic *pCharacteristic = NULL;
BLE2901           *descriptor_2901 = NULL;

bool deviceConnected    = false;
bool oldDeviceConnected = false;
bool clientWrote        = false;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "23a6cc2f-19ce-4108-a792-65d17bc0c958"
#define CHARACTERISTIC_UUID "4ba5b87d-bcd6-4e13-beb1-b52c1fcc604f"

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer *pServer) { deviceConnected = true; };

    void onDisconnect(BLEServer *pServer) { deviceConnected = false; }
};
// --------------------------------------

// Moisture sensor ----
#define SIG GPIO_NUM_36
uint16_t readings_cache[CACHE_MAX];
uint16_t average = 0;
uint8_t  package[4]; // byte array for output, 4 because its two 16-bit
                     // integers (4 bytes)
uint8_t  count = 0;
// --------------------

void setup() {
    Serial.begin(BAUDRATE);
    analogSetAttenuation(ADC_11db); // ~3.3V input

    // Create the BLE Device
    BLEDevice::init("ESP32");

    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    // Create the BLE Service
    BLEService *pService = pServer->createService(SERVICE_UUID);

    // Create a BLE Characteristic
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ |
                                 BLECharacteristic::PROPERTY_NOTIFY |
                                 BLECharacteristic::PROPERTY_INDICATE);

    // Creates BLE Descriptor 0x2902: Client Characteristic Configuration
    // Descriptor (CCCD) Descriptor 2902 is not required when using NimBLE as it
    // is automatically added based on the characteristic properties
    pCharacteristic->addDescriptor(new BLE2902());
    // Adds also the Characteristic User Description - 0x2901 descriptor
    descriptor_2901 = new BLE2901();
    descriptor_2901->setDescription("Hi Jayden Villanueva");
    descriptor_2901->setAccessPermissions(
        ESP_GATT_PERM_READ); // enforce read only - default is Read|Write
    pCharacteristic->addDescriptor(descriptor_2901);

    // Start the service
    pService->start();

    // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(
        0x0); // set value to 0x00 to not advertise this parameter
    BLEDevice::startAdvertising();
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
    package[0] = LOWERBYTE(average);
    package[1] = UPPERBYTE(average);
    package[2] = LOWERBYTE(readings_cache[INDEX_WRAP(count)]);
    package[3] = UPPERBYTE(readings_cache[INDEX_WRAP(count)]);

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
