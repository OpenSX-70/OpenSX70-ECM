#ifndef METER_H
#define METER_H

#include "settings.h"
#include <stdint.h>

extern ADC_HandleTypeDef hadc1;

typedef enum {
    ISO_640,
    ISO_125
} meter_iso;

struct meter_settings{
    meter_iso iso_setting;
    int32_t flash_delay_threshold;
    int32_t flash_fire_threshold;
    int32_t auto_exposure_threshold;
};

void integrator_reset(void);
void meter_set_iso(meter_iso iso_setting);
void meter_auto(struct meter_settings* settings);

extern struct meter_settings current_settings;

#endif