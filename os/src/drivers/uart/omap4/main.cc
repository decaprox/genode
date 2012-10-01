/*
 * \brief  Driver for OMAP4 UARTs
 * \author Ivan Loskutov <ivan.loskutov@ksyslabs.org>
 * \date   2012-09-28
 */

/*
 * Copyright (C) 2012 Ksys Labs LLC
 * Copyright (C) 2011-2012 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/* Genode includes */
#include <base/printf.h>
#include <base/sleep.h>
#include <os/config.h>
#include <cap_session/connection.h>

#include <uart_defs.h>

/* local includes */
#include "omap_uart.h"
#include "terminal_component.h"


int main(int argc, char **argv)
{
	using namespace Genode;

	PDBG("--- OMAP4 UART driver started ---\n");

	/**
	 * Factory used by 'Terminal::Root' at session creation/destruction time
	 */
	struct Omap_uart_driver_factory : Terminal::Driver_factory
	{
		Omap_uart *created[UARTS_NUM];

		/**
		 * Constructor
		 */
		Omap_uart_driver_factory()
		{
			for (unsigned i = 0; i < UARTS_NUM; i++)
				created[i] = 0;
		}

		Terminal::Driver *create(unsigned index, unsigned baudrate,
								 Terminal::Char_avail_callback &callback)
		{
			if (index > UARTS_NUM)
				throw Terminal::Driver_factory::Not_available();

			if (baudrate == 0)
			{
				PDBG("Baudrate is not defined. Use default 115200");
				baudrate = 115200;
			}

			Omap_uart_cfg *cfg  = &omap_uart_cfg[index];
			Omap_uart     *uart =  created[index];

			if (!uart) {
				uart = new (env()->heap())
					Omap_uart(cfg->mmio_base, cfg->mmio_size, cfg->irq_number, baudrate, callback);

				/* update 'created' table */
				created[index] = uart;
			}

			return uart;
		}

		void destroy(Terminal::Driver *driver) { /* TODO */ }

	} driver_factory;

	enum { STACK_SIZE = 0x2000 };
	static Cap_connection cap;
	static Rpc_entrypoint ep(&cap, STACK_SIZE, "uart_ep");

	static Terminal::Root uart_root(&ep, env()->heap(), driver_factory);
	env()->parent()->announce(ep.manage(&uart_root));

	sleep_forever();
	return 0;
}
