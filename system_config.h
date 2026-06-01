#ifndef CONFIG_H
#define CONFIG_H

//SYSTEM CONFIG ================================

#define DEVICE_ROLE_TRANSMITTER 1
#define DEVICE_ROLE_RECEIVER    2

#define DEVICE_ROLE DEVICE_ROLE_TRANSMITTER

#define SAMPLE_RATE       8000
#define FRAME_DURATION_MS 20
#define FRAME_SIZE        160

#define UART_BAUDRATE      115200
#define USE_DMA            1        // 1 = enabled, 0 = disabled

#endif // CONFIG_H
