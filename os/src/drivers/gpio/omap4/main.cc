/*
 * \brief  Gpio driver for the OMAP4
 * \author Ivan Loskutov
 * \date   2012-06-16
 */

/*
 * Copyright 2012 Ksys Labs LLC
 * Contact: <ivan.loskutov@ksyslabs.org>
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
#include <driver.h>


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
};


int main(int, char **)
{
	using namespace Gpio;

	static Driver driver;

	printf("--- omap4 gpio driver ---\n");

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

	sleep_forever();
	return 0;
}

