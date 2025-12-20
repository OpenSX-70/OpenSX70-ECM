#ifndef pollers_h
#define pollers_h

#include "peripheralport.h"
#include "meter.h"

typedef enum poller_state{
    STATE_POLL_DONGLE,
    STATE_POLL_METER,
    STATE_POLL_N
} poller_state;

void poll(void);
poller_state do_state_poll_dongle(void);
poller_state do_state_poll_meter(void);

#endif