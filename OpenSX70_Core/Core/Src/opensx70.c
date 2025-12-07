#include "opensx70.h"

typedef camera_state (*camera_state_funct)(void);

camera_state do_state_darkslide (void);
camera_state do_state_noDongle (void);
//camera_state do_state_dongle (void);
camera_state do_state_flashBar (void);
//camera_state do_state_multi_exp (void);

static const camera_state_funct STATE_MACHINE [STATE_N] = {
    &do_state_darkslide,
    &do_state_noDongle,
//    &do_state_dongle,
    &do_state_flashBar,
//    &do_state_multi_exp
};

//Default state
camera_state state = STATE_DARKSLIDE;

camera_state do_state_darkslide (void){

    //Stay in darkslide until dongle or flashbar detected
    return STATE_DARKSLIDE;
}

camera_state do_state_noDongle (void){

    //Stay in no dongle until darkslide detected
    return STATE_NODONGLE;
}

camera_state do_state_flashBar (void){

    //Stay in flashbar until darkslide detected
    return STATE_FLASHBAR;
}