#include "shutterbutton.h"

struct ShutterButton S1_state = {0};

static bool update_debounced_input(bool raw_state, bool *debounced_state, bool *candidate_state, uint32_t *debounce_timestamp, uint32_t invocation_time){
    if(raw_state != *candidate_state){
        *candidate_state = raw_state;
        *debounce_timestamp = invocation_time;
        return false;
    }

    if((raw_state != *debounced_state) && ((invocation_time - *debounce_timestamp) >= SHUTTER_BUTTON_DEBOUNCE_DELAY)){
        *debounced_state = raw_state;
        return true;
    }

    return false;
}

static void cancel_simultaneous_press(struct ShutterButton *button_state){
    button_state->simultaneous_press = false;

    if(!button_state->S1T_debounced_input_state){
        button_state->S1T_state = false;
        button_state->S1T_pressed_timestamp = 0;
    }
}

void update_button_state(struct ShutterButton *button_state){
    uint32_t invocation_time = HAL_GetTick();

    bool S1T_raw_state = (HAL_GPIO_ReadPin(S1T_GPIO_Port, S1T_Pin) == GPIO_PIN_SET);
    bool S1F_raw_state = (HAL_GPIO_ReadPin(S1F_GPIO_Port, S1F_Pin) == GPIO_PIN_SET);

    bool S1F_changed = update_debounced_input(S1F_raw_state, &button_state->S1F_debounced_input_state, &button_state->S1F_candidate_state, &button_state->S1F_debounce_timestamp, invocation_time);
    bool S1T_changed = update_debounced_input(S1T_raw_state, &button_state->S1T_debounced_input_state, &button_state->S1T_candidate_state, &button_state->S1T_debounce_timestamp, invocation_time);

    if(S1F_changed){
        if(button_state->S1F_debounced_input_state){
            button_state->S1F_state = true;
            button_state->S1F_pressed_timestamp = invocation_time;
        } 
        else{
            button_state->S1F_state = false;
            button_state->S1F_pressed_timestamp = 0;
            cancel_simultaneous_press(button_state);
        }
    }

    if(S1T_changed){
        if(button_state->S1T_debounced_input_state){
            if(button_state->S1F_state && ((invocation_time - button_state->S1F_pressed_timestamp) <= SIMULTANEOUS_PRESS_WINDOW)){
                button_state->simultaneous_press = true;
            } 
            else{
                button_state->S1T_state = true;
                button_state->S1T_pressed_timestamp = invocation_time;
            }
        } 
        else{
            button_state->S1T_state = false;
            button_state->S1T_pressed_timestamp = 0;
            cancel_simultaneous_press(button_state);
        }
    }

    if(button_state->simultaneous_press && (!button_state->S1F_debounced_input_state || !button_state->S1T_debounced_input_state)){
        cancel_simultaneous_press(button_state);
    }

    if(button_state->simultaneous_press && !button_state->S1T_state && ((invocation_time - button_state->S1F_pressed_timestamp) >= SIMULTANEOUS_PRESS_DELAY)){
        button_state->S1T_state = true;
        button_state->S1T_pressed_timestamp = invocation_time;
    }
}
