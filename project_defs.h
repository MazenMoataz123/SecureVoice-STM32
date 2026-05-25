#ifndef PROJECT_DEFS_H 
#define PROJECT_DEFS_H  

#include <stdint.h>
#include <stdbool.h>


#define SYNC_WORD    0xAA
#define PAYLOAD_SIZE 160


typedef enum {
    STATE_BOOT,         //0
    STATE_IDLE,         //1
    STATE_ACTIVE_COMM,  //2
    STATE_ERROR_HALT    //3
} SystemState_t;    //to help with system serial communication insteaf of just numbers

typedef enum {
    TASK_AUDIO = 0,
    TASK_CRYPTO = 1,
    TASK_TRANSPORT_TX = 2,
    TASK_TRANSPORT_RX = 3,
    TASK_SYSTEM = 4,
    TASK_PLAYBACK = 5
} TaskID_t; //in the scope of one 20ms window, there is only 1 of each task so there is no reaoson for actually numbering 


typedef struct __attribute__((packed)) {
    uint8_t  sync_byte;
    uint16_t packet_id;
    uint8_t  flags;
    uint8_t  payload[PAYLOAD_SIZE];
    uint16_t crc16;
} SecurePacket_t;   //defines and ensures packets are sent back to back without any tampering in other layers
#define PACKET_SIZE sizeof(SecurePacket_t)
_Static_assert(sizeof(SecurePacket_t) == 166, "Packet size mismatch");





#endif