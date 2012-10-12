/*
 * \brief  Driver for TDA8029 smart card reader
 * \author Ivan Loskutov <ivan.loskutov@ksyslabs.org>
 * \date   2012-10-13
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
#include <cap_session/connection.h>

#include "driver.h"
#include "smartcard_component.h"


int main(int, char **)
{
	using namespace Genode;

	PDBG("--- tda8029 driver ---\n");

	Driver       driver;

	/*
	 * Initialize server entry point
	 */
	enum { STACK_SIZE = 4096 };
	static Cap_connection cap;
	static Rpc_entrypoint ep(&cap, STACK_SIZE, "smartcard_ep");

	/*
	 * Let the entry point serve the gpio session and root interfaces
	 */
	static Smartcard::Root smartcard_root(&ep, env()->heap(), driver);

	/*
	 * Announce service
	 */
	env()->parent()->announce(ep.manage(&smartcard_root));

	Genode::sleep_forever();
	return 0;
}