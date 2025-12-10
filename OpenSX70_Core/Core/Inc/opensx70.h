#ifndef opensx70_h
#define opensx70_h

#include "main.h"
#include "camerafunctions.h"
#include "peripheralport.h"

extern TIM_HandleTypeDef htim1;
extern ADC_HandleTypeDef hadc1;

typedef enum{
    STATE_INIT,
    STATE_DARKSLIDE,
    STATE_NODONGLE,
    STATE_DONGLE,
    STATE_FLASHBAR,
    STATE_MULTI_EXP,
    STATE_N
} camera_state;


void opensx70_run_state_machine (void);
camera_state do_state_init(void);
camera_state do_state_darkslide(void);
camera_state do_state_noDongle(void);
camera_state do_state_dongle(void);
camera_state do_state_flashBar(void);
camera_state do_state_multi_exp(void);
camera_state return_state(peripheral_device *device);
void ISOBlink(meter_iso *savedISO);
void save_iso(meter_iso iso);
meter_iso read_iso(void);
void s1_iso_swap(void);

#endif