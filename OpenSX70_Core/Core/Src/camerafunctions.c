#include "camerafunctions.h"

volatile bool auto_timeout_flag = false;

void solenoid_init(void){
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
}

void shutter_close(){
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 255);
    HAL_Delay(POWER_DOWN_DELAY);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 77);
}

void shutter_open(){
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
}

void sol2_engage(){
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 255);
}

void sol2_disengage(){
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
}

void sol2_low_power(){
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 77);
}

void mirror_down(){
    HAL_GPIO_WritePin(MOTOR_GPIO_Port, MOTOR_Pin, GPIO_PIN_SET);

    while(HAL_GPIO_ReadPin(S5_GPIO_Port, S5_Pin) != GPIO_PIN_RESET){
        //Wait for S5 to go low
    }

    HAL_GPIO_WritePin(MOTOR_GPIO_Port, MOTOR_Pin, GPIO_PIN_RESET);
}

void mirror_up(){
    if(HAL_GPIO_ReadPin(S3_GPIO_Port, S3_Pin) != GPIO_PIN_SET){
        HAL_GPIO_WritePin(MOTOR_GPIO_Port, MOTOR_Pin, GPIO_PIN_SET);
    }

    while(HAL_GPIO_ReadPin(S5_GPIO_Port, S5_Pin) != GPIO_PIN_SET){
        //Wait for S5 to go high
    }

    HAL_GPIO_WritePin(MOTOR_GPIO_Port, MOTOR_Pin, GPIO_PIN_RESET);
}

void darkslide_eject(){
    shutter_close();
    mirror_up();
    mirror_down();
    shutter_open();
}

void begin_exposure(){
    shutter_close();
    HAL_Delay(40);
    mirror_up();
}

void auto_exposure(meter_iso *iso_setting){
    HAL_Delay(Y_DELAY);

    meter_set_iso(iso_setting);
    integrator_reset();
    watchdog_config(&current_settings->auto_exposure_threshold);
    
    HAL_SuspendTick();
    HAL_TIM_Base_Start_IT(&htim3);
    while(!__HAL_ADC_GET_FLAG(&hadc1, ADC_FLAG_AWD1) || !auto_timeout_flag){
        //Wait for watchdog to trigger or auto timeout
    };
}

void auto_exposure_flashbar(meter_iso *iso_setting){

}