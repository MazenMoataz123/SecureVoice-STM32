#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void    bluetooth_init(void);
int     bluetooth_send(uint8_t *data, uint16_t len);
int     bluetooth_available(void);
uint8_t bluetooth_read_byte(void);
void    bluetooth_print(const char *msg);   // send human-readable text to peer

#ifdef __cplusplus
}
#endif

#endif