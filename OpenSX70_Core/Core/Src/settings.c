#include "settings.h"
#include "meter.h"
#include <stdint.h>

const uint8_t Y_DELAY = 40;
const uint8_t POWER_DOWN_DELAY = 28;

const int32_t ShutterSpeed[] = {     23,   27,   32,    34,  39,   44,  54,   66,  125,  250, 500, 1000, POST, POSB, AUTO, AUTO_F};
//                                  2000  1000   500    250  125    60   30    15     8     4    2     1