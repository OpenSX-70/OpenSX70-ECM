#ifndef PERIPHERAL_PORT_H
#define PERIPHERAL_PORT_H

#include "settings.h"

//Keeping things similar to the DS2408 for funsies.
#define PERIPHERAL_PING_CMD 0x01
#define PERIPHERAL_ACK 0x02
#define CAMERA_ISO_600 0x03
#define CAMERA_ISO_SX70 0x04

#define ALL_LEDS_OFF 0x05

#define GREEN_ON 0x06
#define GREEN_OFF 0x07

#define RED_ON 0x08
#define RED_OFF 0x09

#define BLUE_ON 0x0A
#define BLUE_OFF 0x0B

#define PERIPHERAL_READ_CMD 0xF5
#define PERIPHERAL_SELF_TIMER_CMD 0xF6



typedef enum {
    PERIPHERAL_NONE = 0,
    PERIPHERAL_DONGLE,
    PERIPHERAL_FLASHBAR,
    PERIPHERAL_UNKNOWN
} peripheral_type;

typedef enum peripheral_state {
    DONGLE_STATE_NODONGLE,
    DONGLE_STATE_DONGLE,
    DONGLE_STATE_FLASHBAR,
    DONGLE_STATE_N
} peripheral_state;

typedef struct peripheral_device {
    peripheral_type type;
    uint8_t selector;
    bool switch1;
    bool switch2;
} peripheral_device;

void initialize_peripheral_device(peripheral_device *device);
void set_peripheral_device(peripheral_device *device, uint8_t selector, bool switch1, bool switch2, peripheral_type type);
void update_peripheral_status(peripheral_device *device);
void send_command(uint8_t command);
bool get_dongle_settings(peripheral_device *device);
bool get_switch_state(uint8_t switch_number);

extern peripheral_device current_dongle_state;
extern volatile bool dongle_response_received;
extern volatile bool waiting_for_ping_response;
extern uint8_t peripheral_uart_buffer[1];
extern UART_HandleTypeDef huart2;

#endif
