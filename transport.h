#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <stdint.h>
#include <stdbool.h>
#include "project_defs.h"

/* ===================== FLAGS ===================== */

#define FLAG_DATA   0x01
#define FLAG_ACK    0x02
#define FLAG_HELLO  0x04


/* ===================== API ===================== */

void transport_init(void);

/* Send DATA packet (with retry + ACK) */
int transport_send(SecurePacket_t *packet);

/* Receive handling */
int transport_packet_received(void);
int transport_receive(SecurePacket_t *packet);

/* Handshake (call once at startup) */
int transport_handshake(void);

/* RX processing (called in interrupt or loop if needed) */
void transport_process_rx(void);

/* Sync finder */
int transport_find_sync(uint8_t *stream, uint16_t len);

#endif
