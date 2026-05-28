#include <Arduino.h>

extern "C" {
  #include "app.h"
  #include "timing.h"
}

extern "C" void osSystickHandler(void)
{
    time_tick();
}

void setup()
{
    Serial.begin(115200);
    delay(500);

    Serial.println("[BLINK] Booting...");
    app_init();
}

void loop()
{
    app_loop();
}