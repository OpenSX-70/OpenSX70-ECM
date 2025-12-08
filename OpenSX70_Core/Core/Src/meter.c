#include "meter.h"
#include "opensx70.h"
#include "stm32g0xx_hal_adc.h"

struct meter_settings *current_settings;
struct meter_settings settings_640;
struct meter_settings settings_125;

void integrator_init(void){
    settings_640.iso_setting = ISO_640;
    settings_640.flash_delay_threshold = FD600;
    settings_640.flash_fire_threshold = FF600;
    settings_640.auto_exposure_threshold = A600;

    settings_125.iso_setting = ISO_125;
    settings_125.flash_delay_threshold = FD100;
    settings_125.flash_fire_threshold = FF100;
    settings_125.auto_exposure_threshold = A100;

    //Add logic to set ISO from flash
    current_settings = &settings_640;

    AnalogWDGConfig.LowThreshold = 0;
    AnalogWDGConfig.Channel = ADC_CHANNEL_3;
    AnalogWDGConfig.WatchdogMode = ADC_ANALOGWATCHDOG_SINGLE_REG;
    AnalogWDGConfig.ITMode = DISABLE;

}

void integrator_reset(void){
    HAL_GPIO_WritePin(LM_RESET_GPIO_Port, LM_RESET_Pin, GPIO_PIN_SET);
    HAL_Delay(METER_RESET_DELAY);
    HAL_GPIO_WritePin(LM_RESET_GPIO_Port, LM_RESET_Pin, GPIO_PIN_RESET);
}

void meter_set_iso(meter_iso iso_setting){
    if(iso_setting == ISO_640){
        current_settings = &settings_640;
    } else if(iso_setting == ISO_125){
        current_settings = &settings_125;
    }
}

void auto_exposure_init(struct meter_settings* settings){
    AnalogWDGConfig.HighThreshold = settings->auto_exposure_threshold;
    AnalogWDGConfig.WatchdogNumber = ADC_ANALOGWATCHDOG_1;
    HAL_ADC_AnalogWDGConfig(&hadc1, &AnalogWDGConfig);

    __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_AWD1);
}

void flashbar_exposure_init(struct meter_settings* settings){
    AnalogWDGConfig.WatchdogNumber = ADC_ANALOGWATCHDOG_2;
    AnalogWDGConfig.HighThreshold = settings->flash_delay_threshold;
    HAL_ADC_AnalogWDGConfig(&hadc1, &AnalogWDGConfig);

    AnalogWDGConfig.WatchdogNumber = ADC_ANALOGWATCHDOG_3;
    AnalogWDGConfig.HighThreshold = settings->flash_fire_threshold;
    HAL_ADC_AnalogWDGConfig(&hadc1, &AnalogWDGConfig);

    __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_AWD2);
    __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_AWD3);
}