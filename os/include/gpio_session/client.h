/*
 * \brief  Client-side Gpio session interface
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

#ifndef _INCLUDE__GPIO_SESSION_H__CLIENT_H_
#define _INCLUDE__GPIO_SESSION_H__CLIENT_H_

#include <gpio_session/capability.h>
#include <base/rpc_client.h>

namespace Gpio {

	struct Session_client : Genode::Rpc_client<Session>
	{
		explicit Session_client(Session_capability session)
		: Genode::Rpc_client<Session>(session) { }


		void set_direction_output(int gpio, bool enable)
		{
			call<Rpc_set_direction_output>(gpio, enable);
		}

		void set_direction_input(int gpio)
		{
			call<Rpc_set_direction_input>(gpio);
		}

		void set_dataout(int gpio, bool enable)
		{
			call<Rpc_set_dataout>(gpio, enable);
		}

		int get_datain(int gpio)
		{
			return call<Rpc_get_datain>(gpio);
		}

		void set_debounce_enable(int gpio, bool enable)
		{
			call<Rpc_set_debounce_enable>(gpio, enable);
		}

		void set_debouncing_time(int gpio, unsigned int us)
		{
			call<Rpc_set_debouncing_time>(gpio, us);
		}

		void set_falling_detect(int gpio, bool enable)
		{
			call<Rpc_set_falling_detect>(gpio, enable);
		}

		void set_rising_detect(int gpio, bool enable)
		{
			call<Rpc_set_rising_detect>(gpio, enable);
		}

		void set_irq_enable(int gpio, bool enable)
		{
			call<Rpc_set_irq_enable>(gpio, enable);
		}

		void register_signal(Genode::Signal_context_capability cap, int gpio)
		{
			call<Rpc_register_signal>(cap, gpio);
		}

		void unregister_signal(int gpio)
		{
			call<Rpc_unregister_signal>(gpio);
		}
	};
}

#endif /* _INCLUDE__GPIO_SESSION_H__CLIENT_H_ */