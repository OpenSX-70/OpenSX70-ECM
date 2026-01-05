#include "settings.h"
#include "meter.h"
#include <stdint.h>

const uint8_t Y_DELAY = 100;
const uint8_t POWER_DOWN_DELAY = 15;

// *** FILL FLASH ***
// Explanation for these numbers. Concept is pulled directly from fill flash description in the alpha repair supplement.
// FD values are 1/3 of ISO magic numbers
// FF values are 1.3x the FD numbers
// FD (Flash Delay) value is the value the meter must hit prior to the flash firing. If integration time takes longer than 56ms (Flash_Max_Time), fire the flash regardless.
// FF (Flash Fired) value is the value the meter must hit before the shutter closes. This also has a timeout but is a much shorter 12ms (Flash_Capture_Max_Time).
// I am hard coding these values as the magic number is no longer changing per board.

const int32_t ShutterSpeed[] = {     23,   27,   32,    34,  39,   44,  56,   64,    125,  250, 500, 1000, POST, POSB, AUTO, AUTO_F};
