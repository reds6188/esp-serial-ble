#ifndef SERIAL_BLE_H_
#define SERIAL_BLE_H_

#include <Arduino.h>
#include <console.h>
#include "NimBLEDevice.h"

#define BLE_T "BLE" // Tag for log

#define BLE_CALLBACK NimBLECharacteristicCallbacks

#define BLE_TX_MAX_SIZE     320

// List of Service and Characteristic UUIDs
#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define RX_DATA_UUID "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define TX_DATA_UUID "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

class SerialBle {
    private:
        uint8_t _btMacAddr[6] = {0};
        char _btNameStr[14] = "";
        bool _ble_enabled = false;
        NimBLECharacteristicCallbacks *_pRxHandler;
        NimBLEServer *_pServer;
        NimBLECharacteristic *_pTxCharacteristic;
        NimBLEService *_pService;
        String genDeviceName(uint8_t *address);
    public:
        SerialBle(NimBLECharacteristicCallbacks *_pRxHandler);
        void getBtMacAddress(uint8_t *address);
        static String btMAcAddress_toString(const uint8_t *address);
        void printBLEinfo(void);
        void init(const char * name, bool name_with_address = false);
        void deinitBLE(void);
        void sendData(uint8_t *data, int size);
        bool isEnabled(void);
};

class bleRxHandler : public NimBLECharacteristicCallbacks {
private:
    void (*cbOnReceive)(const char *data, int data_size);
public:
    bleRxHandler();
    bleRxHandler(void (*callback)(const char *data, int data_size));
    void setHandler(void (*callback)(const char *data, int data_size));
    void onWrite(NimBLECharacteristic *pCharacteristic);
};

class ServerCallbacks: public NimBLEServerCallbacks {
public:
	void onConnect(NimBLEServer *pServer, ble_gap_conn_desc *desc);
	void onDisconnect(NimBLEServer *pServer);
};

#endif /* SERIAL_BLE_H_ */