#include "shutterbutton.h"

struct ShutterButton S1_state = {0};

void update_button_state(struct ShutterButton *button_state) {
    uint32_t invocationTime = HAL_GetTick();

    bool S1T_current_state = (HAL_GPIO_ReadPin(S1T_GPIO_Port, S1T_Pin) == GPIO_PIN_SET);
    bool S1F_current_state = (HAL_GPIO_ReadPin(S1F_GPIO_Port, S1F_Pin) == GPIO_PIN_SET);

    if(button_state->simultaneous_press) {
        // If a simultaneous press has already been detected, check if press delay has passed before updating s1 state
        if((invocationTime - button_state->S1F_pressed_timestamp) > SIMULTANEOUS_PRESS_DELAY) {
            button_state->S1T_state = true;
            button_state->S1T_pressed_timestamp = invocationTime;
        }
        return;
    }

    if((S1T_current_state && S1F_current_state) && !(button_state->S1T_state || button_state->S1F_state)) {
        // Both S1T and S1F are pressed simultaneously and were not previously pressed (Shutter release)
        // This is specifically for sonar model cameras so they can autofocus prior to shutter release.

        button_state->simultaneous_press = true;
        button_state->S1F_state = true;
        button_state->S1F_pressed_timestamp = invocationTime;

        return;
    }

    if((button_state->S1T_state == false) && (S1T_current_state == true)){
        button_state->S1T_state = true;
        button_state->S1T_pressed_timestamp = invocationTime;
    }


    
    if((button_state->S1F_state == false) && (S1F_current_state == true)){
        button_state->S1F_state = true;
        button_state->S1F_pressed_timestamp = invocationTime;
    }
}

void reset_button_state(struct ShutterButton *button_state) {
    button_state->S1T_state = false;
    button_state->S1F_state = false;
    button_state->simultaneous_press = false;
    button_state->S1T_pressed_timestamp = 0;
    button_state->S1F_pressed_timestamp = 0;
}

uint32_t abs_diff(uint32_t a, uint32_t b) {
    return (a > b) ? (a - b) : (b - a);
}