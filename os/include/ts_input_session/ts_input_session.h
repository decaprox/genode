/*
 * \brief  Ts_input session interface
 * \author Ivan Loskutov
 * \date   2012-03-29
 */

/*
 * Copyright 2012 Ksys Labs LLC
 * Contact: <ivan.loskutov@ksyslabs.org>
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _INCLUDE__TS_INPUT_SESSION__TS_INPUT_SESSION_H_
#define _INCLUDE__TS_INPUT_SESSION__TS_INPUT_SESSION_H_

#include <session/session.h>
#include <base/rpc.h>

namespace Ts_input {

	struct Session : Genode::Session
	{
		static const char *service_name() { return "Ts_input"; }

		virtual void set_calibration(int a0, int a1, int a2, int a3, int a4, int a5, int a6) = 0;

		/**
		 * Return capability to event buffer dataspace
		 */
		virtual Genode::Dataspace_capability dataspace() = 0;

		/**
		 * Request input state
		 *
		 * \return  true if new events are available
		 */
		virtual bool is_pending() const = 0;

		/**
		 * Flush pending events to event buffer
		 *
		 * \return  number of flushed events
		 */
		virtual int flush() = 0;

		/*******************
		 ** RPC interface **
		 *******************/

		GENODE_RPC(Rpc_set_calibration, void, set_calibration, int, int, int, int, int, int, int);
		GENODE_RPC(Rpc_dataspace, Genode::Dataspace_capability, dataspace);
		GENODE_RPC(Rpc_is_pending, bool, is_pending);
		GENODE_RPC(Rpc_flush, int, flush);
		GENODE_RPC_INTERFACE(Rpc_set_calibration, Rpc_dataspace, Rpc_is_pending, Rpc_flush);
	};
}

#endif /* _INCLUDE__TS_INPUT_SESSION__TS_INPUT_SESSION_H_ */
