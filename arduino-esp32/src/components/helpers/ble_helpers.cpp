#include "ble_helpers.hpp"
#include "HardwareSerial.h"
#include "WString.h"
#include <Arduino.h>
#include <BLE2901.h>
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

BLEServer         *pServer         = NULL;
BLECharacteristic *pCharacteristic = NULL;
BLE2901           *descriptor_2901 = NULL;

bool deviceConnected    = false;
bool oldDeviceConnected = false;
bool clientWrote        = false;

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer *pServer) { deviceConnected = true; };

    void onDisconnect(BLEServer *pServer) { deviceConnected = false; }
};

void setup_ble_notify(const char *service_uuid,
                      const char *characteristic_uuid) {

    // Create the BLE Device
    BLEDevice::init("ESP32");

    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    // Create the BLE Service
    BLEService *pService = pServer->createService(service_uuid);

    // Create a BLE Characteristic
    pCharacteristic = pService->createCharacteristic(
        characteristic_uuid, BLECharacteristic::PROPERTY_READ |
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
    pAdvertising->addServiceUUID(service_uuid);
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(
        0x0); // set value to 0x00 to not advertise this parameter
    BLEDevice::startAdvertising();
}

// Packs u16 integers into little-endian u8 packages
void u16_bytepack(uint8_t cargo[], size_t sizeof_cargo, uint8_t index,
                  uint16_t package) {
    // x2 because its 2 bytes (16-bits) per packages
    index = index * 2;

    if (index + 1 >= sizeof_cargo) {
        Serial.println("Index exceeds size of cargo!");
        abort();
    }

    cargo[index]     = LOWERBYTE(package);
    cargo[index + 1] = UPPERBYTE(package);
}
