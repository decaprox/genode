/*
 * \brief  Gpio driver for the OMAP4
 * \author Ivan Loskutov <ivan.loskutov@ksyslabs.org>
 * \date   2012-06-23
 */

/*
 * Copyright (C) 2012 Ksys Labs LLC
 * Copyright (C) 2012 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/* Genode includes */
#include <gpio_session/gpio_session.h>
#include <cap_session/connection.h>
#include <dataspace/client.h>
#include <base/printf.h>
#include <base/sleep.h>
#include <root/component.h>

/* local includes */
#include "driver.h"
#include "irq_handler.h"


/**
 * Root interface that hands out a statically created session
 */
template <typename SESSION_TYPE>
class Static_root : public Genode::Rpc_object<Genode::Typed_root<SESSION_TYPE> >
{
	private:

		typedef Genode::Capability<SESSION_TYPE> Session_capability;

		Session_capability _session;

	public:

		/**
		 * Constructor
		 *
		 * \param session  session to be provided to the client
		 */
		Static_root(Session_capability session) : _session(session) { }


		/********************
		 ** Root interface **
		 ********************/

		Genode::Session_capability session(Genode::Root::Session_args const &args) { return _session; }
		void upgrade(Genode::Session_capability, Genode::Root::Upgrade_args const &) { }
		void close(Genode::Session_capability) { }
};


namespace Gpio {
	using namespace Genode;
	class Session_component;
};


class Gpio::Session_component : public Genode::Rpc_object<Gpio::Session>
{
	private:

		Driver &_driver;

		Signal_context_capability _read_avail_sigh;

	public:
		Session_component(Driver &driver)
		:
			_driver(driver)
		{
		}

		/************************************
		 ** Gpio::Session interface **
		 ************************************/

		void set_direction_output(int gpio, bool enable)
		{
			_driver.set_gpio_dataout(gpio, enable);
			_driver.set_gpio_direction(gpio, false);
		}

		void set_direction_input(int gpio)
		{
			_driver.set_gpio_direction(gpio, true);
		}

		void set_dataout(int gpio, bool enable)
		{
			_driver.set_gpio_dataout(gpio, enable);
		}

		int get_datain(int gpio)
		{
			return _driver.get_gpio_datain(gpio);
		}

		void set_debounce_enable(int gpio, bool enable)
		{
			_driver.set_gpio_debounce_enable(gpio, enable);
		}

		void set_debouncing_time(int gpio, unsigned int us)
		{
			_driver.set_gpio_debouncing_time(gpio, us);
		}

		void set_falling_detect(int gpio, bool enable)
		{
			_driver.set_gpio_falling_detect(gpio, enable);
		}

		void set_rising_detect(int gpio, bool enable)
		{
			_driver.set_gpio_rising_detect(gpio, enable);
		}

		void set_irq_enable(int gpio, bool enable)
		{
			_driver.set_gpio_irq_enable(gpio, enable);
		}

		void register_signal(Genode::Signal_context_capability cap, int gpio)
		{
			_driver.register_signal(cap, gpio);
		}

		void unregister_signal(int gpio)
		{
			_driver.unregister_signal(gpio);
		}
};


int main(int, char **)
{
	using namespace Gpio;

	printf("--- omap4 gpio driver ---\n");

	Driver       driver;

	Irq_handler  gpio1_irq(Driver::GPIO1_IRQ, driver);
	Irq_handler  gpio2_irq(Driver::GPIO2_IRQ, driver);
	Irq_handler  gpio3_irq(Driver::GPIO3_IRQ, driver);
	Irq_handler  gpio4_irq(Driver::GPIO4_IRQ, driver);
	Irq_handler  gpio5_irq(Driver::GPIO5_IRQ, driver);
	Irq_handler  gpio6_irq(Driver::GPIO6_IRQ, driver);

	/*
	 * Initialize server entry point
	 */
	enum { STACK_SIZE = 4096 };
	static Cap_connection cap;
	static Rpc_entrypoint ep(&cap, STACK_SIZE, "gpio_ep");

	/*
	 * Let the entry point serve the gpio session and root interfaces
	 */
	static Session_component          gpio_session(driver);
	static Static_root<Gpio::Session> gpio_root(ep.manage(&gpio_session));

	/*
	 * Announce service
	 */
	env()->parent()->announce(ep.manage(&gpio_root));

	Genode::sleep_forever();
	return 0;
}

