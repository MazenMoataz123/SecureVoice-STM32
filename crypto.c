/*
 * crypto.c
 *
 * Encrypts / decrypts only the payload[] field of SecurePacket_t.
 * Headers (sync_byte, packet_id, flags, crc16) are NEVER touched here —
 * transport.c owns those.
 *
 * Current cipher: XOR-stream (keyed, deterministic per-packet via packet_id nonce).
 * Drop-in path to Ascon-128: replace keystream_generate() with ascon_128_stream()
 * and call ascon_init() inside crypto_init(). Everything else stays the same.
 *
 * Key management note: hardcoded key is dev/test only.
 * In production: load from a secured flash sector or OTP fuses.
 */

#include "crypto.h"
#include "project_defs.h"   /* SecurePacket_t, PAYLOAD_SIZE */
#include <string.h>
#include <stdint.h>
#include "transport.h"


/* ===================== KEY MATERIAL ===================== */

/*
 * 128-bit symmetric key.
 * Matches Ascon-128 key width so the upgrade is a direct swap.
 * Replace with your actual key before any real deployment.
 */
#define KEY_SIZE_BYTES 16

static const uint8_t STATIC_KEY[KEY_SIZE_BYTES] = {
    0xDE, 0xAD, 0xBE, 0xEF,
    0xCA, 0xFE, 0xBA, 0xBE,
    0x01, 0x23, 0x45, 0x67,
    0x89, 0xAB, 0xCD, 0xEF
};

/* Working copy — crypto_init() loads this so it can be rotated later */
static uint8_t session_key[KEY_SIZE_BYTES];


/* ===================== INTERNAL: KEYSTREAM ===================== */

/*
 * Generates PAYLOAD_SIZE bytes of keystream into `out`.
 *
 * Nonce strategy: packet_id provides per-packet uniqueness.
 * The same payload encrypted twice with the same packet_id yields
 * the same ciphertext — intentional for a deterministic embedded system.
 * Ascon-128 will give proper nonce handling; this is the bridging design.
 *
 * Construction: simple byte-rotating XOR of key ^ nonce bytes.
 * Not cryptographically secure — placeholder for Ascon.
 */
static void keystream_generate(uint8_t *out, uint16_t len, uint16_t packet_id)
{
    uint8_t nonce_lo = (uint8_t)(packet_id & 0xFF);
    uint8_t nonce_hi = (uint8_t)((packet_id >> 8) & 0xFF);

    for (uint16_t i = 0; i < len; i++)
    {
        /*
         * Mix: key byte (rotating) XOR position index XOR nonce bytes.
         * Each byte of keystream is distinct across positions and packets.
         */
        out[i] = session_key[i % KEY_SIZE_BYTES]
                 ^ (uint8_t)i
                 ^ nonce_lo
                 ^ (uint8_t)(nonce_hi + (i >> 3)); /* shift nonce_hi every 8 bytes */
    }
}


/* ===================== API ===================== */

void crypto_init(void)
{
    /*
     * Load session key from static key.
     * TODO (Ascon path): replace with ascon_init(&ctx, STATIC_KEY, nonce)
     *                    and store ctx in a static local.
     */
    memcpy(session_key, STATIC_KEY, KEY_SIZE_BYTES);
}


void encrypt_packet(SecurePacket_t *packet)
{
    if (packet == NULL) return;

    uint8_t keystream[PAYLOAD_SIZE];

    /*
     * Use packet_id as the per-packet nonce.
     * transport.c assigns packet_id before calling us, so it is always valid here.
     */
    keystream_generate(keystream, PAYLOAD_SIZE, packet->packet_id);

    /* XOR payload in-place. Headers and CRC are NOT touched. */
    for (uint16_t i = 0; i < PAYLOAD_SIZE; i++)
    {
        packet->payload[i] ^= keystream[i];
    }

    /*
     * Tag the packet as encrypted via flags.
     * FLAG_ENCRYPTED (0x08) must be defined in transport.h alongside FLAG_DATA/ACK.
     * transport.c reads flags to decide how to handle the payload on RX.
     */
    packet->flags |= FLAG_ENCRYPTED;
}


void decrypt_packet(SecurePacket_t *packet)
{
    if (packet == NULL) return;

    /*
     * Only decrypt if the sender marked it encrypted.
     * Protects against double-decryption or unencrypted control packets.
     */
    if (!(packet->flags & FLAG_ENCRYPTED)) return;

    uint8_t keystream[PAYLOAD_SIZE];

    keystream_generate(keystream, PAYLOAD_SIZE, packet->packet_id);

    /* XOR is its own inverse — same operation as encrypt */
    for (uint16_t i = 0; i < PAYLOAD_SIZE; i++)
    {
        packet->payload[i] ^= keystream[i];
    }

    /* Clear the encrypted flag so higher layers know plaintext is ready */
    packet->flags &= ~FLAG_ENCRYPTED;
}


int crypto_validate_encryption(uint8_t *data, uint16_t len)
{
    if (data == NULL || len == 0) return 0;

    /*
     * Trivial entropy check: a correctly encrypted buffer should not be
     * all-zero or all-identical bytes (that would imply a zero keystream,
     * which means key == 0x00 XOR position — a degenerate case).
     *
     * Not a substitute for a MAC. Ascon-128 provides authentication natively.
     */
    uint8_t first = data[0];
    uint16_t identical_count = 0;

    for (uint16_t i = 0; i < len; i++)
    {
        if (data[i] == first) identical_count++;
    }

    /* Reject if > 90% of bytes are identical */
    if (identical_count > (len * 9 / 10)) return 0;

    return 1;
}