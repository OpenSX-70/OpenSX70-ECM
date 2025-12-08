#ifndef METER_H
#define METER_H

#include "main.h"
#include "settings.h"

extern ADC_HandleTypeDef hadc1;

typedef enum {
    ISO_640,
    ISO_125
} meter_iso;

void integrator_init(void);
void integrator_reset(void);
void meter_set_iso(meter_iso iso_setting);

#endif