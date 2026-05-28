#include <Arduino.h>
#include "bluetooth.h"

// Working Bluetooth UART:
// HC-05 TXD -> PA10
// HC-05 RXD -> PA9
HardwareSerial BT(PA10, PA9);

#define BT_BAUD 9600

void bluetooth_init(void)
{
    BT.begin(BT_BAUD);
    delay(500);
    bluetooth_print("[BT] Device ready\r\n");
}

int bluetooth_send(uint8_t *data, uint16_t len)
{
    if (data == NULL || len == 0) {
        return 0;
    }

    size_t written = BT.write(data, len);

    return (written == len) ? 1 : 0;
}

int bluetooth_available(void)
{
    return BT.available();
}

uint8_t bluetooth_read_byte(void)
{
    return (uint8_t)BT.read();
}

void bluetooth_print(const char *msg)
{
    if (msg == NULL) {
        return;
    }

    BT.print(msg);
}