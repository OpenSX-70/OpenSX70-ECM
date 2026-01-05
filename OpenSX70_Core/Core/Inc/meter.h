#ifndef METER_H
#define METER_H

#include "settings.h"
#include <stdint.h>

extern ADC_HandleTypeDef hadc1;
extern ADC_AnalogWDGConfTypeDef AnalogWDGConfig;

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
    meter_iso iso_setting;
    uint32_t flash_delay_threshold;
    uint32_t flash_fire_threshold;
    uint32_t auto_exposure_threshold;
};

void integrator_init(void);
void integrator_reset(void);
void meter_set_iso(meter_iso *iso_setting);
void watchdog_config(uint32_t *threshold);

extern struct meter_settings settings_640;
extern struct meter_settings settings_125;
extern struct meter_settings *current_settings;

extern ADC_AnalogWDGConfTypeDef MeterWDGConfig;

#endif