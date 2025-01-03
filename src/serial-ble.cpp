#include "serial-ble.h"

bleRxHandler::bleRxHandler():NimBLECharacteristicCallbacks(){}

bleRxHandler::bleRxHandler(void (*callback)(const char *data, int data_size)):NimBLECharacteristicCallbacks(){
	setHandler(callback);
}

void bleRxHandler::setHandler(void (*callback)(const char *data, int data_size)) {
	cbOnReceive = callback;
}

void bleRxHandler::onWrite(NimBLECharacteristic *pCharacteristic){

	String data_str = pCharacteristic->getValue();
	uint8_t len = pCharacteristic->getDataLength();

	if (cbOnReceive != nullptr)
		cbOnReceive(data_str.c_str(), len);
	else
		console.error(BLE_T, "No Rx Serial BLE callback was specified");
}

void ServerCallbacks::onConnect(NimBLEServer *pServer, ble_gap_conn_desc *desc){
	String bleRemoteAddress = SerialBle::btMAcAddress_toString(desc->peer_id_addr.val);
	console.success(BLE_T, "BLE client connected, remote address is " + bleRemoteAddress);
	NimBLEDevice::stopAdvertising();

	uint16_t id = pServer->getPeerDevices()[0];
	console.info(BLE_T, "Connection ID = " + String(id));
};

void ServerCallbacks::onDisconnect(NimBLEServer *pServer){
	console.warning(BLE_T, "Client disconnected - Start advertising");
	NimBLEDevice::startAdvertising();
};


SerialBle::SerialBle(NimBLECharacteristicCallbacks *pRxHandler) {
	_pRxHandler = pRxHandler;
}

void SerialBle::init(const char * name, bool name_with_address) {

	console.info(BLE_T, "Start BLE configuration");

	// Reading Bluetooth MAC address ------------------------------------------------------
	getBtMacAddress(_btMacAddr);
	if(name_with_address)
		snprintf(_btNameStr, sizeof(_btNameStr), "%s_%02X%02X%02X", name, _btMacAddr[3], _btMacAddr[4], _btMacAddr[5]);
	else
		strncpy(_btNameStr, name, sizeof(_btNameStr));
	printBLEinfo();

	NimBLEDevice::init(_btNameStr);
	NimBLEDevice::setPower(ESP_PWR_LVL_P9); // +9 dB

	// NimBLEDevice::setSecurityAuth(true, true, true);
	// NimBLEDevice::setSecurityPasskey(123456);
	// NimBLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_ONLY);

	_pServer = NimBLEDevice::createServer();
	_pServer->setCallbacks(new ServerCallbacks());

	_pService = _pServer->createService(BLEUUID(SERVICE_UUID));

	// Create BLE Characteristic for Transmitting Data --------------------------------------------
	_pTxCharacteristic = _pService->createCharacteristic(
		BLEUUID(TX_DATA_UUID),
		NIMBLE_PROPERTY::NOTIFY);

	// Create BLE Characteristic for Receiving Data -----------------------------------------------
	NimBLECharacteristic *pRxCharacteristic = _pService->createCharacteristic(
		BLEUUID(RX_DATA_UUID),
		NIMBLE_PROPERTY::WRITE);

	// pRxCharacteristic->setCallbacks(cbData);
	//pRxCharacteristic->setCallbacks(new bleRxHandler());
	pRxCharacteristic->setCallbacks(_pRxHandler);
	console.log(BLE_T, "Created characteristic for Data");

	// Start service
	_pService->start();

	NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
	pAdvertising->addServiceUUID(_pService->getUUID());
	pAdvertising->setScanResponse(true);
	pAdvertising->start();

	console.success(BLE_T, "BLE configuration completed");

	_ble_enabled = true;
}

void SerialBle::getBtMacAddress(uint8_t *address) {
	esp_read_mac(address, ESP_MAC_BT);
}

String SerialBle::btMAcAddress_toString(const uint8_t *address) {
	char btMacAddrStr[18];
	snprintf(btMacAddrStr, 18, "%02X:%02X:%02X:%02X:%02X:%02X", address[0], address[1], address[2], address[3], address[4], address[5]);
	return String(btMacAddrStr);
}

String genDeviceName(const uint8_t *address, const char* name){
	char btNameStr[14] = "";
	snprintf(btNameStr, sizeof(btNameStr), "%s_%02X%02X%02X", name, address[3], address[4], address[5]);
	return String(btNameStr);
}

void SerialBle::printBLEinfo(void) {
	console.info(BLE_T, "Device Bluetooth MAC Address: " + btMAcAddress_toString(_btMacAddr));
	console.info(BLE_T, "Device Bluetooth name: " + String(_btNameStr));
}

void SerialBle::deinitBLE(void) {
	NimBLEDevice::deinit(true);
	_ble_enabled = false;
	console.success(BLE_T, "BLE was stopped");
}

void SerialBle::sendData(uint8_t *data, int size) {
	char buffer[BLE_TX_MAX_SIZE] = "";
	char tmp[6];
	String str;

	if(_ble_enabled) {
		if(size < 0)
			console.warning(BLE_T, "NOTIFY: buffer is empty");
		else if(size > BLE_TX_MAX_SIZE)
			console.warning(BLE_T, "NOTIFY: buffer exceed maximum length");
		else {
			// Print values in hexadecimal
			for(uint8_t i = 0; i < size; i++) {
				buffer[i] = *(data + i);
				sprintf(tmp, "0x%.2X ", buffer[i]);
				str += String(tmp);
			}
			console.success(BLE_T, "NOTIFY (size = " + String(size) + "): \"" + String(buffer) + "\" [" + str + "]");
			_pTxCharacteristic->setValue(data, size);
			_pTxCharacteristic->notify();   
	    }
	}
	else
		console.warning(BLE_T, "NOTIFY: impossible to send data due BLE disabling");
}

bool SerialBle::isEnabled(void) {
	return _ble_enabled;
}