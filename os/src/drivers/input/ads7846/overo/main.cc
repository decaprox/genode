/**
 * \brief  Input driver for ADS7846
 * \author Ivan Loskutov
 * \date   2012-02-16
 */

/*
 * Copyright 2012 Ksys Labs LLC 
 * Contact: <ivan.loskutov@ksyslabs.org>
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/* Genode */
#include <base/env.h>
#include <base/printf.h>
#include <base/sleep.h>
#include <input/component.h>
#include <cap_session/connection.h>

#include <ads7846_defs.h>

#include "ads7846.h"
#include "irq_handler.h"

using namespace Genode;

static Event_queue ev_queue;


namespace Input {

	/*
	 * Event handling is disabled on queue creation and will be enabled later if a
	 * session is created.
	 */
	void event_handling(bool enable)
	{
		if (enable)
			ev_queue.enable();
		else
			ev_queue.disable();
	}

	bool event_pending() { return !ev_queue.empty(); }
	Event get_event() { return ev_queue.get(); }
}


int main(int argc, char **argv)
{
	printf("--- ADS7846 input driver ---\n");

	ADS7846     ads7846(ev_queue, ADS7846_IRQ);

	Irq_handler ads7846_irq(ADS7846_IRQ, ads7846);

	/*
	 * Initialize server entry point
	 */
	enum { STACK_SIZE = 4096 };
	static Cap_connection cap;
	static Rpc_entrypoint ep(&cap, STACK_SIZE, "ads7846_ep");

	/*
	 * Let the entry point serve the input root interface
	 */
	static Input::Root input_root(&ep, env()->heap());
	env()->parent()->announce(ep.manage(&input_root));

	Genode::sleep_forever();
	return 0;
}
