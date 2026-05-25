#ifndef SYS_HEALTH_H
#define SYS_HEALTH_H

#include "project_defs.h"

// Initialization
void SysHealth_Init(void);

// Data Validation
bool SysHealth_VerifyPacket(SecurePacket_t *pkt);
uint16_t SysHealth_CalculateCRC(uint8_t *data, uint16_t len);

// Performance Monitoring
void SysHealth_MarkTaskStart(uint8_t task_id);
void SysHealth_MarkTaskEnd(uint8_t task_id);

// Telemetry
void SysHealth_ReportStatus(SystemState_t current_state);

void SysHealth_WatchdogPet(void);

uint8_t SysHealth_GetBatteryLevel(void);

void SysHealth_NotifyOverrun(TaskID_t task);

#endif