/*
 * \brief  McSPI driver for the OMAP4
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

/* Genode includes */
#include <spi_session/spi_session.h>
#include <cap_session/connection.h>
#include <dataspace/client.h>
#include <base/printf.h>
#include <base/sleep.h>
#include <root/component.h>

/* local includes */
#include "driver.h"

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


namespace Spi {
	using namespace Genode;
	class Session_component;
};


class Spi::Session_component : public Genode::Rpc_object<Spi::Session>
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
		 ** Spi::Session interface **
		 ************************************/

		void set_module_ctrl(int module, uint32_t value)
		{
			_driver.mcspi_set_module_ctrl(module, value);
		}

		uint32_t get_module_ctrl(int module)
		{
			return _driver.mcspi_get_module_ctrl(module);
		}

		void set_channel_config(int module, int channel, uint32_t value)
		{
			_driver.mcspi_set_channel_config(module, channel, value);
		}

		uint32_t get_channel_config(int module, int channel)
		{
			return _driver.mcspi_get_channel_config(module, channel);
		}

		void set_channel_control(int module, int channel, uint32_t value)
		{
			_driver.mcspi_set_channel_control(module, channel, value);
		}

		uint32_t get_channel_control(int module, int channel)
		{
			return _driver.mcspi_get_channel_control(module, channel);
		}

		uint32_t write(int module, int channel, uint32_t value)
		{
			return _driver.mcspi_write(module, channel, value);
		}

		uint32_t read(int module, int channel)
		{
			uint32_t val = _driver.mcspi_write(module, channel, 0);
			//PDBG("read %02x", val & 0xff);
			return val;
		}
};


int main(int, char **)
{
	using namespace Spi;

	printf("--- omap4 mcspi driver ---\n");

	static Driver       driver;

	/*
	 * Initialize server entry point
	 */
	enum { STACK_SIZE = 4096 };
	static Cap_connection cap;
	static Rpc_entrypoint ep(&cap, STACK_SIZE, "spi_ep");

	/*
	 * Let the entry point serve the spi session and root interfaces
	 */
	static Session_component          spi_session(driver);
	static Static_root<Spi::Session>  spi_root(ep.manage(&spi_session));

	/*
	 * Announce service
	 */
	env()->parent()->announce(ep.manage(&spi_root));

	sleep_forever();
	return 0;
}

