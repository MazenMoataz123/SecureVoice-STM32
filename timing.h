#ifndef TIMING_H
#define TIMING_H

#define SAMPLE_RATE_HZ        8000
#define FRAME_DURATION_MS     1000

#define SAMPLES_PER_FRAME     ((SAMPLE_RATE_HZ * FRAME_DURATION_MS) / 1000)
#define BYTES_PER_FRAME       SAMPLES_PER_FRAME

#define FULL_DUPLEX_ENABLED   1

// Slow frame pacing for 9600 baud Bluetooth testing.
// This is intentionally choppy/clicky, not real-time voice.
#define FRAME_PERIOD_US       (FRAME_DURATION_MS * 1000)

//========TIMING CONTROL============
void time_init(void);

/* called every 1ms from SysTick */
void time_tick(void);

/* check if the next slow test frame is ready */
int time_frame_ready(void);

/* clear the flag after processing */
void time_clear_frame_flag(void);
#endif
