/*
 * \brief  Gpio driver for the am33xx
 * \author Ivan Loskutov <ivan.loskutov@ksyslabs.org>
 * \author Stefan Kalkowski <stefan.kalkowski@genode-labs.com>
 * \date   2012-06-23
 */

/*
 * Copyright (C) 2012 Ksys Labs LLC
 * Copyright (C) 2012-2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/* Genode includes */
#include <base/printf.h>
#include <base/sleep.h>
#include <cap_session/connection.h>
#include <gpio/component.h>
#include <gpio/config.h>

/* local includes */
#include <driver.h>


Omap_driver::Gpio_bank Omap_driver::_gpio_bank[Genode::Board_base::GPIO_BANKS] = {
	Gpio_bank(Genode::Board_base::GPIO0_MMIO_BASE,
	          Genode::Board_base::GPIO0_MMIO_SIZE,
	          Genode::Board_base::GPIO0_IRQ),
 	Gpio_bank(Genode::Board_base::GPIO1_MMIO_BASE,
	          Genode::Board_base::GPIO1_MMIO_SIZE,
	          Genode::Board_base::GPIO1_IRQ),
	Gpio_bank(Genode::Board_base::GPIO2_MMIO_BASE,
	          Genode::Board_base::GPIO2_MMIO_SIZE,
	          Genode::Board_base::GPIO2_IRQ),
	Gpio_bank(Genode::Board_base::GPIO3_MMIO_BASE,
	          Genode::Board_base::GPIO3_MMIO_SIZE,
	          Genode::Board_base::GPIO3_IRQ),
};

int main(int, char **)
{
	using namespace Genode;

	printf("--- am33xx gpio driver ---\n");

	Omap_driver &driver = Omap_driver::factory();
	Gpio::process_config(driver);

	/*
	 * Initialize server entry point
	 */
	enum { STACK_SIZE = 4096 };
	static Cap_connection cap;
	Sliced_heap sliced_heap(env()->ram_session(), env()->rm_session());
	static Rpc_entrypoint ep(&cap, STACK_SIZE, "gpio_ep");
	static Gpio::Root gpio_root(&ep, &sliced_heap, driver);

	/*
	 * Announce service
	 */
	env()->parent()->announce(ep.manage(&gpio_root));

	Genode::sleep_forever();
	return 0;
}

