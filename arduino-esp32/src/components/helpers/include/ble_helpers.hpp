#ifndef HELPERS_H
#define HELPERS_H

#include "HardwareSerial.h"
#include <Arduino.h>
#include <BLE2901.h>
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <stdbool.h>

#define CACHE_MAX 10
#define INDEX_WRAP(x)                                                          \
    ((x + CACHE_MAX - 1) %                                                     \
     CACHE_MAX) // makes sure than when the count resets to zero, the reading is
                // still read from the last index
#define LOWERBYTE(x) (x & 0xFF)
#define UPPERBYTE(x) ((x >> 8) & 0xFF)

// BLE -----------------------------------
extern BLEServer         *pServer;
extern BLECharacteristic *pCharacteristic;
extern BLE2901           *descriptor_2901;

extern bool deviceConnected;
extern bool oldDeviceConnected;
extern bool clientWrote;

void setup_ble_notify(const char *service_uuid,
                      const char *characteristic_uuid);
void u16_bytepack(uint8_t cargo[], size_t sizeof_cargo, uint8_t index,
                  uint16_t package);

#endif
