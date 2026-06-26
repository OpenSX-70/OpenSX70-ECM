#ifndef SHUTTERBUTTON_H
#define SHUTTERBUTTON_H  

#include "main.h"
#include "settings.h"
#include <stdbool.h>
#include <stdint.h>

struct ShutterButton{
    uint32_t S1T_pressed_timestamp;
    uint32_t S1F_pressed_timestamp;
    uint32_t S1T_debounce_timestamp;
    uint32_t S1F_debounce_timestamp;
    bool S1T_state;
    bool S1T_candidate_state;
    bool S1T_debounced_input_state;
    bool S1F_state;
    bool S1F_candidate_state;
    bool S1F_debounced_input_state;
    bool simultaneous_press;
};

extern struct ShutterButton S1_state;

void update_button_state(struct ShutterButton *button_state);

#endif // SHUTTERBUTTON_H
