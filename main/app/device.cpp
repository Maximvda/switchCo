#include "device.hpp"

using app::Device;

static const char* TAG = {"Device"};

void Device::init()
{
    config_driver_.init();
    id_ = config_driver_.deviceId();
}

void Device::handleMessage(GincoMessage& message)
{

}

// void Device::toggle_switch(uint8_t switch_id){
//     inputs_[switch_id].toggle();
// }

// void Device::on_can_message(driver::can::message_t can_mes){
//     // Not the id of this module so immediately return
//     // TODO: Configure can driver to filter these messages out!
//     if (can_mes.id != id)
//         return;

//     switch(can_mes.feature_type){
//         case 1:
//             outputs[can_mes.index].handle_message(can_mes);
//             break;
//         case 3:
//             inputs[can_mes.index].handle_message(can_mes);
//             break;
//         default:
//             break;
//     }
// }