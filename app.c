#include "app.h"
#include "timing.h"
#include "audio.h"
#include "crypto.h"
#include "transport.h"
#include "sys_health.h"
#include "project_defs.h"
#include "debug.h"
#include "bluetooth.h"

SecurePacket_t TX_packet;
SecurePacket_t RX_packet;

static uint32_t frame_count = 0;

void app_init(void)
{
    debug_init();
    debug_print("[APP] Initializing...");

    time_init();
    debug_print("[APP] Timing OK");

    audio_init();
    debug_print("[APP] Audio OK");

    crypto_init();
    debug_print("[APP] Crypto OK");

    transport_init();               // also calls bluetooth_init() inside
    debug_print("[APP] Transport + Bluetooth OK");

    debug_print("[APP] All systems go. Entering main loop.");
    bluetooth_print("[APP] STM32 online and ready\r\n");
}

void app_loop(void)
{
    transport_process_rx();         // always poll BT for incoming bytes

    if(time_frame_ready())
    {
        time_clear_frame_flag();
        frame_count++;

        debug_print_int("[FRAME] count", frame_count);

        // -------- AUDIO --------
        debug_audio_on();
        SysHealth_MarkTaskStart(TASK_AUDIO);

        audio_get_frame(TX_packet.payload);

        SysHealth_MarkTaskEnd(TASK_AUDIO);
        debug_audio_off();

        // print first 8 bytes of audio payload as proof
        debug_print_bytes("[AUDIO] payload[0..7]", TX_packet.payload, 8);

        // -------- ENCRYPT --------
        debug_encrypt_on();
        SysHealth_MarkTaskStart(TASK_CRYPTO);

        encrypt_packet(&TX_packet);

        SysHealth_MarkTaskEnd(TASK_CRYPTO);
        debug_encrypt_off();

        // -------- TX --------
        debug_tx_on();
        SysHealth_MarkTaskStart(TASK_TRANSPORT_TX);

        int tx_result = transport_send(&TX_packet);

        SysHealth_MarkTaskEnd(TASK_TRANSPORT_TX);
        debug_tx_off();
  
        if(tx_result == 1) {
            debug_print("[TX] Packet sent to BT");
        } else {
            debug_print("[TX] BT send failed — check wiring");
        }
      }

    // -------- RX PATH --------
    if(transport_packet_received())
    {
        debug_print("[RX] Packet received");

        SysHealth_MarkTaskStart(TASK_TRANSPORT_RX);
        transport_receive(&RX_packet);
        uint16_t crc = SysHealth_CalculateCRC(RX_packet.payload, PAYLOAD_SIZE);
        SysHealth_MarkTaskEnd(TASK_TRANSPORT_RX);

        debug_print_int("[RX] CRC", crc);
        debug_print_bytes("[RX] payload[0..7]", RX_packet.payload, 8);

        SysHealth_MarkTaskStart(TASK_CRYPTO);
        decrypt_packet(&RX_packet);
        SysHealth_MarkTaskEnd(TASK_CRYPTO);
        debug_print("[DECRYPT] Done");

        SysHealth_MarkTaskStart(TASK_PLAYBACK);
        play_audio(RX_packet.payload);
        SysHealth_MarkTaskEnd(TASK_PLAYBACK);
        debug_print("[PLAYBACK] Done");

        // send confirmation text to BT peer
        bluetooth_print("[RX] Packet received, decrypted, played\r\n");
    }
}