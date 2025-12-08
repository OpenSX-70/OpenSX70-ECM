#include "opensx70.h"
#include "camerafunctions.h"
#include "main.h"
#include "stm32g0xx_hal_gpio.h"

meter_iso savedISO;
volatile bool isoBlinked = false;

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
    savedISO = read_iso();
    solenoid_init();
    integrator_init();
    initializePeripheralDevice(&current_dongle_state);
    if(HAL_GPIO_ReadPin(S5_GPIO_Port, S5_Pin) != GPIO_PIN_RESET){
        shutter_close();
        mirror_down();
        shutter_open();
    }

    s1_iso_swap();
    return STATE_DARKSLIDE;
}

camera_state do_state_darkslide (void){
    #if SHUTTERDARKSLIDE
    if (HAL_GPIO_ReadPin(S1T_GPIO_Port, S1T_Pin) == GPIO_PIN_SET){
    #endif
        if ((HAL_GPIO_ReadPin(S8_GPIO_Port, S8_Pin) == GPIO_PIN_SET) && (HAL_GPIO_ReadPin(S9_GPIO_Port, S9_Pin) == GPIO_PIN_RESET)){
            darkslide_eject();        
        }

    #if SHUTTERDARKSLIDE
    }
    #endif
    if(!isoBlinked){
        ISOBlink(&savedISO);
    }

    return return_state(&current_dongle_state);
}

camera_state do_state_noDongle (void){

    return return_state(&current_dongle_state);
}

camera_state do_state_flashBar (void){

    return return_state(&current_dongle_state);
}

camera_state do_state_dongle (void){

    return return_state(&current_dongle_state);
}

camera_state do_state_multi_exp (void){

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

void ISOBlink(meter_iso *savedISO){
    switch(*savedISO){
        case ISO_640:
            for(uint8_t i=0; i<2; i++){
                send_command(BLUE_ON);
                HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
                HAL_Delay(100);
                send_command(BLUE_OFF);
                HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
                HAL_Delay(100);
            }
            break;
        case ISO_125:
            for(uint8_t i=0; i<2; i++){
                send_command(RED_ON);
                HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
                HAL_Delay(100);
                send_command(RED_OFF);
                HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
                HAL_Delay(100);
            }
            break;
    }
}

void save_iso(meter_iso *iso) {
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
    savedISO = *iso;
}

meter_iso read_iso(void) {
    uint32_t data = *(uint32_t*)FLASH_USER_DATA_ADDR;

    if (data != ISO_640 && data != ISO_125) {
        return ISO_640;
        //Debug output to indicate no valid ISO found in flash
    }
    return (meter_iso)data;
}

void s1_iso_swap(void){
    meter_iso currentISO = savedISO;
    if(HAL_GPIO_ReadPin(S1T_GPIO_Port, S1T_Pin) == GPIO_PIN_SET){
        meter_iso newISO;
        switch(currentISO){
            case ISO_640:
                newISO = ISO_125;
                break;
            case ISO_125:
                newISO = ISO_640;
                break;
            default:
                newISO = ISO_640;
                break;
        }
        save_iso(&newISO);
        ISOBlink(&savedISO);
        isoBlinked = true;
    }
    while(HAL_GPIO_ReadPin(S1T_GPIO_Port, S1T_Pin) == GPIO_PIN_SET);
}