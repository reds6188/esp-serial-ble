#include "serial-ble.h"

char btNameStr[14];
uint8_t btMacAddr[6];
bool ble_disabled = true;

static NimBLEServer *pServer;
NimBLECharacteristic *pTxCharacteristic;

NimBLEService *pService;

void (*rxSerialBleCb)(const char *data, int data_size);

void bleRxHandler::onWrite(NimBLECharacteristic *pCharacteristic)
{

	String data_str = pCharacteristic->getValue();
	uint8_t len = pCharacteristic->getDataLength();

	if (rxSerialBleCb != nullptr)
		rxSerialBleCb(data_str.c_str(), len);
	else
		console.error(BLE_T, "No Rx Serial BLE callback was specified");
}

class ServerCallbacks : public NimBLEServerCallbacks
{

	void onConnect(NimBLEServer *pServer, ble_gap_conn_desc *desc)
	{
		String bleRemoteAddress = "";
		for (uint8_t i = 0; i < 6; i++)
		{
			char hex_str[3];
			sprintf(hex_str, "%02x", desc->peer_id_addr.val[5 - i]);
			bleRemoteAddress += String(hex_str) + ":";
		}
		bleRemoteAddress[bleRemoteAddress.length() - 1] = '\0'; // Remove last ":"
		console.success(BLE_T, "BLE client connected, remote address is " + bleRemoteAddress);
		NimBLEDevice::stopAdvertising();

		uint16_t id = pServer->getPeerDevices()[0];
		console.info(BLE_T, "Connection ID = " + String(id));
		// NimBLEConnInfo info = pServer->getPeerInfo(0);

		// console.success(BLE_T, "Client connected");
		// Serial.println("Multi-connect support: start advertising");
		// NimBLEDevice::startAdvertising();
	};

	void onDisconnect(NimBLEServer *pServer)
	{
		console.warning(BLE_T, "Client disconnected - Start advertising");
		if (!ble_disabled)
			NimBLEDevice::startAdvertising();
	};
};

void getBtMacAddress(uint8_t *address)
{
	esp_read_mac(address, ESP_MAC_BT);
}

String btMAcAddress_toString(const uint8_t *address)
{
	char btMacAddrStr[18];
	snprintf(btMacAddrStr, 18, "%02X:%02X:%02X:%02X:%02X:%02X", address[0], address[1], address[2], address[3], address[4], address[5]);
	return String(btMacAddrStr);
}

void printBLEinfo(void)
{
	console.info(BLE_T, "Device Bluetooth MAC Address: " + btMAcAddress_toString(btMacAddr));
	console.info(BLE_T, "Device Bluetooth name: " + String(btNameStr));
}

// void initBLE(const char * name, BLE_CALLBACK* cbData)
void initBLE(const char *name, void (*callback)(const char *data, int data_size))
{
	// cbData = rxSerialBleCb;
	rxSerialBleCb = callback;

	console.info(BLE_T, "Start BLE configuration");

	// Reading Bluetooth MAC address ------------------------------------------------------
	getBtMacAddress(btMacAddr);
	snprintf(btNameStr, sizeof(btNameStr), "%s_%02X%02X%02X", name, btMacAddr[3], btMacAddr[4], btMacAddr[5]);
	printBLEinfo();

	NimBLEDevice::init(btNameStr);
	NimBLEDevice::setPower(ESP_PWR_LVL_P9); // +9 dB

	// NimBLEDevice::setSecurityAuth(true, true, true);
	// NimBLEDevice::setSecurityPasskey(123456);
	// NimBLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_ONLY);

	pServer = NimBLEDevice::createServer();
	pServer->setCallbacks(new ServerCallbacks());

	pService = pServer->createService(BLEUUID(SERVICE_UUID));

	// Create BLE Characteristic for Transmitting Data --------------------------------------------
	pTxCharacteristic = pService->createCharacteristic(
		BLEUUID(TX_DATA_UUID),
		NIMBLE_PROPERTY::NOTIFY);

	// Create BLE Characteristic for Receiving Data -----------------------------------------------
	NimBLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
		BLEUUID(RX_DATA_UUID),
		NIMBLE_PROPERTY::WRITE);

	// pRxCharacteristic->setCallbacks(cbData);
	pRxCharacteristic->setCallbacks(new bleRxHandler());
	console.log(BLE_T, "Created characteristic for Data");

	// Start service
	pService->start();

	NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
	pAdvertising->addServiceUUID(pService->getUUID());
	pAdvertising->setScanResponse(true);
	pAdvertising->start();

	console.success(BLE_T, "BLE configuration completed");

	ble_disabled = false;
}

void deinitBLE(void)
{
	NimBLEDevice::deinit(true);
	ble_disabled = true;
	console.success(BLE_T, "BLE was stopped");
}

void sendDataNotify(uint8_t *data, uint8_t size)
{
	// console.success(BLE_T, "NOTIFY: " + str);
	char buffer[320] = "";
	char tmp[6];
	String str;

	if (size < 320)
	{
		for (uint8_t i = 0; i < size; i++)
		{
			buffer[i] = *(data + i);
			sprintf(tmp, "0x%.2X ", buffer[i]);
			str += String(tmp);
		}
		console.success(BLE_T, "NOTIFY (size = " + String(size) + "): \"" + String(buffer) + "\" [" + str + "]");
	}
	else
		console.warning(BLE_T, "NOTIFY: buffer exceed maximum length");

	if (ble_disabled)
		return;

	pTxCharacteristic->setValue(data, size);
	pTxCharacteristic->notify();
}

bool isEnabledBLE(void)
{
	return !ble_disabled;
}