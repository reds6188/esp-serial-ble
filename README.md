# esp-serial-ble
Serial over BLE for ESP32

## Authors

Matteo Bet, Daniele Rossi

## API Documentation

### `SerialBle(NimBLECharacteristicCallbacks *pRxHandler)`
Create a SerialBle instance with data handler pointed by `*pRxHandler`.

### `init(const char * name, bool name_with_address)`
Initialize BLE device with `name`. If `name_with_address` is true, will be appended a part of the MAC address at the end of the name.

### `void sendData(uint8_t *data, int size)`
Send `data` array of `size` bytes to the BLE remote device.


## Examples

Initialize the device with name `test` and send the string `OK >>> Valid command` if receives `help` command, elsewhere send the string `NO >>> Not valid command`

```c++
#include <serial-ble.h>

#define HELP    "help"

bleRxHandler rxHandler;
SerialBle ble(&rxHandler);

void ble_callback(const char *data, int data_size) {
    String res;
	if(!strncmp(data, HELP, sizeof(HELP)-1)) {
        res = "OK >>> Valid command\r\n";
		ble.sendData((uint8_t *)res.c_str(), res.length());
    }
    else {
        res = "NO >>> Not valid command\r\n";
		ble.sendData((uint8_t *)res.c_str(), res.length());
    }
}

void setup () {
    rxHandler.setHandler(ble_callback);
    ble.init("test");

}

void loop() {
    delay(1000);
}
```