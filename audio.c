#include <stdint.h>
#include "audio.h"
#include "project_defs.h"

#define AUDIO_PAYLOAD_SIZE 160


void audio_init(void) {

    
    // Placeholder for hardware setup
}

uint8_t linear_to_ulaw(int16_t sample)
{
    const int16_t BIAS = 0x84; // Bias for small values (so they don't map to zero).
    const int16_t CLIP = 32635; // Maximum linear value.

    uint8_t sign; // Sign mask used to set the μ-law sign bit.
    uint8_t exponent; // Exponent for the logarithmic encoding.(adds coarse detail to the encoding).
    uint8_t mantissa; // Mantissa for the logarithmic encoding.(adds fine detail to the encoding).
    /* exponent + mantissa = approximate original value */
    uint8_t ulawbyte; // The resulting μ-law byte after compression.

    if (sample < 0)
    {
        sample = -sample;
        sign = 0x7F;
    }
    else
    {
        sign = 0xFF;
    }

    if (sample > CLIP)
    {
        sample = CLIP;
    }
    sample = sample + BIAS;

    exponent = 7;

for (int exp_mask = 0x4000; (sample & exp_mask) == 0 && exponent > 0; exp_mask >>= 1)
{
    exponent--;
}

mantissa = (sample >> (exponent + 3)) & 0x0F;

ulawbyte = ~(sign & ((exponent << 4) | mantissa)); // Combine the sign, exponent, and mantissa into a single byte and invert it for μ-law encoding.

    return ulawbyte;
}

void audio_get_frame(uint8_t *payload)
{
    static int16_t fake_sample = 0;

    for (int i=0;i<AUDIO_PAYLOAD_SIZE;i++)
    {
        fake_sample += 500; // Increment the sample value to create a changing waveform.

        if (fake_sample > 30000)
            fake_sample = -30000; // Wrap around to create a continuous waveform.
        
        payload[i] = linear_to_ulaw(fake_sample);
    }
}



void play_audio(uint8_t *payload) {
    // Leave this empty for now
    (void)payload;
}
