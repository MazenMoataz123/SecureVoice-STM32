#ifndef TIMING_H
#define TIMING_H

#include "system_config.h"

#define SAMPLE_RATE_HZ        SAMPLE_RATE

#define SAMPLES_PER_FRAME     ((SAMPLE_RATE_HZ * FRAME_DURATION_MS) / 1000)
#define BYTES_PER_FRAME       SAMPLES_PER_FRAME

#define FULL_DUPLEX_ENABLED   0

// 20 ms frame pacing for one-way real-time voice at 115200 baud.
#define FRAME_PERIOD_US       (FRAME_DURATION_MS * 1000)

//========TIMING CONTROL============
void time_init(void);

/* called every 1ms from SysTick */
void time_tick(void);

/* check if the next audio frame is ready */
int time_frame_ready(void);

/* clear the flag after processing */
void time_clear_frame_flag(void);
#endif
