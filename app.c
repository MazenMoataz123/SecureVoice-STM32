#include "app.h"
#include "timing.h"
#include "audio.h"
#include "crypto.h"
#include "transport.h"
#include "project_defs.h"
#include "debug.h"
#include "system_config.h"

SecurePacket_t TX_packet;
SecurePacket_t RX_packet;

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

#if DEVICE_ROLE == DEVICE_ROLE_TRANSMITTER
    debug_print("[APP] Role: transmitter");
#elif DEVICE_ROLE == DEVICE_ROLE_RECEIVER
    debug_print("[APP] Role: receiver");
#else
    debug_print("[APP] ERROR: invalid DEVICE_ROLE");
#endif

    debug_print("[APP] All systems go. Entering main loop.");
}

void app_loop(void)
{
#if DEVICE_ROLE == DEVICE_ROLE_TRANSMITTER
    if (time_frame_ready()) {
        time_clear_frame_flag();

        audio_get_frame(TX_packet.payload);
        transport_prepare_packet(&TX_packet);
        encrypt_packet(&TX_packet);
        (void)transport_finalize_send(&TX_packet);
    }
#elif DEVICE_ROLE == DEVICE_ROLE_RECEIVER
    transport_process_rx();

    if (transport_packet_received()) {
        if (transport_receive(&RX_packet)) {
            decrypt_packet(&RX_packet);
            play_audio(RX_packet.payload);
        }
    }
#endif
}
