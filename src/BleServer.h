#ifndef __BLE_SERVER_H__
#define __BLE_SERVER_H__

#include <Arduino.h>
#include <NimBLEDevice.h>
#include "config.h"


#define LOG_TAG_BLESERVER "BleServer"

#define VESC_SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define VESC_CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define VESC_CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

class BleServer : public NimBLEServerCallbacks,
                  public BLECharacteristicCallbacks
{
public:
  BleServer();
  void init(Stream *vesc);
  void loop();

  // NimBLEServerCallbacks
  void onConnect(NimBLEServer *pServer, ble_gap_conn_desc *desc);
  void onDisconnect(NimBLEServer *pServer);

  // NimBLECharacteristicCallbacks
  void onWrite(NimBLECharacteristic *pCharacteristic);
  void onSubscribe(NimBLECharacteristic *pCharacteristic, ble_gap_conn_desc *desc, uint16_t subValue);
  void onStatus(NimBLECharacteristic *pCharacteristic, Status status, int code);

private:
  Stream *vescUart;
};

#endif