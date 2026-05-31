#ifndef AUDIO_H
#define AUDIO_H

#include <stdint.h>
#include "project_defs.h"

/*
 * AUDIO MODULE
 *
 * Responsibility:
 * - Capture microphone audio
 * - Convert ADC samples to signed PCM
 * - Encode PCM into 8-bit G.711 μ-law
 * - Fill TX payload with PAYLOAD_SIZE bytes
 * - Decode received μ-law payload
 * - Output basic PWM speaker signal
 */

void audio_init(void);

void audio_get_frame(uint8_t *buffer);

void play_audio(uint8_t *payload);

int audio_health_check(void);

#endif