#include "settings.h"
#include "meter.h"
#include <stdint.h>

const uint8_t Y_DELAY = 40;
const uint8_t POWER_DOWN_DELAY = 28;

// {PRESCALER, PERIOD (ARR), FLASH_ENABLED}
// Values are scaled to microseconds, if any feel too over/under there is a lot of flexibility.
struct shutter_speed_timing ShutterSpeedTiming[] = {
    {7, 45999, false, MANUAL_SPEED}, // S_1_2000, 23ms
    {7, 53999, false, MANUAL_SPEED}, // S_1_1000, 27ms
    {7, 61999, false, MANUAL_SPEED}, // S_1_500, 32ms
    {15, 33999, false, MANUAL_SPEED}, // S_1_250, 34ms
    {15, 40999, true, MANUAL_SPEED}, // S_1_125, 41ms
    {15, 47999, true, MANUAL_SPEED}, // S_1_60, 48 ms
    {15, 57999, true, MANUAL_SPEED}, // S_1_30, 58ms
    {31, 32999, true, MANUAL_SPEED}, // S_1_15, 66ms
    {31, 62499, true, MANUAL_SPEED}, // S_1_8, 125ms
    {63, 62499, true, MANUAL_SPEED}, // S_1_4, 250ms
    {127, 62499, true, MANUAL_SPEED}, // S_1_2, 500ms
    {255, 62499, true, MANUAL_SPEED}, // S_1, 1000ms
    {0,0, true, T_MODE},         // POST (handled separately)
    {0,0, true, B_MODE},         // POSB (handled separately)
    {0,0, true, AUTO_MODE},         // AUTO (handled separately)
    {0,0, true, AUTO_F_MODE}          // AUTO_F (handled separately)
};
