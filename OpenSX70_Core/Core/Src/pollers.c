#include "pollers.h"
#include "peripheralport.h"

typedef poller_state (*poller_state_funct)(void);

poller_state do_state_poll_dongle(void);
poller_state do_state_poll_meter(void);

static const poller_state_funct POLLER_MACHINE [STATE_POLL_N] = {
    &do_state_poll_dongle,
    &do_state_poll_meter
};

poller_state poller = STATE_POLL_DONGLE;

void poll(){
    poller = POLLER_MACHINE[poller]();
}

poller_state do_state_poll_dongle(){
    update_peripheral_status(&current_dongle_state);
    return STATE_POLL_METER;
}

poller_state do_state_poll_meter(){

    return STATE_POLL_DONGLE;
}