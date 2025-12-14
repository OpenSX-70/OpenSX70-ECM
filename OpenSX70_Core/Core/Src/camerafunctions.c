#include "camerafunctions.h"


volatile bool auto_timeout_flag = false;
volatile bool fd_timeout_flag = false;
volatile bool ff_timeout_flag = false;
volatile bool multiple_exposure_flag = false;

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
    HAL_GPIO_WritePin(MOTOR_GPIO_Port, MOTOR_Pin, 1);

    uint8_t stable = 0;
    while(stable < DEBOUNCE_DELAY){
        if(HAL_GPIO_ReadPin(S5_GPIO_Port, S5_Pin) == 0){
            stable++;
        } else {
            stable = 0;
        }
        HAL_Delay(1);
    }

    HAL_GPIO_WritePin(MOTOR_GPIO_Port, MOTOR_Pin, 0);
}

void mirror_up(){
    if(HAL_GPIO_ReadPin(S3_GPIO_Port, S3_Pin) == 0){
        HAL_GPIO_WritePin(MOTOR_GPIO_Port, MOTOR_Pin, 1);
    }

    uint8_t stable = 0;
    while(stable < DEBOUNCE_DELAY){
        if(HAL_GPIO_ReadPin(S5_GPIO_Port, S5_Pin) == 1){
            stable++;
        } else {
            stable = 0;
        }
        HAL_Delay(1);
    }

    HAL_GPIO_WritePin(MOTOR_GPIO_Port, MOTOR_Pin, 0);
}

void sonar_focus(){
    if(HAL_GPIO_ReadPin(S1F_GPIO_Port, S1F_Pin)){
        HAL_GPIO_WritePin(S1F_FBW_GPIO_Port, S1F_FBW_Pin, GPIO_PIN_SET);
    }
    else{
        HAL_GPIO_WritePin(S1F_FBW_GPIO_Port, S1F_FBW_Pin, GPIO_PIN_RESET);
    }
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
    HAL_GPIO_WritePin(LM_RESET_GPIO_Port, LM_RESET_Pin, 1);

    meter_set_iso(iso_setting);
    watchdog_config(&current_settings->auto_exposure_threshold);
    HAL_Delay(Y_DELAY);
    
    //HAL_SuspendTick();
    shutter_open();
    HAL_GPIO_WritePin(LM_RESET_GPIO_Port, LM_RESET_Pin, 0);
    __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_AWD1);
    while(!__HAL_ADC_GET_FLAG(&hadc1, ADC_FLAG_AWD1)){

    }
    __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_AWD1);

    exposure_finish();
}

void auto_exposure_flashbar(meter_iso *iso_setting){
    s2_ffa_mode();
    fd_timeout_flag = false;
    ff_timeout_flag = false;
    TIM16->CNT = 0;
    TIM17->CNT = 0;
    __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_AWD1);

    meter_set_iso(iso_setting);
    watchdog_config(&current_settings->flash_delay_threshold);
    integrator_reset();

    sol2_engage();
    HAL_Delay(Y_DELAY);
    sol2_low_power();
    HAL_SuspendTick();
    HAL_TIM_Base_Start_IT(&htim16);

    shutter_open();
    while(!__HAL_ADC_GET_FLAG(&hadc1, ADC_FLAG_AWD1) || !fd_timeout_flag){
        //Wait for watchdog to trigger or fd timeout
    }
    HAL_TIM_Base_Stop_IT(&htim16);
    __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_AWD1);

    HAL_GPIO_WritePin(FF_PIN_GPIO_Port, FF_PIN_Pin, GPIO_PIN_SET);

    watchdog_config(&current_settings->flash_fire_threshold);
    HAL_TIM_Base_Start_IT(&htim17);
    while(!__HAL_ADC_GET_FLAG(&hadc1, ADC_FLAG_AWD1) || !ff_timeout_flag){
        //Wait for watchdog to trigger or ff timeout
    }
    HAL_TIM_Base_Stop_IT(&htim17);
    HAL_GPIO_WritePin(FF_PIN_GPIO_Port, FF_PIN_Pin, GPIO_PIN_RESET);

    sol2_disengage();
    exposure_finish();
    s2_usart_mode();
    __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_AWD1);
}

void manual_exposure(uint8_t selector_value){
    HAL_Delay(Y_DELAY);

    if(selector_value >= Dongle_Flash_Limit){
        uint32_t delay_time = (selector_value - Flash_Capture_Delay);
        HAL_Delay(delay_time);
    }
}

void bulb_mode(){
    HAL_Delay(Y_DELAY);
    shutter_open();
    while(!HAL_GPIO_ReadPin(S1T_GPIO_Port, S1T_Pin));
    //Fast flash function here
    //HAL_Delay(Flash_Capture_Delay);
    exposure_finish();
}

void time_mode(){
    HAL_Delay(Y_DELAY);
    shutter_open();
    while(HAL_GPIO_ReadPin(S1T_GPIO_Port, S1T_Pin));
    while(!HAL_GPIO_ReadPin(S1T_GPIO_Port, S1T_Pin));
    //Fast flash function here
    //HAL_Delay(Flash_Capture_Delay);
    exposure_finish();
}

void exposure_finish(){
    HAL_ResumeTick();
    shutter_close();
    HAL_Delay(30);

    if(multiple_exposure_flag){
        while(HAL_GPIO_ReadPin(S1T_GPIO_Port, S1T_Pin));
        return;
    }
    else{
        HAL_Delay(100);
        while(HAL_GPIO_ReadPin(S1T_GPIO_Port, S1T_Pin));
        mirror_down();
        shutter_open();
        HAL_Delay(100);
    }
}

void s2_ffa_mode(){
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = S2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(S2_GPIO_Port, &GPIO_InitStruct);
    HAL_GPIO_WritePin(S2_GPIO_Port, S2_Pin, GPIO_PIN_RESET);
}

void s2_usart_mode(){
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = S2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_USART2;
    HAL_GPIO_Init(S2_GPIO_Port, &GPIO_InitStruct);
}