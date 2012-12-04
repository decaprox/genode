/*
 * \brief  Input driver for TS2046
 * \author Ivan Loskutov <ivan.loskutov@ksyslabs.org>
 * \date   2012-08-01
 */

/*
 * Copyright (C) 2012 Ksys Labs LLC
 * Copyright (C) 2012 Genode Labs GmbH
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


#include "driver.h"
#include "signal_handler.h"

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
	PDBG("--- TSC2046 input driver ---\n");

	Driver         driver(ev_queue, Driver::CHIPSEE_TS_GPIO);
	Signal_handler sig_handler(driver);
	/*
	 * Initialize server entry point
	 */
	enum { STACK_SIZE = 4096 };
	static Cap_connection cap;
	static Rpc_entrypoint ep(&cap, STACK_SIZE, "tsc2046_ep");

	/*
	 * Let the entry point serve the input root interface
	 */
	static Input::Root input_root(&ep, env()->heap());
	env()->parent()->announce(ep.manage(&input_root));

	Genode::sleep_forever();
	return 0;
}
