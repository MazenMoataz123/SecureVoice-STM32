#include "transport.h"
#include "bluetooth.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>

/* ===================== CONFIG ===================== */

#define RX_BUFFER_SIZE 256
#define ACK_TIMEOUT    100
#define TRANSPORT_SEND_ACKS 0

/* ===================== INTERNAL ===================== */

static uint8_t rx_buffer[RX_BUFFER_SIZE];
static uint16_t rx_index = 0;

static SecurePacket_t rx_packet;
static volatile int packet_ready = 0;

static uint16_t packet_counter = 0;

/* ===================== CRC ===================== */

static uint16_t calc_crc(uint8_t *data, uint16_t len)
{
    uint16_t crc = 0xFFFF;

    for (uint16_t i = 0; i < len; i++) {
        crc ^= data[i];

        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }

    return crc;
}

/* ===================== INIT ===================== */

void transport_init(void)
{
    rx_index = 0;
    packet_ready = 0;
    packet_counter = 0;

    bluetooth_init();
}

/* ===================== TX PREPARE / SEND ===================== */

void transport_prepare_packet(SecurePacket_t *packet)
{
    if (packet == NULL) {
        return;
    }

    packet->sync_byte = SYNC_WORD;
    packet->packet_id = packet_counter++;

    /*
     * New packet starts as normal DATA.
     * encrypt_packet() can later add FLAG_ENCRYPTED.
     */
    packet->flags = FLAG_DATA;

    /*
     * CRC is calculated after encryption.
     */
    packet->crc16 = 0;
}

int transport_finalize_send(SecurePacket_t *packet)
{
    if (packet == NULL) {
        return 0;
    }

    /*
     * CRC is calculated after encryption, over the transmitted data,
     * excluding sync_byte and excluding crc16 itself.
     */
    packet->crc16 = calc_crc(
        (uint8_t *)&packet->packet_id,
        sizeof(SecurePacket_t) - 1 - 2
    );

    return bluetooth_send((uint8_t *)packet, sizeof(SecurePacket_t));
}

/*
 * Backward-compatible wrapper.
 * For normal encrypted audio, app.c should use:
 * prepare -> encrypt -> finalize_send
 */
int transport_send(SecurePacket_t *packet)
{
    transport_prepare_packet(packet);
    return transport_finalize_send(packet);
}

/* ===================== HANDSHAKE ===================== */

int transport_handshake(void)
{
    SecurePacket_t hello;

    memset(&hello, 0, sizeof(hello));

    hello.sync_byte = SYNC_WORD;
    hello.packet_id = 0;
    hello.flags = FLAG_HELLO;

    hello.crc16 = calc_crc(
        (uint8_t *)&hello.packet_id,
        sizeof(SecurePacket_t) - 1 - 2
    );

    bluetooth_send((uint8_t *)&hello, sizeof(SecurePacket_t));

    int timeout = ACK_TIMEOUT;

    while (timeout--) {
        if (packet_ready && (rx_packet.flags & FLAG_ACK)) {
            packet_ready = 0;
            return 1;
        }
    }

    return 0;
}

/* ===================== SYNC FIND ===================== */

int transport_find_sync(uint8_t *stream, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++) {
        if (stream[i] == SYNC_WORD) {
            return i;
        }
    }

    return -1;
}

/* ===================== BYTE PROCESS ===================== */

static void process_byte(uint8_t byte)
{
    rx_buffer[rx_index++] = byte;

    if (rx_index >= RX_BUFFER_SIZE) {
        rx_index = 0;
    }

    int sync_pos = transport_find_sync(rx_buffer, rx_index);

    if (sync_pos < 0) {
        return;
    }

    if ((uint16_t)(rx_index - sync_pos) < sizeof(SecurePacket_t)) {
        return;
    }

    memcpy(&rx_packet, &rx_buffer[sync_pos], sizeof(SecurePacket_t));

    uint16_t crc = calc_crc(
        (uint8_t *)&rx_packet.packet_id,
        sizeof(SecurePacket_t) - 1 - 2
    );

    if (crc == rx_packet.crc16) {
#if TRANSPORT_SEND_ACKS
        /*
         * ACKs are disabled for one-way voice streaming by default.
         * Re-enable only for non-real-time transport testing.
         */
        if (rx_packet.flags & FLAG_DATA) {
            SecurePacket_t ack;

            memset(&ack, 0, sizeof(ack));

            ack.sync_byte = SYNC_WORD;
            ack.packet_id = rx_packet.packet_id;
            ack.flags = FLAG_ACK;

            ack.crc16 = calc_crc(
                (uint8_t *)&ack.packet_id,
                sizeof(SecurePacket_t) - 1 - 2
            );

            bluetooth_send((uint8_t *)&ack, sizeof(SecurePacket_t));
        }
#endif

        packet_ready = 1;
    }

    uint16_t remaining = rx_index - (sync_pos + sizeof(SecurePacket_t));

    memmove(
        rx_buffer,
        &rx_buffer[sync_pos + sizeof(SecurePacket_t)],
        remaining
    );

    rx_index = remaining;
}

/* ===================== RX POLLING ===================== */

void transport_process_rx(void)
{
    while (bluetooth_available()) {
        process_byte(bluetooth_read_byte());
    }
}

/* ===================== PUBLIC RECEIVE API ===================== */

int transport_packet_received(void)
{
    return packet_ready;
}

int transport_receive(SecurePacket_t *packet)
{
    if (packet == NULL) {
        return 0;
    }

    if (!packet_ready) {
        return 0;
    }

    memcpy(packet, &rx_packet, sizeof(SecurePacket_t));

    packet_ready = 0;

    return 1;
}
