#include "timing.h"
#include "project_defs.h"

static volatile uint32_t ms_counter = 0;
static volatile int frame_flag = 0;

//this manages the timing, the whole process of recevi

void time_init(void) {
    ms_counter = 0;
    frame_flag = 0;
}

void time_tick(void) {
    ms_counter++;
    if(ms_counter>= FRAME_DURATION_MS) {
        ms_counter = 0;
        frame_flag = 1; // announcing that a frame has been processed/arrived/is ready
    }
}

int time_frame_ready(void){
    return frame_flag;
}

void time_clear_frame_flag(void) {
    frame_flag = 0;
}