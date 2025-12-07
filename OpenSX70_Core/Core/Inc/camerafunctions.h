#ifndef CAMERAFUNCTIONS_H
#define CAMERAFUNCTIONS_H  

#include "main.h"
extern TIM_HandleTypeDef htim1;

void shutter_close(void);
void shutter_open(void);
void sol2_engage(void);
void sol2_disengage(void);
void sol2_lower(void);
void mirror_down(void);
void mirror_up(void);
void darkslide_eject(void);
#endif