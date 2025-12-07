#ifndef opensx70_h
#define opensx70_h

typedef enum{
    STATE_DARKSLIDE,
    STATE_NODONGLE,
//    STATE_DONGLE,
    STATE_FLASHBAR,
//    STATE_MULTI_EXP,
    STATE_N
} camera_state;

camera_state do_state_darkslide(void);
camera_state do_state_noDongle(void);
//camera_state do_state_dongle(void);
camera_state do_state_flashBar(void);
//camera_state do_state_multi_exp(void);

#endif