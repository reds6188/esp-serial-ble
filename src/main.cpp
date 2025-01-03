#include <Arduino.h>
#include "../include/serial-ble.h"

#define MAIN_T	"MAIN"

#define BLE_RESTART "restart"
#define BLE_HELP    "help"
#define BLE_LED     "led"

bleRxHandler rxHandler;
SerialBle ble(&rxHandler);
bool oneShot = false;

void ble_callback(const char *data, int data_size) {
    String res;
	if(data_size < 96) {
		if(!strncmp(data, BLE_HELP, sizeof(BLE_HELP)-1)) {
			String info_str[9];
			info_str[0] = "=========== COMMAND LIST ===========";
			info_str[1] = "'info' ------> Get device info";
			info_str[2] = "'pwd=xyz' ---> Set Wi-Fi password with string \"xyz\"";
			info_str[3] = "'reset' -----> Reset Wi-Fi SSID and password";
			info_str[4] = "'restart' ---> Reset device";
			info_str[5] = "'save' ------> Save Wi-Fi SSID and password";
			info_str[6] = "'scan' ------> Scan Wi-Fi networks";
			info_str[7] = "'set=n' -----> Set network 'n' of the scan list";
			info_str[8] = "'ssid=xyz' --> Set Wi-Fi SSID with string \"xyz\"";
			for(uint8_t i=0 ; i<9 ; i++) {
				info_str[i] += "\r\n";
				ble.sendData((uint8_t *)info_str[i].c_str(), info_str[i].length());
			}
		}
		else if(!strncmp(data, BLE_RESTART, sizeof(BLE_RESTART)-1)) {
			res = "OK >>> Restarting device, please reconnect after few seconds\r\n";
			ble.sendData((uint8_t *)res.c_str(), res.length());
			ESP.restart();
		}
		else {
			console.warning(MAIN_T, "Unknown string");
			res = "ERR >>> Invalid command\r\n";
			ble.sendData((uint8_t *)res.c_str(), res.length());
		}
	}
	else {
		console.error(MAIN_T, "Buffer exceed length");
		res = "ERR >>> Message is too long\r\n";
		ble.sendData((uint8_t *)res.c_str(), res.length());
	}
}

void ble_callback2(const char *data, int data_size) {
    String res;
	if(data_size < 96) {
		if(!strncmp(data, BLE_HELP, sizeof(BLE_HELP)-1)) {
			String info_str[9];
			info_str[0] = "=========== COMMAND LIST ===========";
			info_str[1] = "'STOCAZZO";
			info_str[2] = "'pwd=xyz' ---> Set Wi-Fi password with string \"xyz\"";
			info_str[3] = "'reset' -----> Reset Wi-Fi SSID and password";
			info_str[4] = "'restart' ---> Reset device";
			info_str[5] = "'save' ------> Save Wi-Fi SSID and password";
			info_str[6] = "'scan' ------> Scan Wi-Fi networks";
			info_str[7] = "'set=n' -----> Set network 'n' of the scan list";
			info_str[8] = "'ssid=xyz' --> Set Wi-Fi SSID with string \"xyz\"";
			for(uint8_t i=0 ; i<9 ; i++) {
				info_str[i] += "\r\n";
				ble.sendData((uint8_t *)info_str[i].c_str(), info_str[i].length());
			}
		}
		else if(!strncmp(data, BLE_RESTART, sizeof(BLE_RESTART)-1)) {
			res = "OK >>> Restarting device, please reconnect after few seconds\r\n";
			ble.sendData((uint8_t *)res.c_str(), res.length());
			ESP.restart();
		}
		else {
			console.warning(MAIN_T, "Unknown string");
			res = "ERR >>> Invalid command\r\n";
			ble.sendData((uint8_t *)res.c_str(), res.length());
		}
	}
	else {
		console.error(MAIN_T, "Buffer exceed length");
		res = "ERR >>> Message is too long\r\n";
		ble.sendData((uint8_t *)res.c_str(), res.length());
	}
}

void setup () {
    rxHandler.setHandler(ble_callback);
    ble.init("prova");

}

void loop() {
    delay(30000);
    if (!oneShot){
        oneShot = true;
        rxHandler.setHandler(ble_callback2);
    }
}