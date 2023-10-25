#pragma once

#include "standard_task.hpp"
#include  "concurrent.hpp"
#include "events.h"

using utils::StandardTask;
using utils::Milliseconds;

namespace app {

	class GpioTask : public StandardTask {

	private:
		void handle(utils::Message&) override;
		void onStart() override;
		void onTimeout() override;
       	Milliseconds queueTimeout() override { return 10;};

	public:

		GpioTask(uint32_t priority) : StandardTask(priority) {}

		const char * name() const override { return "Gpio"; }

		bool isrGpioToggle(uint32_t pin_id)
		{
			return postFromISR(EVENT_GPIO_TOGGLE, pin_id);
		}

	};

}