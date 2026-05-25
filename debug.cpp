#include <Arduino.h>
#include "debug.h"

// Serial2 is used for Bluetooth (PA2=TX, PA3=RX)
// Serial  is used for USB debug monitor (your PC)
#define DEBUG_SERIAL  Serial
#define BT_SERIAL     Serial2

void debug_init(void)
{
    DEBUG_SERIAL.begin(115200);
    while (!DEBUG_SERIAL);   // wait for USB serial to connect
    DEBUG_SERIAL.println("[DEBUG] Serial monitor ready");
}

void debug_print(const char *msg)
{
    DEBUG_SERIAL.println(msg);
}

void debug_print_int(const char *label, int value)
{
    DEBUG_SERIAL.print(label);
    DEBUG_SERIAL.print(": ");
    DEBUG_SERIAL.println(value);
}

void debug_print_bytes(const char *label, uint8_t *data, uint16_t len)
{
    DEBUG_SERIAL.print(label);
    DEBUG_SERIAL.print(": ");
    for (uint16_t i = 0; i < len; i++) {
        if (data[i] < 0x10) DEBUG_SERIAL.print("0");
        DEBUG_SERIAL.print(data[i], HEX);
        DEBUG_SERIAL.print(" ");
    }
    DEBUG_SERIAL.println();
}

// --- Task markers (now print instead of blink) ---

void debug_audio_on()    { DEBUG_SERIAL.println("[AUDIO]   >>> START"); }
void debug_audio_off()   { DEBUG_SERIAL.println("[AUDIO]   <<< END");   }

void debug_encrypt_on()  { DEBUG_SERIAL.println("[ENCRYPT] >>> START"); }
void debug_encrypt_off() { DEBUG_SERIAL.println("[ENCRYPT] <<< END");   }

void debug_tx_on()       { DEBUG_SERIAL.println("[TX]      >>> START"); }
void debug_tx_off()      { DEBUG_SERIAL.println("[TX]      <<< END");   }

void debug_tx_success()  { DEBUG_SERIAL.println("[TX]      ✓ ACK RECEIVED — transport alive"); }
void debug_tx_fail()     { DEBUG_SERIAL.println("[TX]      ✗ NO ACK    — transport stubbed or peer missing"); }