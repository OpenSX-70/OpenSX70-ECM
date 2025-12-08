#include "meter.h"

struct meter_settings current_settings;

void integrator_reset(void){
    HAL_GPIO_WritePin(LM_RESET_GPIO_Port, LM_RESET_Pin, GPIO_PIN_SET);
    HAL_Delay(METER_RESET_DELAY);
    HAL_GPIO_WritePin(LM_RESET_GPIO_Port, LM_RESET_Pin, GPIO_PIN_RESET);
}

void meter_set_iso(meter_iso iso_setting){
    if(iso_setting == ISO_640){
        current_settings.iso_setting = ISO_640;
        current_settings.flash_delay_threshold = FD600;
        current_settings.flash_fire_threshold = FF600;
        current_settings.auto_exposure_threshold = A600;
    } else if(iso_setting == ISO_125){
        current_settings.iso_setting = ISO_125;
        current_settings.flash_delay_threshold = FD100;
        current_settings.flash_fire_threshold = FF100;
        current_settings.auto_exposure_threshold = A100;
    }
}