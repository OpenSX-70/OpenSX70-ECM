#include "peripheralport.h"

peripheral_device current_dongle_state;

static uint8_t selector_mask = 0b00001111, switch1_mask = 0b00010000, switch2_mask = 0b00100000;

typedef enum peripheral_state {
    DONGLE_STATE_NODONGLE,
    DONGLE_STATE_DONGLE,
    DONGLE_STATE_FLASHBAR,
    DONGLE_STATE_N
} peripheral_state;

typedef peripheral_state (*peripheral_state_funct)(peripheral_device *device);

peripheral_state do_dongle_state_noDongle(peripheral_device *device);
peripheral_state do_dongle_state_dongle(peripheral_device *device);
peripheral_state do_dongle_state_flashBar(peripheral_device *device);

static const peripheral_state_funct PERIPHERAL_MACHINE[DONGLE_STATE_N] = {
    &do_dongle_state_noDongle,
    &do_dongle_state_dongle,
    &do_dongle_state_flashBar
};

peripheral_state port_state = DONGLE_STATE_NODONGLE;

void updatePeripheralStatus(peripheral_device *device){
    port_state = PERIPHERAL_MACHINE[port_state](device);
}

void initializePeripheralDevice(peripheral_device *device){
    device->selector = 200;
    device->switch1 = false;
    device->switch2 = false;
    device->type = PERIPHERAL_NONE;
}

peripheral_state do_dongle_state_noDongle(peripheral_device *device){
    if(HAL_GPIO_ReadPin(S2_GPIO_Port, S2_Pin) == GPIO_PIN_RESET){
        setPeripheralDevice(device, 100, false, false, PERIPHERAL_FLASHBAR);
        return DONGLE_STATE_FLASHBAR;
    }

    // Code for dongle detection would go here

    return DONGLE_STATE_NODONGLE;
}

peripheral_state do_dongle_state_flashBar(peripheral_device *device){
    if(HAL_GPIO_ReadPin(S2_GPIO_Port, S2_Pin) != GPIO_PIN_RESET){
        initializePeripheralDevice(device);
        return DONGLE_STATE_NODONGLE;
    }

    return DONGLE_STATE_FLASHBAR;
}

peripheral_state do_dongle_state_dongle(peripheral_device *device){
    return DONGLE_STATE_DONGLE;
}



void setPeripheralDevice(peripheral_device *device, uint8_t selector, bool switch1, bool switch2, peripheral_type type) {
    device->selector = selector;
    device->switch1 = switch1;
    device->switch2 = switch2;
    device->type = type;
}