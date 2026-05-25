#ifndef CRYPTO_H
#define CRYPTO_H

#include <stdint.h>
#include "project_defs.h"   /* SecurePacket_t, PAYLOAD_SIZE */

/*
 * CRYPTO MODULE
 *
 * Responsibility:
 *   - Encrypt / decrypt packet->payload[] ONLY (in-place)
 *   - Set / clear FLAG_ENCRYPTED in packet->flags
 *   - MUST NOT touch sync_byte, packet_id, crc16 — those belong to transport.c
 *
 * Current cipher: XOR-stream (keyed, nonce = packet_id)
 * Upgrade path:   Drop in Ascon-128 inside keystream_generate() — API unchanged.
 */

void crypto_init(void);

/*
 * Encrypts payload in-place.
 * Sets FLAG_ENCRYPTED in packet->flags.
 * Call AFTER transport_send() fills packet_id, BEFORE send_raw().
 */
void encrypt_packet(SecurePacket_t *packet);

/*
 * Decrypts payload in-place.
 * Only acts if FLAG_ENCRYPTED is set. Clears it on success.
 * Call AFTER transport_receive() hands you the raw packet.
 */
void decrypt_packet(SecurePacket_t *packet);

/*
 * Sanity check: returns 1 if data looks non-trivially encrypted.
 * NOT a MAC — used only as a development-time smoke test.
 * Ascon-128 provides real authentication tags natively.
 */
int crypto_validate_encryption(uint8_t *data, uint16_t len);

#endif /* CRYPTO_H */