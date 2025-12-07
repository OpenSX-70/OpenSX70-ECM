#include "camerafunctions.h"
#include "main.h"

void solenoid_init(void){
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
}

void shutter_close(){
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 255);
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