#include <stdint.h>
#include <stddef.h>

#include "audio.h"
#include "project_defs.h"
#include "hal_audio.h"

#define SAMPLE_DELAY_US 125

/*
 * MAX9814 usually has a DC offset around 1.25V.
 * On a 12-bit 3.3V ADC:
 * 1.25 / 3.3 * 4095 ≈ 1550
 *
 * We calibrate it automatically in audio_init().
 */
static int32_t mic_center = 1550;

void audio_init(void)
{
    hal_audio_init();

    /*
     * Simple startup calibration.
     * Keep the room quiet during boot for best result.
     */
    int32_t sum = 0;

    for (uint16_t i = 0; i < 200; i++) {
        sum += hal_audio_read_mic();
        hal_audio_delay_us(500);
    }

    mic_center = sum / 200;
}

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

    for (int exp_mask = 0x4000; (sample & exp_mask) == 0 && exponent > 0; exp_mask >>= 1) {
        exponent--;
    }

    mantissa = (sample >> (exponent + 3)) & 0x0F;

    ulawbyte = ~(sign & ((exponent << 4) | mantissa));

    return ulawbyte;
}

static int16_t adc12_to_pcm16(uint16_t adc_sample)
{
    /*
     * Convert ADC reading from 0..4095 into signed audio around 0.
     */
    int32_t centered = (int32_t)adc_sample - mic_center;

    /*
     * Gain/scaling.
     * If mic is too quiet, change << 5 to << 6.
     * If distorted/clipping, change << 5 to << 4.
     */
    int32_t pcm = centered << 5;

    if (pcm > 32767) {
        pcm = 32767;
    }

    if (pcm < -32768) {
        pcm = -32768;
    }

    return (int16_t)pcm;
}

void audio_get_frame(uint8_t *payload)
{
    if (payload == NULL) {
        return;
    }

    for (uint16_t i = 0; i < PAYLOAD_SIZE; i++) {
        uint16_t adc_sample = hal_audio_read_mic();

        int16_t pcm_sample = adc12_to_pcm16(adc_sample);

        payload[i] = linear_to_ulaw(pcm_sample);

        /*
         * 8 kHz sample rate:
         * 1 / 8000 = 125 microseconds.
         */
        hal_audio_delay_us(SAMPLE_DELAY_US);
    }
}

void play_audio(uint8_t *payload)
{
    /*
     * Speaker playback is not implemented here yet.
     * Your speaker code should decode/output this payload later.
     */
    (void)payload;
}

int audio_health_check(void)
{
    uint16_t sample = hal_audio_read_mic();

    /*
     * If ADC is stuck near 0 or 4095, wiring may be wrong.
     */
    if (sample > 10 && sample < 4085) {
        return 1;
    }

    return 0;
}