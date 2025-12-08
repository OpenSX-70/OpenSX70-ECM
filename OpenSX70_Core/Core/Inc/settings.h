#ifndef SETTINGS_H
#define SETTINGS_H
#include "main.h"


//      Feature toggles
#define SHUTTERDARKSLIDE 0       //1 Enables feature to not eject dark slide until shutter button is pressed
#define TIMER_MIRROR_UP 1        //1 Enables mirror up feature for self timer modes
#define EIGHT_SHOT_PACK 1        //1 Makes all counter-based functions work based on an 8 shot pack rather than 10
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
// When I have a configurator style dongle set up I will be doing validation on that side.
// Until then, YOU will need to validate that you are not overloading a switch.

#define MEXP_MODE 0
#define SELF_TIMER 2
#define DONGLE_AUTO_FLASH 1
#define AUTO_STOP_DOWN 0

//----------------END DONGLE SWITCH FEATURE SELECTION---------------------

//---------------MAGIC NUMBERS---------------------------------------------
#define A100 1023
#define A600 255
#define FD100 200
#define FF100 260
#define FD600 40
#define FF600 52
//---------------END MAGIC NUMBERS-----------------------------------------

//---------------Flashbar and Dongle Flash---------------------------------
#define Flashbar_Change 1
#define Flash_Capture_Delay 4
#define Flash_Max_Time 56
#define Flash_Capture_Max_Time 12
#define Dongle_Flash_Limit 4
//---------------End Flash settings----------------------------------------

//---------------METER SETTINGS--------------------------------------------
#define AUTO_TIMEOUT 15000 //Sets the maximum time an auto exposure can take place. Required due to new meter design.
#define METER_INTERVAL 100 // Sets how long each meter measurement sample is taken in ms
#define METER_AUTO_WARNING 100 // If predicted ms is over this value, warning LED will shine in auto mode
#define METER_PREDICTION_OFFSET 20 // in ms. This gets added to the prediction. At f8 I noticed all meter predictions were around 20ms off
#define METER_SLOPE_HANDICAP 0 // Not used currently. Used to increase/decrease the slope of the prediction.
//#define METER_RESET_DELAY 2 // Time required for light meter capacitor to fully reset
//---------------END METER SETTINGS----------------------------------------

// Timeout constant for peripheral communication (5ms)
#define PERIPHERAL_TIMEOUT_MS 5
#define PERIPHERAL_RESPONSE_TIMEOUT_TICKS 1000  // Assuming ~1ms per state machine tick, 1 second timeout


enum positions_t {POST = -100, POSB, AUTO600, AUTO100};//ANALOGUEWORKS AUTO 600BW

extern const uint8_t POWER_DOWN_DELAY;
extern const uint8_t Y_DELAY;
extern const uint8_t METER_RESET_DELAY;
extern const int32_t ShutterSpeed[]; //reduced speeds from 25 (slot5) to compensate flash firing


#endif