#include <Arduino.h>
#include "hal_audio.h"

// MAX9814 OUT -> A0 / PA0
#define MIC_PIN PA0

// Temporary speaker output through a 2N2222 transistor driver.
// Do not connect an 8 ohm speaker directly to an STM32 pin.
// A proper audio amplifier is preferred for the final build.
#define SPEAKER_PIN PA6

void hal_audio_init(void)
{
    analogReadResolution(12);   // ADC range: 0..4095
    pinMode(MIC_PIN, INPUT);

    hal_audio_speaker_init();
}

uint16_t hal_audio_read_mic(void)
{
    return (uint16_t)analogRead(MIC_PIN);
}

void hal_audio_delay_us(uint32_t us)
{
    delayMicroseconds(us);
}

void hal_audio_speaker_init(void)
{
    pinMode(SPEAKER_PIN, OUTPUT);

    /*
     * 8-bit PWM output: 0..255
     * 128 is the midpoint / silence-ish value.
     */
    analogWriteResolution(8);
    analogWrite(SPEAKER_PIN, 128);
}

void hal_audio_write_speaker_pwm(uint8_t value)
{
    analogWrite(SPEAKER_PIN, value);
}
