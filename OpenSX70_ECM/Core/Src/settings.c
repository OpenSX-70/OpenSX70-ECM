#include "settings.h"
#include "meter.h"
#include <stdint.h>

const uint8_t Y_DELAY = 40;
const uint8_t POWER_DOWN_DELAY = 28;

// For easily generating prescaler and period values for the times you want, use this calculator
// https://deepbluembedded.com/stm32-timer-calculator/
// set your desired length of time (Timer Interrupt Interval), the clock frequency (16mhz for the default OpenSX70 config), 
// and leave the register width set to 16 bits.
// ARR = period, PSC = prescaler
#if FUZZY_MANUAL_MODE
struct fuzzy_shutter_speed_timing FuzzyShutterSpeedTiming[] = {
    {.min_prescaler = 7, .min_period = 43999, .max_prescaler = 7, .max_period = 49999, .flash_enabled = false, .type = MANUAL_SPEED}, //23, 22-25ms
    {.min_prescaler = 7, .min_period = 49999, .max_prescaler = 7, .max_period = 58999, .flash_enabled = false, .type = MANUAL_SPEED}, //27, 25 - 29.5
    {.min_prescaler = 7, .min_period = 58999, .max_prescaler = 15, .max_period = 32999, .flash_enabled = false, .type = MANUAL_SPEED}, //32, 29.5 - 33
    {.min_prescaler = 15, .min_period = 32999, .max_prescaler = 15, .max_period = 37499, .flash_enabled = false, .type = MANUAL_SPEED}, //34, 33 - 37.5
    {.min_prescaler = 15, .min_period = 37499, .max_prescaler = 15, .max_period = 44499, .flash_enabled = true, .type = MANUAL_SPEED}, //41, 37.5 - 44.5
    {.min_prescaler = 15, .min_period = 44499, .max_prescaler = 15, .max_period = 52999, .flash_enabled = true, .type = MANUAL_SPEED}, //48, 44.5 - 53
    {.min_prescaler = 15, .min_period = 52999, .max_prescaler = 15, .max_period = 61999, .flash_enabled = true, .type = MANUAL_SPEED}, //58, 53 - 62
    {.min_prescaler = 15, .min_period = 61999, .max_prescaler = 31, .max_period = 39999, .flash_enabled = true, .type = MANUAL_SPEED}, //66, 62 - 80
    {.min_prescaler = 31, .min_period = 49999, .max_prescaler = 47, .max_period = 49999, .flash_enabled = true, .type = MANUAL_SPEED}, //125, 100 - 150
    {.min_prescaler = 63, .min_period = 56249, .max_prescaler = 79, .max_period = 54999, .flash_enabled = true, .type = MANUAL_SPEED}, //250, 225 - 275
    {.min_prescaler = 127, .min_period = 59374, .max_prescaler = 143, .max_period = 58332, .flash_enabled = true, .type = MANUAL_SPEED}, //500, 475 - 525
    {.min_prescaler = 239, .min_period = 64999, .max_prescaler = 255, .max_period = 64061, .flash_enabled = true, .type = MANUAL_SPEED}, //1000, 975 - 1025
    {.min_prescaler = 0, .min_period = 0, .max_prescaler = 0, .max_period = 0, .flash_enabled = true, .type = T_MODE},
    {.min_prescaler = 0, .min_period = 0, .max_prescaler = 0, .max_period = 0, .flash_enabled = true, .type = B_MODE},
    {.min_prescaler = 0, .min_period = 0, .max_prescaler = 0, .max_period = 0, .flash_enabled = true, .type = AUTO_MODE},
    {.min_prescaler = 0, .min_period = 0, .max_prescaler = 0, .max_period = 0, .flash_enabled = true, .type = AUTO_F_MODE}
};
#endif
struct shutter_speed_timing ShutterSpeedTiming[] = {
    {.prescaler = 7, .period = 45999, .flash_enabled = false, .type = MANUAL_SPEED},  // S_1_2000, 23ms
    {.prescaler = 7, .period = 53999, .flash_enabled = false, .type = MANUAL_SPEED},  // S_1_1000, 27ms
    {.prescaler = 7, .period = 61999, .flash_enabled = false, .type = MANUAL_SPEED},  // S_1_500, 32ms
    {.prescaler = 15, .period = 33999, .flash_enabled = false, .type = MANUAL_SPEED}, // S_1_250, 34ms
    {.prescaler = 15, .period = 40999, .flash_enabled = true, .type = MANUAL_SPEED},  // S_1_125, 41ms
    {.prescaler = 15, .period = 47999, .flash_enabled = true, .type = MANUAL_SPEED},  // S_1_60, 48 ms
    {.prescaler = 15, .period = 57999, .flash_enabled = true, .type = MANUAL_SPEED},  // S_1_30, 58ms
    {.prescaler = 31, .period = 32999, .flash_enabled = true, .type = MANUAL_SPEED},  // S_1_15, 66ms
    {.prescaler = 31, .period = 62499, .flash_enabled = true, .type = MANUAL_SPEED},  // S_1_8, 125ms
    {.prescaler = 63, .period = 62499, .flash_enabled = true, .type = MANUAL_SPEED},  // S_1_4, 250ms
    {.prescaler = 127, .period = 62499, .flash_enabled = true, .type = MANUAL_SPEED}, // S_1_2, 500ms
    {.prescaler = 255, .period = 62499, .flash_enabled = true, .type = MANUAL_SPEED}, // S_1, 1000ms
    {.prescaler = 0, .period =0, .flash_enabled = true, .type = T_MODE},              // POST (handled separately)
    {.prescaler = 0, .period =0, .flash_enabled = true, .type = B_MODE},              // POSB (handled separately)
    {.prescaler = 0, .period =0, .flash_enabled = true, .type = AUTO_MODE},           // AUTO (handled separately)
    {.prescaler = 0, .period =0, .flash_enabled = true, .type = AUTO_F_MODE}          // AUTO_F (handled separately)
};
