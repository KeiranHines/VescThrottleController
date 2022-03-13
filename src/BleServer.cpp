#include "BleServer.h"
#include <sstream>
#include "utils.h"
#include "esp_bt_main.h"

#define FULL_PACKET 512

const int BLE_PACKET_SIZE = 128;
NimBLEServer *pServer = NULL;
NimBLEService *pServiceVesc = NULL;
NimBLECharacteristic *pCharacteristicVescTx = NULL;
NimBLECharacteristic *pCharacteristicVescRx = NULL;

bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;
std::string bufferString;
int bleLoop = 0;

BleServer::BleServer() {}

bool updateFlag = false;
uint32_t frameNumber = 0;

// NimBLEServerCallbacks::onConnect
inline void BleServer::onConnect(NimBLEServer *pServer, ble_gap_conn_desc *desc)
{
    deviceConnected = true;
    NimBLEDevice::startAdvertising();
#ifdef DEBUG
    char buf[128];
    snprintf(buf, 128, "Client connected: %s", NimBLEAddress(desc->peer_ota_addr).toString().c_str());
    Serial.println(buf);
#endif
};

// NimBLEServerCallbacks::onDisconnect
inline void BleServer::onDisconnect(NimBLEServer *pServer)
{
#ifdef DEBUG
    Serial.println(clientDisconnected);
#endif
    deviceConnected = false;
    NimBLEDevice::startAdvertising();
}

void BleServer::init(Stream *vesc)
{
    vescUart = vesc;

    NimBLEDevice::init(VESC_NAME);
    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(this);
    auto pSecurity = new NimBLESecurity();
    pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);

    pServiceVesc = pServer->createService(VESC_SERVICE_UUID);
    pCharacteristicVescTx = pServiceVesc->createCharacteristic(VESC_CHARACTERISTIC_UUID_TX, NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::READ);
    pCharacteristicVescTx->setCallbacks(this);
    pCharacteristicVescRx = pServiceVesc->createCharacteristic(VESC_CHARACTERISTIC_UUID_RX, NIMBLE_PROPERTY::WRITE);
    pCharacteristicVescRx->setCallbacks(this);

    // Start the VESC service
    pServiceVesc->start();

    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(VESC_SERVICE_UUID);
    pAdvertising->setAppearance(0x00);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x0); // Disable advertising.
    pAdvertising->start();
}

void BleServer::loop()
{
    if (vescUart->available())
    {
        int oneByte;
        while (vescUart->available())
        {
            oneByte = vescUart->read();
            bufferString.push_back(oneByte);
        }
        if (deviceConnected)
        {
            while (bufferString.length() > 0)
            {
                if (bufferString.length() > BLE_PACKET_SIZE)
                {
                    pCharacteristicVescTx->setValue(bufferString.substr(0, BLE_PACKET_SIZE));
                    bufferString = bufferString.substr(BLE_PACKET_SIZE);
                }
                else
                {
                    pCharacteristicVescTx->setValue(bufferString);
                    bufferString.clear();
                }
                pCharacteristicVescTx->notify();
                delay(10); // Rate limit bluetooth stack.
            }
        }
    }

    // disconnecting
    if (!deviceConnected && oldDeviceConnected)
    {
        delay(500);                  // wait for time to cleanup.
        pServer->startAdvertising(); // restart advertising
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected)
    {
        oldDeviceConnected = deviceConnected;
    }
}

// NimBLECharacteristicCallbacks::onWrite
void BleServer::onWrite(BLECharacteristic *pCharacteristic)
{
    std::string rxValue = pCharacteristic->getValue();
    if (rxValue.length() > 0)
    {
        #ifdef DEBUG
        printSerial("On Write: ", rxValue, rxValue.length());
        #endif
        for (int i = 0; i < rxValue.length(); i++)
        {
            vescUart->write(rxValue[i]);
        }
    }
}
// NimBLECharacteristicCallbacks::onSubscribe
void BleServer::onSubscribe(NimBLECharacteristic *pCharacteristic, ble_gap_conn_desc *desc, uint16_t subValue)
{
#ifdef DEBUG
    char buf[256];
    snprintf(buf, 256, "Client ID: %d, Address: %s, Subvalue %d, Characteristics %s ",
             desc->conn_handle, NimBLEAddress(desc->peer_ota_addr).toString().c_str(), subValue,
             pCharacteristic->getUUID().toString().c_str());
    Serial.println(buf);
#endif
}

// NimBLECharacteristicCallbacks::onSubscribe
void BleServer::onStatus(NimBLECharacteristic *pCharacteristic, Status status, int code)
{
#ifdef DEBUG
    char buf[256];
    snprintf(buf, 256, "Notification/Indication status code: %d, return code: %d", status, code);
#endif
}