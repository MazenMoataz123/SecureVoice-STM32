#include "sys_health.h"
#include <stdio.h>

static uint32_t task_start_times[20]; // Assuming max 20 tasks for simplicity
static uint32_t task_end_time[20];

static uint32_t fake_time = 0;

static uint8_t battery_level = 100;

void SysHealth_Init(void){
    for(int i=0; i<20;i++){
        task_start_times[i] = 0;
        task_end_time[i] = 0;
    }

    battery_level = 100;
    fake_time=0;
    printf("System Health Initialized\n");
}



bool SysHealth_VerifyPacket(SecurePacket_t *pkt)
{
    if(pkt == NULL) return false;
    if(pkt->sync_byte != SYNC_WORD) return false;
    return true;
}

uint16_t SysHealth_CalculateCRC(uint8_t *data, uint16_t len)
{
    uint16_t crc = 0;
    for (uint16_t i = 0; i < len; i++)
    {
        crc ^= data[i];    
        

    }
    return crc;
}

void SysHealth_MarkTaskStart(uint8_t task_id)
{
    if(task_id < 20) {
        task_start_times[task_id] = fake_time; // In real implementation, this would be a timestamp
    }
}

void SysHealth_MarkTaskEnd(uint8_t task_id)
{
    if (task_id < 10)
    {
        task_end_time[task_id] = fake_time;
    }
}




void SysHealth_ReportStatus(SystemState_t current_state)
{
    printf("[SYS] State: %d | Battery: %d%%\n",
           current_state,
           battery_level);
}


void SysHealth_NotifyOverrun(TaskID_t task)
{
    printf("[SYS] Task %d overrun detected\n", task);
}