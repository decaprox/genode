/*
 * \brief  Client-side Gpio session interface
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
	};
}

#endif /* _INCLUDE__GPIO_SESSION_H__CLIENT_H_ */