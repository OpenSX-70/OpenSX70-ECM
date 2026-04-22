#ifndef METER_H
#define METER_H

#include "settings.h"
#include <stdint.h>

extern ADC_HandleTypeDef hadc1;
extern ADC_AnalogWDGConfTypeDef AnalogWDGConfig;
extern TIM_HandleTypeDef htim3;
extern volatile bool poller_exposure_complete;
extern volatile bool integration_started;
extern volatile uint32_t tim3_overflow_count;

typedef enum {
    ISO_640,
    ISO_125
} meter_iso;

typedef enum {
    OFF,
    LOW_LIGHT,
    MANUAL_METER
} light_meter_helper;

struct meter_settings{
    uint32_t flash_delay_threshold;
    uint32_t flash_fire_threshold;
    uint32_t auto_exposure_threshold;
};

void integrator_init(meter_iso *iso_setting);
void meter_set_iso(meter_iso *iso_setting);
void watchdog_config(uint32_t *threshold);
void approximate_exposure_time(light_meter_helper lm_helper);

extern struct meter_settings settings_640;
extern struct meter_settings settings_125;
extern struct meter_settings *current_settings;

extern ADC_AnalogWDGConfTypeDef MeterWDGConfig;

#endif