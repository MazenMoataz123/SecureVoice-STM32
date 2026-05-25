#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <stdint.h>
#include "project_defs.h"

/* ===================== FLAGS ===================== */
#define FLAG_DATA      0x01
#define FLAG_ACK       0x02
#define FLAG_HELLO     0x04
#define FLAG_ENCRYPTED 0x08

/* ===================== API ===================== */

void transport_init(void);

/*
 * Step 1:
 * Prepare packet metadata before encryption.
 * This sets sync_byte, packet_id, and flags.
 */
void transport_prepare_packet(SecurePacket_t *packet);

/*
 * Step 2:
 * After encryption, calculate CRC and send the packet.
 */
int transport_finalize_send(SecurePacket_t *packet);

/*
 * Optional wrapper for old code.
 * Not recommended for encrypted TX path anymore.
 */
int transport_send(SecurePacket_t *packet);

/* Receive handling */
int transport_packet_received(void);
int transport_receive(SecurePacket_t *packet);

/* Handshake */
int transport_handshake(void);

/* RX processing */
void transport_process_rx(void);

/* Sync finder */
int transport_find_sync(uint8_t *stream, uint16_t len);

#endif