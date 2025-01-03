#ifndef SERIAL_BLE_H_
#define SERIAL_BLE_H_

#include <Arduino.h>
#include <console.h>
#include "NimBLEDevice.h"

#define BLE_T           "BLE"   // Tag for log

#define BLE_CALLBACK    NimBLECharacteristicCallbacks

// List of Service and Characteristic UUIDs
/*
#define SERVICE_UUID	"fcd44c96-97d3-11ed-a8fc-0242ac120002"
#define RX_DATA_UUID	"fcd4533a-97d3-11ed-a8fc-0242ac120002"
#define TX_DATA_UUID	"fcd454c0-97d3-11ed-a8fc-0242ac120002"
*/
#define SERVICE_UUID	"6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define RX_DATA_UUID	"6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define TX_DATA_UUID	"6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

class bleRxHandler: public NimBLECharacteristicCallbacks {
    public:
        void onWrite(NimBLECharacteristic *pCharacteristic);
};

String getBtMacAddress(void);
void getBtMacAddress(uint8_t * address);
//void initBLE(const char * name, BLE_CALLBACK* cbData);
void initBLE(const char * name, void(*callback)(const char *data, int data_size));
void deinitBLE(void);
void printBLEinfo(void);
void sendDataNotify(uint8_t * data, uint8_t size);
bool isEnabledBLE(void);

#endif  /* SERIAL_BLE_H_ */