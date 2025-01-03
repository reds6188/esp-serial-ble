#include <Arduino.h>
#include "../include/serial-ble.h"

void ble_callback(const char *data, int data_size) {

}

void setup () {
    initBLE("prova", ble_callback);
}

void loop() {
    delay(100);
}