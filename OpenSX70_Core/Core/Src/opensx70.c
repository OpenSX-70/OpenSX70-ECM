#include "opensx70.h"
#include "meter.h"

// Flash storage address (last page for user data)
#define FLASH_USER_DATA_ADDR  (0x08000000 + 32*1024 - 2048)

typedef camera_state (*camera_state_funct)(void);

camera_state do_state_darkslide (void);
camera_state do_state_noDongle (void);
camera_state do_state_dongle (void);
camera_state do_state_flashBar (void);
camera_state do_state_multi_exp (void);

static const camera_state_funct STATE_MACHINE [STATE_N] = {
    &do_state_init,
    &do_state_darkslide,
    &do_state_noDongle,
    &do_state_dongle,
    &do_state_flashBar,
    &do_state_multi_exp
};

//Default state
camera_state state = STATE_INIT;

void opensx70_run_state_machine (void){
    state = STATE_MACHINE[state]();
}

camera_state do_state_init (void){
    solenoid_init();
    integrator_init();
    initializePeripheralDevice(&current_dongle_state);
    if(HAL_GPIO_ReadPin(S5_GPIO_Port, S5_Pin) != GPIO_PIN_RESET){
        shutter_close();
        mirror_down();
        shutter_open();
    }

    //TODO! S1 ISO swap call goes here
    return STATE_DARKSLIDE;
}

camera_state do_state_darkslide (void){

    //Stay in darkslide until dongle or flashbar detected
    return return_state(&current_dongle_state);
}

camera_state do_state_noDongle (void){

    //Stay in no dongle until darkslide detected
    return return_state(&current_dongle_state);
}

camera_state do_state_flashBar (void){

    //Stay in flashbar until darkslide detected
    return return_state(&current_dongle_state);
}

camera_state do_state_dongle (void){

    //Stay in dongle until darkslide detected
    return return_state(&current_dongle_state);
}

camera_state do_state_multi_exp (void){

    //Stay in multi exp until darkslide detected
}

camera_state return_state(peripheral_device *device){
    switch(device->type){
        case PERIPHERAL_NONE:
            return STATE_NODONGLE;
        case PERIPHERAL_DONGLE:
            if(get_switch_state(MEXP_MODE)){
                //multipleExposureMode = true;
                //mEXPFirstRun = true;
                return STATE_MULTI_EXP;
            }
            return STATE_DONGLE;
        case PERIPHERAL_FLASHBAR:
            return STATE_FLASHBAR;
        default:
            return STATE_NODONGLE;
    }
}

void ISOBlink(){
    /*
    switch(savedISO){
        case ISO_600:
            for(uint8_t i=0; i<2; i++){
                sendCommand(BLUE_ON);
                digitalWrite(PIN_LED2, HIGH);
                delay(100);
                sendCommand(BLUE_OFF);
                digitalWrite(PIN_LED2, LOW);
                delay(100);
            }
            break;
        case ISO_SX70:
            for(uint8_t i=0; i<2; i++){
                sendCommand(RED_ON);
                digitalWrite(PIN_LED1, HIGH);
                delay(100);
                sendCommand(RED_OFF);
                digitalWrite(PIN_LED1, LOW);
                delay(100);
            }
            break;
    }
    */
}

void save_iso(meter_iso iso) {
    HAL_FLASH_Unlock();
    
    FLASH_EraseInitTypeDef eraseInit = {
        .TypeErase = FLASH_TYPEERASE_PAGES,
        .Page = (FLASH_USER_DATA_ADDR - 0x08000000) / FLASH_PAGE_SIZE,
        .NbPages = 1
    };
    uint32_t pageError;
    HAL_FLASHEx_Erase(&eraseInit, &pageError);
    
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, FLASH_USER_DATA_ADDR, (uint64_t)iso);
    
    HAL_FLASH_Lock();
}

meter_iso read_iso(void) {
    uint32_t data = *(uint32_t*)FLASH_USER_DATA_ADDR;

    if (data != ISO_640 && data != ISO_125) {
        return ISO_640;  // Default to ISO_640 if invalid
    }
    return (meter_iso)data;
}