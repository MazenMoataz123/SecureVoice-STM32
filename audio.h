#ifndef AUDIO_H
#define AUDIO_H

#include <stdint.h>
#include "project_defs.h"

/*
 * AUDIO MODULE
 * Responsibility:
 * - Capture raw audio (mic / simulated input)
 * - Compress into 8-bit G.711 A-law
 * - Output fixed-size frame (PAYLOAD_SIZE)
 */

void audio_init(void);      //i personally beleive a ping pong buffer is much safer with timing hazards, but we can also just have a single buffer that is filled by the audio module and read by the main loop when the frame is ready, as long as we are sure that the audio module is filling the buffer in a deterministic way and that the main loop is only reading from it when the frame is ready, we should be fine.   

/*
 * Fills buffer with exactly PAYLOAD_SIZE bytes of encoded audio.
 * Output format: G.711 A-law (8-bit per sample)
 *
 * Must be deterministic and real-time safe (< frame deadline)(this becomes less of an issue if we use a ping pong buffer)
 */
void audio_get_frame(uint8_t *buffer);

void play_audio(uint8_t *payload);
/*
 * Optional debug hook:
 * Returns 1 if audio source is healthy, 0 otherwise
 */
int audio_health_check(void);

#endif