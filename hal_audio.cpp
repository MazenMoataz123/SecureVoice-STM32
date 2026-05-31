#include <Arduino.h>
#include "hal_audio.h"

// MAX9814 OUT -> PA0 / A0
#define MIC_PIN A0

void hal_audio_init(void)
{
    analogReadResolution(12);   // STM32 ADC range: 0 to 4095
    pinMode(MIC_PIN, INPUT);
}

uint16_t hal_audio_read_mic(void)
{
    return (uint16_t)analogRead(MIC_PIN);
}

void hal_audio_delay_us(uint32_t us)
{
    delayMicroseconds(us);
}