#ifndef CONFIG_H
#define CONFIG_H

//SYSTEM CONFIG ================================

#define SAMPLE_RATE 8000            //8Khz aka 8k samples per second, those sample come in a rate that is native to the mic
#define FRAME_DURATION_MS  1000     // slow 9600 baud Bluetooth test pacing; choppy, not real-time voice
#define FRAME_SIZE         160      // fixed SecureVoice payload size in bytes



#define UART_BAUDRATE      115200
#define USE_DMA            1        // 1 = enabled, 0 = disabled

#endif // CONFIG_H
