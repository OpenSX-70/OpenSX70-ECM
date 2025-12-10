#include "meter.h"

struct meter_settings *current_settings;
struct meter_settings settings_640;
struct meter_settings settings_125;

ADC_AnalogWDGConfTypeDef MeterWDGConfig;

void integrator_init(void){
    settings_640.iso_setting = ISO_640;
    settings_640.flash_delay_threshold = FD600;
    settings_640.flash_fire_threshold = FF600;
    settings_640.auto_exposure_threshold = A600;

    settings_125.iso_setting = ISO_125;
    settings_125.flash_delay_threshold = FD100;
    settings_125.flash_fire_threshold = FF100;
    settings_125.auto_exposure_threshold = A100;

    // Using the same base config for watchdog. 2 and 3 are both 8 bit so we will just reconfigure awd1
    MeterWDGConfig.WatchdogNumber = ADC_ANALOGWATCHDOG_1;
    MeterWDGConfig.LowThreshold = 0;
    MeterWDGConfig.HighThreshold = 0;
    MeterWDGConfig.Channel = ADC_CHANNEL_3;
    MeterWDGConfig.WatchdogMode = ADC_ANALOGWATCHDOG_SINGLE_REG;
    MeterWDGConfig.ITMode = DISABLE;
}

void integrator_reset(void){
    HAL_GPIO_WritePin(LM_RESET_GPIO_Port, LM_RESET_Pin, GPIO_PIN_SET);
    HAL_Delay(METER_RESET_DELAY);
    HAL_GPIO_WritePin(LM_RESET_GPIO_Port, LM_RESET_Pin, GPIO_PIN_RESET);
}

void meter_set_iso(meter_iso *iso_setting){
    if(*iso_setting == ISO_640){
        current_settings = &settings_640;
    } else if(*iso_setting == ISO_125){
        current_settings = &settings_125;
    }
}

void watchdog_config(uint32_t *threshold){
    MeterWDGConfig.HighThreshold = *threshold;
    HAL_ADC_AnalogWDGConfig(&hadc1, &MeterWDGConfig);
    __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_AWD1);
}