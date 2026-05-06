#include "opensx70.h"

meter_iso savedISO;

volatile bool isoBlinked = false;
bool multiple_exposure_first_run = true;

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

camera_state state = STATE_INIT;

void opensx70_run_state_machine (void){
    state = STATE_MACHINE[state]();
    sonar_focus();
}

camera_state do_state_init (void){
    savedISO = read_iso();
    solenoid_init();
    integrator_init(&savedISO);
    initialize_peripheral_device(&current_dongle_state);
    HAL_TIM_Base_Start_IT(&htim14);
    __HAL_ADC_DISABLE_IT(&hadc1, ADC_IT_AWD1);
    HAL_GPIO_WritePin(LM_RESET_GPIO_Port, LM_RESET_Pin, 1);
    if(HAL_GPIO_ReadPin(S5_GPIO_Port, S5_Pin)){
        shutter_close();
        mirror_down();
        shutter_open();
    }
    s1_iso_swap();
    return STATE_DARKSLIDE;
}

camera_state do_state_darkslide (void){
    camera_state next_state = STATE_DARKSLIDE;
    if (HAL_GPIO_ReadPin(S8_GPIO_Port, S8_Pin) && !HAL_GPIO_ReadPin(S9_GPIO_Port, S9_Pin)){
        #if SHUTTERDARKSLIDE
        if (HAL_GPIO_ReadPin(S1T_GPIO_Port, S1T_Pin) == GPIO_PIN_SET){
        #endif
            darkslide_eject();
            next_state = return_state(&current_dongle_state);
        #if SHUTTERDARKSLIDE
        }
        #endif
    }
    else{
        next_state = return_state(&current_dongle_state);
    }

    if(!isoBlinked){
        ISOBlink(&savedISO);
    } 
    init_complete = true;
    return next_state;
}

camera_state do_state_noDongle (void){
    if(HAL_GPIO_ReadPin(S1T_GPIO_Port, S1T_Pin) == GPIO_PIN_SET){
        HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);

        begin_exposure();
        auto_exposure(&savedISO);
    }
    return return_state(&current_dongle_state);
}

camera_state do_state_flashBar (void){
    if(HAL_GPIO_ReadPin(S1T_GPIO_Port, S1T_Pin) == GPIO_PIN_SET){
        begin_exposure();
        auto_exposure_flashbar(&savedISO);
    }
    return return_state(&current_dongle_state);
}

camera_state do_state_dongle (void){
    if(HAL_GPIO_ReadPin(S1T_GPIO_Port, S1T_Pin) == GPIO_PIN_SET){
        if(get_switch_state(SELF_TIMER)){
            self_timer();
        }
        begin_exposure();
        dongle_functions();
    }

    return return_state(&current_dongle_state);
}

camera_state do_state_multi_exp (void){
    bool mexpSwitchStatus = get_switch_state(MEXP_MODE);

    if(HAL_GPIO_ReadPin(S1T_GPIO_Port, S1T_Pin) == GPIO_PIN_SET){
        if(mexpSwitchStatus){
            if(get_switch_state(SELF_TIMER)){
                self_timer();
            }
            if(multiple_exposure_first_run){
                multiple_exposure_first_run = false;
                begin_exposure();
            }
            dongle_functions();
        }
        else if(!mexpSwitchStatus && !multiple_exposure_first_run){
            multiple_exposure_flag = false;
            exposure_finish();
            return STATE_DONGLE;
        }
    }

    if(!mexpSwitchStatus && multiple_exposure_first_run){
        multiple_exposure_flag = false;
        return STATE_DONGLE;
    }

    return STATE_MULTI_EXP;
}

camera_state return_state(peripheral_device *device){
    switch(device->type){
        case PERIPHERAL_NONE:
            return STATE_NODONGLE;
        case PERIPHERAL_DONGLE:

            if(get_switch_state(MEXP_MODE)){
                multiple_exposure_flag = true;
                multiple_exposure_first_run = true;
                return STATE_MULTI_EXP;
            }
            return STATE_DONGLE;
        case PERIPHERAL_FLASHBAR:
            return STATE_FLASHBAR;
        default:
            return STATE_NODONGLE;
    }
}

void dongle_functions(void){
    switch (ShutterSpeedTiming[current_dongle_state.selector].type)
    {
        case MANUAL_SPEED:
            manual_exposure(ShutterSpeedTiming[current_dongle_state.selector]);
            break;
        case T_MODE:
            time_mode();
            break;
        case B_MODE:
            bulb_mode();
            break;
        case AUTO_MODE:
            auto_exposure(&savedISO);
            break;
        case AUTO_F_MODE:
            auto_exposure_flashbar(&savedISO);
            break;
        default:
            break;
    }
}

void self_timer(void){
    HAL_GPIO_WritePin(S1F_FBW_GPIO_Port, S1F_FBW_Pin, GPIO_PIN_RESET);
    send_command(PERIPHERAL_SELF_TIMER_CMD);
    HAL_Delay(4000);
    shutter_close();
    HAL_Delay(2000);
    mirror_up();
    HAL_Delay(1000);
    HAL_GPIO_WritePin(S1F_FBW_GPIO_Port, S1F_FBW_Pin, GPIO_PIN_SET);
    HAL_Delay(3000);
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
    savedISO = iso;
}

meter_iso read_iso(void) {
    uint32_t data = *(uint32_t*)FLASH_USER_DATA_ADDR;

    if (data != ISO_640 && data != ISO_125) {
        return ISO_640;
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
                savedISO = ISO_640;
                break;
        }
        save_iso(newISO);
        ISOBlink(&savedISO);
        isoBlinked = true;
    }
    while(HAL_GPIO_ReadPin(S1T_GPIO_Port, S1T_Pin) == GPIO_PIN_SET);
}