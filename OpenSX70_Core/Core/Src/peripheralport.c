#include "peripheralport.h"

peripheral_device current_dongle_state;
uint8_t peripheral_uart_buffer[1];
volatile bool dongle_response_received = false;

static uint8_t selector_mask = 0b00001111, switch1_mask = 0b00010000, switch2_mask = 0b00100000;

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

    send_command(PERIPHERAL_PING_CMD);

    return DONGLE_STATE_FLASHBAR;
}

peripheral_state do_dongle_state_dongle(peripheral_device *device){
    static bool dma_started = false;
    static uint32_t timeout_counter = 0;

    if (!dma_started) {
        if (get_dongle_settings(device)) {
            dma_started = true;
            timeout_counter = 0;
        } else {
            initializePeripheralDevice(device);
            dma_started = false;
            return DONGLE_STATE_NODONGLE;
        }
    }

    if (dongle_response_received) {
        dongle_response_received = false;
        dma_started = false;
        timeout_counter = 0;
        return DONGLE_STATE_DONGLE;
    }

    timeout_counter++;
    if (timeout_counter > PERIPHERAL_RESPONSE_TIMEOUT_TICKS) {
        initializePeripheralDevice(device);
        dma_started = false;
        timeout_counter = 0;
        dongle_response_received = false;
        return DONGLE_STATE_NODONGLE;
    }

    return DONGLE_STATE_DONGLE;
}

void setPeripheralDevice(peripheral_device *device, uint8_t selector, bool switch1, bool switch2, peripheral_type type) {
    device->selector = selector;
    device->switch1 = switch1;
    device->switch2 = switch2;
    device->type = type;
}

void send_command(uint8_t command){
    HAL_HalfDuplex_EnableTransmitter(&huart2);
    HAL_UART_Transmit(&huart2, &command, 1, PERIPHERAL_TIMEOUT_MS);
    HAL_HalfDuplex_EnableReceiver(&huart2);
}

bool get_dongle_settings(peripheral_device *device){
    bool success = false;
    send_command(PERIPHERAL_READ_CMD);

    HAL_StatusTypeDef status = HAL_UART_Receive_DMA(&huart2, peripheral_uart_buffer, 1);
    if (status == HAL_OK) {
        success = true;
    } else {
        success = false;
    }
    HAL_HalfDuplex_EnableTransmitter(&huart2);
    return success;
}

bool get_switch_state(uint8_t switch_number){
    switch (switch_number){
        case 1:
            return current_dongle_state.switch1;
        case 2:
            return current_dongle_state.switch2;
        default:
            return false;
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        setPeripheralDevice(&current_dongle_state, (peripheral_uart_buffer[0] & selector_mask), (peripheral_uart_buffer[0] & switch1_mask), (peripheral_uart_buffer[0] & switch2_mask), PERIPHERAL_DONGLE);
        dongle_response_received = true;
    }
}