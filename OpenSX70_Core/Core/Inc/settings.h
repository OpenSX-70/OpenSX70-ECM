#ifndef SETTINGS_H
#define SETTINGS_H
#include "main.h"


//      Feature toggles
#define SHUTTERDARKSLIDE 0       //1 Enables feature to not eject dark slide until shutter button is pressed
#define LIGHMETER_HELPER 1       //1 Enables viewfinder light meter helper
#define EJECT_AFTER_DEPRESSING 1 //1 Enables the user to hold the shutter button to prevent photo ejection


//----------------DONGLE SWITCH FEATURE SELECTION-------------------------
// 1 and 2 values assign features to switch 1 and 2, 0 means unused.
// Example values:
// #define MEXP_MODE 1   : MEXP_MODE on switch 1
// #define SELF_TIMER 2  : SELF_TIMER on switch 2
// #define {whatever} 0  : No switch assigned. 
// DO NOT ASSIGN MULTIPLE THINGS TO THE SAME VALUE (except 0).
// DOING SO WILL BREAK THINGS. YOU CANNOT HAVE MULTIPLE FUNCTIONS ASSIGNED TO THE SAME SWITCH.
// When I have a configuration style dongle set up I will be doing validation on that side.
// Configuration dongle may be difficult due to how flash writing works on the stm32. Have to erase and write a whole flash page.
// Until then, YOU will need to validate that you are not overloading a switch.

#define MEXP_MODE 1
#define SELF_TIMER 2

//----------------END DONGLE SWITCH FEATURE SELECTION---------------------

//---------------MAGIC NUMBERS---------------------------------------------
#define A100 4093 //less than max value as watchdog requires exceeding value to trigger
#define A600 1200
#define FD100 2809
#define FF100 3652
#define FD600 700
#define FF600 910
//---------------END MAGIC NUMBERS-----------------------------------------

//---------------Flashbar and Dongle Flash---------------------------------
#define Flash_Capture_Delay 4
//---------------End Flash settings----------------------------------------

//---------------METER SETTINGS--------------------------------------------
#define METER_OVERFLOW_THRESHOLD 2
//---------------END METER SETTINGS----------------------------------------

#define PERIPHERAL_TIMEOUT_MS 5

#define DEBOUNCE_DELAY 5

enum setting_type{
    MANUAL_SPEED,
    T_MODE,
    B_MODE,
    AUTO,
    AUTO_F,
};

enum dial_settings{
    S_1_2000 = 0,
    S_1_1000,
    S_1_500,
    S_1_250,
    S_1_125,
    S_1_60,
    S_1_30,
    S_1_15,
    S_1_8,
    S_1_4,
    S_1_2,
    S_1,
    POST, 
    POSB, 
    AUTO, 
    AUTO_F
};

extern const uint8_t POWER_DOWN_DELAY;
extern const uint8_t Y_DELAY;
extern const int32_t ShutterSpeed[]; //reduced speeds from 25 (slot5) to compensate flash firing

struct shutter_speed_timing{
    uint16_t prescaler;
    uint16_t period;
    bool flash_enabled;
};

extern struct shutter_speed_timing ShutterSpeedTiming[];

#define FLASH_USER_DATA_ADDR  (0x08000000 + 32*1024 - 2048)


#endif