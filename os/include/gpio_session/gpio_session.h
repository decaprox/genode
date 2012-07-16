/*
 * \brief  Gpio session interface
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

#ifndef _INCLUDE__GPIO_SESSION__GPIO_SESSION_H_
#define _INCLUDE__GPIO_SESSION__GPIO_SESSION_H_

#include <base/signal.h>
#include <dataspace/capability.h>
#include <session/session.h>

namespace Gpio {

	struct Session : Genode::Session
	{
		static const char *service_name() { return "Gpio"; }

		virtual ~Session() { }

		virtual void set_direction_output(int gpio, bool enable) = 0;
		virtual void set_direction_input(int gpio) = 0;
		virtual void set_dataout(int gpio, bool enable) = 0;

		/*******************
		 ** RPC interface **
		 *******************/

		GENODE_RPC(Rpc_set_direction_output, void, set_direction_output, int, bool);
		GENODE_RPC(Rpc_set_direction_input, void, set_direction_input, int);
		GENODE_RPC(Rpc_set_dataout, void, set_dataout, int, bool);
		GENODE_RPC_INTERFACE(Rpc_set_direction_output, Rpc_set_direction_input, Rpc_set_dataout);
	};
}

#endif /* _INCLUDE__GPIO_SESSION__GPIO_SESSION_H_ */
