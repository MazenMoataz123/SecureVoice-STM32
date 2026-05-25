#include <Arduino.h>
#include "bluetooth.h"

// HC-05 wired to UART2:
// PA2 → HC-05 RX
// PA3 ← HC-05 TX
#define BT_SERIAL   Serial2
#define BT_BAUD  9600

void bluetooth_init(void)
{
    BT_SERIAL.begin(BT_BAUD);
    // give HC-05 time to settle
    delay(500);
    bluetooth_print("[BT] Device ready\r\n");
}

int bluetooth_send(uint8_t *data, uint16_t len)
{
    uint16_t written = BT_SERIAL.write(data, len);
    return (written == len) ? 1 : 0;
}

int bluetooth_available(void)
{
    return BT_SERIAL.available();
}

uint8_t bluetooth_read_byte(void)
{
    return (uint8_t)BT_SERIAL.read();
}

void bluetooth_print(const char *msg)
{
    BT_SERIAL.print(msg);
}