#ifndef DEBUG_H
#define DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

void debug_init(void);

void debug_audio_on(void);
void debug_audio_off(void);
void debug_encrypt_on(void);
void debug_encrypt_off(void);
void debug_tx_on(void);
void debug_tx_off(void);

void debug_tx_success(void);
void debug_tx_fail(void);

void debug_print(const char *msg);
void debug_print_int(const char *label, int value);
void debug_print_bytes(const char *label, uint8_t *data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif