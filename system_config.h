#ifndef CONFIG_H
#define CONFIG_H

//SYSTEM CONFIG ================================

#define SAMPLE_RATE 8000            //8Khz aka 8k samples per second, those sample come in a rate that is native to the mic
#define FRAME_DURATION_MS  20       // so this means thats 8000 bytes per sec,ten we assumed that a decent amount of frame duration is 20ms, so 8000 x 20ms we are getting 160 samples per frame
#define FRAME_SIZE         160      // 8000 x 20ms = 160 samples per frame (sample is a byte in this case)



#define UART_BAUDRATE      115200
#define USE_DMA            1        // 1 = enabled, 0 = disabled

#endif // CONFIG_H