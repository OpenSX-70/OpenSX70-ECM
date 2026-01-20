#include "pollers.h"
#include "peripheralport.h"

typedef poller_state (*poller_state_funct)(void);

poller_state do_state_poll_wait(void);
poller_state do_state_poll_dongle(void);
poller_state do_state_poll_meter(void);

static const poller_state_funct POLLER_MACHINE [STATE_POLL_N] = {
    &do_state_poll_wait,
    &do_state_poll_dongle,
    &do_state_poll_meter
};

volatile bool init_complete = false;
poller_state poller = STATE_POLL_WAIT;

void poll(){
    poller = POLLER_MACHINE[poller]();
}

poller_state do_state_poll_wait(){
    if(init_complete){
        return STATE_POLL_DONGLE;
    }
    return STATE_POLL_WAIT;
}

poller_state do_state_poll_dongle(){
    update_peripheral_status(&current_dongle_state);
    return STATE_POLL_METER;
}

poller_state do_state_poll_meter(){
    if(!LIGHMETER_HELPER){
        return STATE_POLL_DONGLE;
    }
    switch(current_dongle_state.type){
        case PERIPHERAL_NONE:
            approximate_exposure_time(LOW_LIGHT);
            break;
        case PERIPHERAL_DONGLE:
            approximate_exposure_time(OFF);
            break;
        case PERIPHERAL_FLASHBAR:
            approximate_exposure_time(OFF);
            break;
        default:
            break;
    }
    return STATE_POLL_DONGLE;
}