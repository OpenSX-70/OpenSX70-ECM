#ifndef CAMERAFUNCTIONS_H
#define CAMERAFUNCTIONS_H  

#include "main.h"
#include "meter.h"
#include "settings.h"
#include "stm32g0xx_hal_tim.h"

extern TIM_HandleTypeDef htim1;  // Solenoid PWM
extern TIM_HandleTypeDef htim3;  // Auto exposure timeout. Maybe self timer after reconfig as well?
extern TIM_HandleTypeDef htim14; // Polling timer
extern TIM_HandleTypeDef htim16; // FD timeout timer
extern TIM_HandleTypeDef htim17; // FF timeout timer

extern volatile bool auto_exposure_timeout_flag;
extern volatile bool tim16_timeout_flag;
extern volatile bool tim17_timeout_flag;
extern volatile bool multiple_exposure_flag;

void solenoid_init(void);
void shutter_close(void);
void shutter_open(void);
void sol2_engage(void);
void sol2_disengage(void);
void sol2_low_power(void);
void mirror_down(void);
void mirror_up(void);
void sonar_focus(void);
void darkslide_eject(void);
void begin_exposure(void);
void auto_exposure(meter_iso *iso_setting);
void auto_exposure_flashbar(meter_iso *iso_setting);
void manual_exposure(uint8_t selector_value);
void bulb_mode(void);
void time_mode(void);
void exposure_finish(void);
void flash(void);
void s2_ffa_mode(void);
void s2_usart_mode(void);
#endif