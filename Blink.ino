#include <Arduino.h>

extern "C" {
  #include "app.h"
  #include "timing.h"
}

#define BT_SERIAL  Serial2
#define BT_BAUD    9600

// 1ms tick — hooks into STM32 Arduino core
extern "C" void osSystickHandler(void)
{
    time_tick();
}

// ===================== BT LINK TEST =====================
static void bt_link_test(void)
{
    Serial.println("[BLINK] Running BT link test...");
    BT_SERIAL.println("HELLO FROM STM32 - LINK TEST");

    uint32_t start = millis();
    bool reply_received = false;

    // Wait up to 5 seconds for any reply from peer
    while (millis() - start < 5000)
    {
        if (BT_SERIAL.available())
        {
            Serial.print("[BT RX] ");
            while (BT_SERIAL.available())
            {
                char c = BT_SERIAL.read();
                Serial.print(c);
            }
            Serial.println();
            reply_received = true;
            break;
        }
    }

    if (reply_received) {
        Serial.println("[BLINK] BT link OK — proceeding to app");
        BT_SERIAL.println("LINK CONFIRMED - STARTING APP");
    } else {
        Serial.println("[BLINK] BT link FAILED — no reply in 5s, proceeding anyway");
        BT_SERIAL.println("NO PEER REPLY - STARTING APP ANYWAY");
    }
}

// ===================== SETUP =====================
void setup()
{
    Serial.begin(9600);
    BT_SERIAL.begin(BT_BAUD);
    delay(500);

    Serial.println("[BLINK] Booting...");

    bt_link_test();   // test BT link before handing off to app

    app_init();       // full system init
}

// ===================== LOOP =====================
void loop()
{
    app_loop();
}