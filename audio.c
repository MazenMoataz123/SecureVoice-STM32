#include <stdint.h>
#include <stddef.h>

#include "audio.h"
#include "project_defs.h"
#include "hal_audio.h"

/*
 * 8 kHz sample rate:
 * 1 / 8000 = 125 microseconds per sample.
 */
#define SAMPLE_DELAY_US 125
#define PLAYBACK_SAMPLE_DELAY_US 125

/*
 * MAX9814 usually has a DC offset around 1.25V.
 * On a 12-bit 3.3V ADC:
 * 1.25 / 3.3 * 4095 ~= 1550
 *
 * We calibrate this at startup.
 */
static int32_t mic_center = 1550;

void audio_init(void)
{
    hal_audio_init();

    /*
     * Calibrate mic center at startup.
     * Keep the mic quiet during boot for best result.
     */
    int32_t sum = 0;

    for (uint16_t i = 0; i < 200; i++) {
        sum += hal_audio_read_mic();
        hal_audio_delay_us(500);
    }

    mic_center = sum / 200;
}

/*
 * Convert signed 16-bit PCM to G.711 mu-law.
 */
uint8_t linear_to_ulaw(int16_t sample)
{
    const int16_t BIAS = 0x84;
    const int16_t CLIP = 32635;

    uint8_t sign;
    uint8_t exponent;
    uint8_t mantissa;
    uint8_t ulawbyte;

    if (sample < 0) {
        sample = -sample;
        sign = 0x7F;
    } else {
        sign = 0xFF;
    }

    if (sample > CLIP) {
        sample = CLIP;
    }

    sample = sample + BIAS;

    exponent = 7;

    for (int exp_mask = 0x4000;
         (sample & exp_mask) == 0 && exponent > 0;
         exp_mask >>= 1) {
        exponent--;
    }

    mantissa = (sample >> (exponent + 3)) & 0x0F;

    ulawbyte = ~(sign & ((exponent << 4) | mantissa));

    return ulawbyte;
}

/*
 * Convert G.711 mu-law back to signed 16-bit PCM.
 */
static int16_t ulaw_to_linear(uint8_t ulaw_byte)
{
    ulaw_byte = ~ulaw_byte;

    int sign = ulaw_byte & 0x80;
    int exponent = (ulaw_byte >> 4) & 0x07;
    int mantissa = ulaw_byte & 0x0F;

    int sample = ((mantissa << 3) + 0x84) << exponent;
    sample -= 0x84;

    if (sign) {
        sample = -sample;
    }

    if (sample > 32767) {
        sample = 32767;
    }

    if (sample < -32768) {
        sample = -32768;
    }

    return (int16_t)sample;
}

/*
 * Convert 12-bit ADC reading to signed 16-bit PCM.
 */
static int16_t adc12_to_pcm16(uint16_t adc_sample)
{
    int32_t centered = (int32_t)adc_sample - mic_center;

    /*
     * Gain.
     * If too quiet: change << 4 to << 5.
     * If distorted/noisy: change << 4 to << 3.
     */
    int32_t pcm = centered << 4;

    if (pcm > 32767) {
        pcm = 32767;
    }

    if (pcm < -32768) {
        pcm = -32768;
    }

    return (int16_t)pcm;
}

/*
 * Fill TX payload with real microphone audio encoded as G.711 mu-law.
 */
void audio_get_frame(uint8_t *payload)
{
    if (payload == NULL) {
        return;
    }

    for (uint16_t i = 0; i < PAYLOAD_SIZE; i++) {
        uint16_t adc_sample = hal_audio_read_mic();

        int16_t pcm_sample = adc12_to_pcm16(adc_sample);

        payload[i] = linear_to_ulaw(pcm_sample);

        hal_audio_delay_us(SAMPLE_DELAY_US);
    }
}

/*
 * Play received G.711 mu-law audio through PWM speaker output.
 *
 * Temporary speaker setup:
 * PA6 drives a 2N2222 transistor speaker circuit for testing.
 *
 * Do not connect an 8 ohm speaker directly to an STM32 pin.
 * Use a proper audio amplifier for the final build.
 */
void play_audio(uint8_t *payload)
{
    if (payload == NULL) {
        return;
    }

    for (uint16_t i = 0; i < PAYLOAD_SIZE; i++) {
        int16_t pcm = ulaw_to_linear(payload[i]);

        /*
         * Convert signed 16-bit PCM:
         * -32768..32767
         *
         * to unsigned 8-bit PWM:
         * 0..255
         */
        uint8_t pwm_value = (uint8_t)(((int32_t)pcm + 32768) >> 8);

        hal_audio_write_speaker_pwm(pwm_value);

        hal_audio_delay_us(PLAYBACK_SAMPLE_DELAY_US);
    }

    /*
     * Return PWM to midpoint after frame.
     */
    hal_audio_write_speaker_pwm(128);
}

int audio_health_check(void)
{
    uint16_t sample = hal_audio_read_mic();

    if (sample > 10 && sample < 4085) {
        return 1;
    }

    return 0;
}
