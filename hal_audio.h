#ifndef HAL_AUDIO_H
#define HAL_AUDIO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void hal_audio_init(void);

uint16_t hal_audio_read_mic(void);

void hal_audio_delay_us(uint32_t us);

#ifdef __cplusplus
}
#endif

#endif