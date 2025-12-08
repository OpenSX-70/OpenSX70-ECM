#include "meter.h"

void integrator_reset(void){
    HAL_GPIO_WritePin(LM_RESET_GPIO_Port, LM_RESET_Pin, GPIO_PIN_SET);
    HAL_Delay(METER_RESET_DELAY);
    HAL_GPIO_WritePin(LM_RESET_GPIO_Port, LM_RESET_Pin, GPIO_PIN_RESET);
}