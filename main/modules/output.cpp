#include "output.h"
#include "gpio.h"

#include <string>

#include "config.h"
#include "esp_log.h"
#include "can_driver.h"


const static char* TAG = {"Output"};

Output::Output(){};

Output::Output(uint8_t new_id){
    id = new_id;
    char config_key[10] = {"outputx"};
    sprintf(config_key, "output%i", new_id);
    if (config::get_key(config_key) == 1){
        pwm = true;
        ESP_LOGI(TAG, "Output %i is configured as pwm", id);
    }
    // Update state to last state
    sprintf(config_key, "pstate%i", new_id);
    uint8_t value = config::get_key(config_key);
    driver::gpio::set_output(id, value);
    state = value;
}

void Output::handle_message(driver::can::message_t can_mes){
    switch(can_mes.function_address){
        case 0: // Turn off
            set_state(false);
            break;
        case 1: // Turn on
            can_mes.data ? set_pwm(can_mes.data) : set_state(true);
            break;
        case 2: // Toggle
            toggle();
            break;
        case 3: // Run an effect
            run_effect(static_cast<output::Effect>(can_mes.data));
            break;
        case 0xFF: // Request state (doing in end of call)
            break;
        default:
            break;
    }
    // Always aknowledge and return state
    can_mes.ack = true;
    driver::can::transmit(can_mes, 1, &brightness);
}

void Output::set_pwm(uint32_t level){
    // Not in pwm mode so switch this
    if (!pwm){
        driver::gpio::change_output(id, true, level);
        pwm = true;
    }
    brightness = level;
    uint32_t value = 32 * static_cast<float>(level); // 8191 / 256 = 32
    driver::gpio::set_level(id, value);
}

void Output::set_state(bool on){
    state = on;
    uint32_t value = state ? 1 : 0;
    brightness = state ? 255 : 0;
    if (pwm){
        driver::gpio::change_output(id, false, value);
    }
    driver::gpio::set_output(id, value);
}

void Output::toggle(){
    set_state(!state);
}

void Output::run_effect(output::Effect effect){
    ESP_LOGW(TAG, "effects still to be implemented");
}