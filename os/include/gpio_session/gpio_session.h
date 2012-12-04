/*
 * \brief  Gpio session interface
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

#ifndef _INCLUDE__GPIO_SESSION__GPIO_SESSION_H_
#define _INCLUDE__GPIO_SESSION__GPIO_SESSION_H_

#include <base/signal.h>
#include <dataspace/capability.h>
#include <session/session.h>

namespace Gpio {

	using namespace Genode;

	struct Session : Genode::Session
	{
		static const char *service_name() { return "Gpio"; }

		virtual ~Session() { }

		virtual void set_direction_output(int gpio, bool enable) = 0;
		virtual void set_direction_input(int gpio) = 0;
		virtual void set_dataout(int gpio, bool enable) = 0;
		virtual int  get_datain(int gpio) = 0;
		virtual void set_debounce_enable(int gpio, bool enable) = 0;
		virtual void set_debouncing_time(int gpio, unsigned int us) = 0;
		virtual void set_falling_detect(int gpio, bool enable) = 0;
		virtual void set_rising_detect(int gpio, bool enable) = 0;
		virtual void set_irq_enable(int gpio, bool enable) = 0;

		virtual void register_signal(Genode::Signal_context_capability cap, int gpio) = 0;
		virtual void unregister_signal(int gpio) = 0;

		/*******************
		 ** RPC interface **
		 *******************/

		GENODE_RPC(Rpc_set_direction_output, void, set_direction_output, int, bool);
		GENODE_RPC(Rpc_set_direction_input,  void, set_direction_input,  int);
		GENODE_RPC(Rpc_set_dataout,          void, set_dataout,          int, bool);
		GENODE_RPC(Rpc_get_datain,           int,  get_datain,           int);
		GENODE_RPC(Rpc_set_debounce_enable,  void, set_debounce_enable,  int, bool);
		GENODE_RPC(Rpc_set_debouncing_time,  void, set_debouncing_time,  int, unsigned int);
		GENODE_RPC(Rpc_set_falling_detect,   void, set_falling_detect,   int, bool);
		GENODE_RPC(Rpc_set_rising_detect,    void, set_rising_detect,    int, bool);
		GENODE_RPC(Rpc_set_irq_enable,       void, set_irq_enable,       int, bool);
		GENODE_RPC(Rpc_register_signal,      void, register_signal,      Genode::Signal_context_capability, int);
		GENODE_RPC(Rpc_unregister_signal,    void, unregister_signal,    int);


		typedef Meta::Type_tuple<Rpc_set_direction_output,
				Meta::Type_tuple<Rpc_set_direction_input,
				Meta::Type_tuple<Rpc_set_dataout,
				Meta::Type_tuple<Rpc_get_datain,
				Meta::Type_tuple<Rpc_set_debounce_enable,
				Meta::Type_tuple<Rpc_set_debouncing_time,
				Meta::Type_tuple<Rpc_set_falling_detect,
				Meta::Type_tuple<Rpc_set_rising_detect,
				Meta::Type_tuple<Rpc_set_irq_enable,
				Meta::Type_tuple<Rpc_register_signal,
				Meta::Type_tuple<Rpc_unregister_signal,
								Meta::Empty>
				> > > > > > > > > > Rpc_functions;
	};
}

#endif /* _INCLUDE__GPIO_SESSION__GPIO_SESSION_H_ */
