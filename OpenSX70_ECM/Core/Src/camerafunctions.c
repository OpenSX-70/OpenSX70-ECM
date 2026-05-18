#include "camerafunctions.h"

volatile bool auto_timeout_flag = false;
volatile bool tim16_timeout_flag = false;
volatile bool tim17_timeout_flag = false;
volatile bool multiple_exposure_flag = false;
volatile bool tim3_timeout_flag = false;
volatile bool auto_exposure_active = false;

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

bool debounce_read(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState expected_state){
    uint8_t stable = 0;
    while(stable < DEBOUNCE_DELAY){
        if(HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == expected_state){
            stable++;
        } else {
            stable = 0;
        }
        HAL_Delay(1);
    }
    return true;
}

void mirror_down(){
    // Motor on until S5 closes.
    HAL_GPIO_WritePin(MOTOR_GPIO_Port, MOTOR_Pin, 1);

    debounce_read(S5_GPIO_Port, S5_Pin, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(MOTOR_GPIO_Port, MOTOR_Pin, 0);
}

void mirror_up(){
    // Motor on until S5 opens, wait for mirror up (s3 to close).
    HAL_GPIO_WritePin(MOTOR_GPIO_Port, MOTOR_Pin, 1);

    debounce_read(S5_GPIO_Port, S5_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(MOTOR_GPIO_Port, MOTOR_Pin, 0);

    while(HAL_GPIO_ReadPin(S3_GPIO_Port, S3_Pin) == GPIO_PIN_RESET);
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
    HAL_Delay(Y_DELAY);
    mirror_up();
    mirror_down();
    shutter_open();
}

void begin_exposure(){
    __HAL_ADC_DISABLE_IT(&hadc1, ADC_IT_AWD1);
    // Stop poller interrupts during exposure
    if(HAL_TIM_Base_Stop_IT(&htim14) != HAL_OK) {
        HAL_TIM_Base_Stop_IT(&htim14);
    }
    if(HAL_TIM_Base_Stop_IT(&htim3) != HAL_OK) {
        HAL_TIM_Base_Stop_IT(&htim3);
    }
    shutter_close();
    HAL_Delay(40);
    mirror_up();
}

void auto_exposure(meter_iso *iso_setting){
    // Auto mode timeouts set by film ISO to mimic original camera behavior. 
    // Auto exposure timeout for SX-70 speed cameras is 15 seconds.
    // Auto exposure timout for 600 speed cameras is 5 seconds.
    switch(*iso_setting){
        case ISO_125:
            htim3.Init.Prescaler = 3663;
            htim3.Init.Period = 65501;
            break;
        default:
            htim3.Init.Prescaler = 1231;
            htim3.Init.Period = 64934;    
    }
    
    if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_GPIO_WritePin(LM_RESET_GPIO_Port, LM_RESET_Pin, 1);
    __HAL_TIM_SET_COUNTER(&htim3, 0);
    __HAL_TIM_CLEAR_FLAG(&htim3, TIM_FLAG_UPDATE);
    tim3_timeout_flag = false;

    meter_set_iso(iso_setting);
    watchdog_config(&current_settings->auto_exposure_threshold);

    HAL_Delay(Y_DELAY);
    
    HAL_SuspendTick();

    if(HAL_TIM_Base_Start_IT(&htim3) != HAL_OK) {
        HAL_TIM_Base_Start_IT(&htim3);
    }

    __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_AWD1);
    HAL_GPIO_WritePin(LM_RESET_GPIO_Port, LM_RESET_Pin, 0);
    
    shutter_open();
    
    
    while(!__HAL_ADC_GET_FLAG(&hadc1, ADC_FLAG_AWD1) && !tim3_timeout_flag){
        // Wait for either ADC integration or 15 second timeout
    }

    if(HAL_TIM_Base_Stop_IT(&htim3) != HAL_OK) {
        HAL_TIM_Base_Stop_IT(&htim3);
    }

    exposure_finish();
    __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_AWD1);
}

void auto_exposure_flashbar(meter_iso *iso_setting){
    __HAL_TIM_SET_COUNTER(&htim16, 0);
    __HAL_TIM_SET_COUNTER(&htim17, 0);
    __HAL_TIM_CLEAR_FLAG(&htim16, TIM_FLAG_UPDATE);
    __HAL_TIM_CLEAR_FLAG(&htim17, TIM_FLAG_UPDATE);
    tim16_timeout_flag = false;
    tim17_timeout_flag = false;
    htim16.Init.Prescaler = 15;
    htim16.Init.Period = 55999;
    htim17.Init.Prescaler = 3;
    htim17.Init.Period = 47999;

    s2_ffa_mode();
    HAL_GPIO_WritePin(FFA_POWER_EN_GPIO_Port, FFA_POWER_EN_Pin, 0);

    HAL_GPIO_WritePin(LM_RESET_GPIO_Port, LM_RESET_Pin, 1);

    meter_set_iso(iso_setting);
    watchdog_config(&current_settings->flash_delay_threshold);

    sol2_engage();
    __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_AWD1);
    HAL_Delay(Y_DELAY);
    sol2_low_power();
    HAL_SuspendTick();

    HAL_StatusTypeDef tim16_status = HAL_TIM_Base_Start_IT(&htim16);
    if (tim16_status != HAL_OK) {
        tim16_status = HAL_TIM_Base_Start_IT(&htim16);
    }

    shutter_open();
    
    HAL_GPIO_WritePin(LM_RESET_GPIO_Port, LM_RESET_Pin, 0);
    __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_AWD1);
    while(!__HAL_ADC_GET_FLAG(&hadc1, ADC_FLAG_AWD1) && !tim16_timeout_flag){
        //Wait for watchdog to trigger or fd timeout
    }

    if(HAL_TIM_Base_Stop_IT(&htim16) != HAL_OK) {
        HAL_TIM_Base_Stop_IT(&htim16);
    }

    if(HAL_TIM_Base_Start_IT(&htim17) != HAL_OK) {
        HAL_TIM_Base_Start_IT(&htim17);
    }

    watchdog_config(&current_settings->flash_fire_threshold);

    __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_AWD1);
    
    HAL_GPIO_WritePin(FFA_POWER_EN_GPIO_Port, FFA_POWER_EN_Pin, 1);
    HAL_GPIO_WritePin(FF_PIN_GPIO_Port, FF_PIN_Pin, 1);
    
    sol2_disengage();
    
    while(!__HAL_ADC_GET_FLAG(&hadc1, ADC_FLAG_AWD1) && !tim17_timeout_flag){
        //Wait for watchdog to trigger or ff timeout
    }

    if(HAL_TIM_Base_Stop_IT(&htim17) != HAL_OK) {
        HAL_TIM_Base_Stop_IT(&htim17);
    }

    
    exposure_finish();
    __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_AWD1);
}

void manual_exposure(struct shutter_speed_timing *timing){
    HAL_Delay(Y_DELAY);
    HAL_SuspendTick();

    htim3.Init.Prescaler = timing->prescaler;
    htim3.Init.Period = timing->period;

    if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
    {
        Error_Handler();
    }

    __HAL_TIM_SET_COUNTER(&htim3, 0);
    __HAL_TIM_CLEAR_FLAG(&htim3, TIM_FLAG_UPDATE);
    tim3_timeout_flag = false;

    if(timing->flash_enabled){
        HAL_GPIO_WritePin(FFA_POWER_EN_GPIO_Port, FFA_POWER_EN_Pin, 0);
    }

    shutter_open();

    if(HAL_TIM_Base_Start_IT(&htim3) != HAL_OK) {
        HAL_TIM_Base_Start_IT(&htim3);
    }

    while(!tim3_timeout_flag){
        
    }
    if(timing->flash_enabled){
        flash();
    }

    if(HAL_TIM_Base_Stop_IT(&htim3) != HAL_OK) {
        HAL_TIM_Base_Stop_IT(&htim3);
    }
    exposure_finish();
}

#if FUZZY_MANUAL_MODE
void fuzzy_manual_exposure(struct shutter_speed_timing *timing, meter_iso *iso_setting){
    HAL_Delay(Y_DELAY);
    HAL_SuspendTick();

    #if FUZZY_MANUAL_MODE
    __HAL_TIM_SET_COUNTER(&htim16, 0);
    __HAL_TIM_SET_COUNTER(&htim17, 0);
    __HAL_TIM_CLEAR_FLAG(&htim16, TIM_FLAG_UPDATE);
    __HAL_TIM_CLEAR_FLAG(&htim17, TIM_FLAG_UPDATE);
    tim16_timeout_flag = false;
    tim17_timeout_flag = false;
    htim16.Init.Prescaler = timing->min_prescaler;
    htim16.Init.Period = timing->min_period;
    htim17.Init.Prescaler = timing->max_prescaler;
    htim17.Init.Period = timing->max_period;
    
    if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_TIM_Base_Init(&htim17) != HAL_OK)
    {
        Error_Handler();
    }
    
    HAL_GPIO_WritePin(LM_RESET_GPIO_Port, LM_RESET_Pin, 1);
    #else
    htim3.Init.Prescaler = timing->prescaler;
    htim3.Init.Period = timing->period;

    if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
    {
        Error_Handler();
    }

    __HAL_TIM_SET_COUNTER(&htim3, 0);
    __HAL_TIM_CLEAR_FLAG(&htim3, TIM_FLAG_UPDATE);
    tim3_timeout_flag = false;
    #endif

    if(timing->flash_enabled){
        HAL_GPIO_WritePin(FFA_POWER_EN_GPIO_Port, FFA_POWER_EN_Pin, 0);
    }

    shutter_open();

    #if FUZZY_MANUAL_MODE
    __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_AWD1);
    HAL_GPIO_WritePin(LM_RESET_GPIO_Port, LM_RESET_Pin, 0);
    if(HAL_TIM_Base_Start_IT(&htim16) != HAL_OK) {
        HAL_TIM_Base_Start_IT(&htim16);
    }
    if(HAL_TIM_Base_Start_IT(&htim17) != HAL_OK) {
        HAL_TIM_Base_Start_IT(&htim17);
    }

    while((!tim16_timeout_flag && !__HAL_ADC_GET_FLAG(&hadc1, ADC_FLAG_AWD1)) && !tim17_timeout_flag){
        
    }
    #else
    if(HAL_TIM_Base_Start_IT(&htim3) != HAL_OK) {
        HAL_TIM_Base_Start_IT(&htim3);
    }

    while(!tim3_timeout_flag){
        
    }
    #endif
    if(timing->flash_enabled){
        flash();
    }

    #if FUZZY_MANUAL_MODE
    if(HAL_TIM_Base_Stop_IT(&htim16) != HAL_OK) {
        HAL_TIM_Base_Stop_IT(&htim16);
    }
    if(HAL_TIM_Base_Stop_IT(&htim17) != HAL_OK) {
        HAL_TIM_Base_Stop_IT(&htim17);
    }
    #else
    if(HAL_TIM_Base_Stop_IT(&htim3) != HAL_OK) {
        HAL_TIM_Base_Stop_IT(&htim3);
    }
    #endif
    exposure_finish();
}
#endif

void bulb_mode(){
    HAL_Delay(Y_DELAY);
    HAL_GPIO_WritePin(FFA_POWER_EN_GPIO_Port, FFA_POWER_EN_Pin, 0);
    shutter_open();
    while(HAL_GPIO_ReadPin(S1T_GPIO_Port, S1T_Pin));
    flash();
    HAL_Delay(Flash_Capture_Delay);
    exposure_finish();
}

void time_mode(){
    HAL_Delay(Y_DELAY);
    HAL_GPIO_WritePin(FFA_POWER_EN_GPIO_Port, FFA_POWER_EN_Pin, 0);
    shutter_open();
    while(HAL_GPIO_ReadPin(S1T_GPIO_Port, S1T_Pin));
    while(!HAL_GPIO_ReadPin(S1T_GPIO_Port, S1T_Pin));
    flash();
    HAL_Delay(Flash_Capture_Delay);
    exposure_finish();
}

void exposure_finish(){
    HAL_ResumeTick();
    shutter_close();
    
    HAL_GPIO_WritePin(FFA_POWER_EN_GPIO_Port, FFA_POWER_EN_Pin, 0);
    HAL_GPIO_WritePin(FF_PIN_GPIO_Port, FF_PIN_Pin, 0);
    HAL_Delay(30);
    s2_usart_mode();
    HAL_GPIO_WritePin(FFA_POWER_EN_GPIO_Port, FFA_POWER_EN_Pin, 1);
    while(HAL_GPIO_ReadPin(S1T_GPIO_Port, S1T_Pin));
    if(multiple_exposure_flag){
        HAL_TIM_Base_Start_IT(&htim14);
        HAL_Delay(100);
        return;
    }
    else{
        mirror_down();
        shutter_open();
        HAL_Delay(100);
    }
    HAL_TIM_Base_Start_IT(&htim14); // Resume polling after exposure finished.
}

void flash(){
    HAL_GPIO_WritePin(FFA_POWER_EN_GPIO_Port, FFA_POWER_EN_Pin, 1);
    s2_ffa_mode();
    HAL_GPIO_WritePin(FF_PIN_GPIO_Port, FF_PIN_Pin, 1);
    // Reset happens in exposure finish
}

void s2_ffa_mode(){
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = S2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(S2_GPIO_Port, &GPIO_InitStruct);
    HAL_GPIO_WritePin(S2_GPIO_Port, S2_Pin, 0);
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
