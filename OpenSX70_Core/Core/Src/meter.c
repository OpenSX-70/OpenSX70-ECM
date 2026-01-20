#include "meter.h"

struct meter_settings *current_settings;
struct meter_settings settings_640;
struct meter_settings settings_125;
struct meter_settings settings_polling;
volatile bool poller_exposure_complete = false;
volatile bool integration_started = false;
volatile uint32_t tim3_overflow_count = 0;

ADC_AnalogWDGConfTypeDef MeterWDGConfig;

void integrator_init(meter_iso *iso_setting){
    settings_640.flash_delay_threshold = FD600;
    settings_640.flash_fire_threshold = FF600;
    settings_640.auto_exposure_threshold = A600;

    settings_125.flash_delay_threshold = FD100;
    settings_125.flash_fire_threshold = FF100;
    settings_125.auto_exposure_threshold = A100;

    settings_polling.flash_delay_threshold = 0;
    settings_polling.flash_fire_threshold = 0;
    settings_polling.auto_exposure_threshold = 4000;

    MeterWDGConfig.LowThreshold = 0;
    MeterWDGConfig.Channel = ADC_CHANNEL_3;
    MeterWDGConfig.WatchdogMode = ADC_ANALOGWATCHDOG_SINGLE_REG;
    MeterWDGConfig.ITMode = DISABLE;

    switch(*iso_setting){
        case ISO_640:
            current_settings = &settings_640;
            break;
        case ISO_125:
            current_settings = &settings_125;
            break;
    }
    
    MeterWDGConfig.WatchdogNumber = ADC_ANALOGWATCHDOG_1;
    MeterWDGConfig.HighThreshold = current_settings->auto_exposure_threshold;
    HAL_ADC_AnalogWDGConfig(&hadc1, &MeterWDGConfig);
}

void integrator_reset(void){
    HAL_GPIO_WritePin(LM_RESET_GPIO_Port, LM_RESET_Pin, 1);
    HAL_Delay(METER_RESET_DELAY);
    HAL_GPIO_WritePin(LM_RESET_GPIO_Port, LM_RESET_Pin, 0);
}

void meter_set_iso(meter_iso *iso_setting){
    switch(*iso_setting){
        case ISO_640:
            current_settings = &settings_640;
            break;
        case ISO_125:
            current_settings = &settings_125;
            break;
    }
}

void watchdog_config(uint32_t *threshold){
    MeterWDGConfig.HighThreshold = *threshold;
    MeterWDGConfig.ITMode = DISABLE;
    HAL_ADC_AnalogWDGConfig(&hadc1, &MeterWDGConfig);
}

void approximate_exposure_time(light_meter_helper lm_helper){
    //static uint32_t predicted_us; 

    if(lm_helper == OFF){
        HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
        return;
    }
    
    if(!integration_started){
        htim3.Init.Prescaler = 15;
        htim3.Init.Period = 65535;
        if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
        {
            Error_Handler();
        }
        tim3_overflow_count = 0;
        __HAL_TIM_SET_COUNTER(&htim3, 0);
        __HAL_TIM_CLEAR_FLAG(&htim3, TIM_FLAG_UPDATE);
        __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_AWD1);

        HAL_GPIO_WritePin(LM_RESET_GPIO_Port, LM_RESET_Pin, 0);
        HAL_TIM_Base_Start_IT(&htim3);
        integration_started = true;
        return;
    }
    else{
        if(tim3_overflow_count >= METER_OVERFLOW_THRESHOLD){
            HAL_TIM_Base_Stop_IT(&htim3);
            HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
            integration_started = false;
            HAL_GPIO_WritePin(LM_RESET_GPIO_Port, LM_RESET_Pin, 1);
        }
        else if(__HAL_ADC_GET_FLAG(&hadc1, ADC_FLAG_AWD1)){
            HAL_TIM_Base_Stop_IT(&htim3);
            HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
            integration_started = false;
            HAL_GPIO_WritePin(LM_RESET_GPIO_Port, LM_RESET_Pin, 1);
        }
    }

    
}

