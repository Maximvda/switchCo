#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include "can_driver.h"
#include "config.h"
#include "device.h"

#define RX_TASK_PRIO 8

extern "C" {
	void app_main();
}

void app_main(void)
{
	config::init();
	device::init();
    xTaskCreate(driver::gpio::task, "gpio_task", 2048, NULL, 1, NULL);
	xTaskCreatePinnedToCore(driver::can::receive_task, "CAN_rx", 4096, NULL, RX_TASK_PRIO, NULL, tskNO_AFFINITY);
}
