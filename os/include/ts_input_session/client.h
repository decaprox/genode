/*
 * \brief  Client-side Ts_input session interface
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

#ifndef _INCLUDE__TS_INPUT_SESSION_H__CLIENT_H_
#define _INCLUDE__TS_INPUT_SESSION_H__CLIENT_H_

#include <ts_input_session/ts_input_session.h>
#include <base/rpc_client.h>
#include <base/printf.h>

namespace Ts_input {

	struct Session_client : Genode::Rpc_client<Session>
	{
		Session_client(Genode::Capability<Session> cap)
		: Genode::Rpc_client<Session>(cap) { }

		void set_calibration(int a0, int a1, int a2, int a3, int a4, int a5, int a6)
		{
			PDBG("client set_calibration");
			call<Rpc_set_calibration>(a0, a1, a2, a3, a4, a5, a6);
		}

		Genode::Dataspace_capability dataspace() {
			return call<Rpc_dataspace>(); }

		bool is_pending() const {
			return call<Rpc_is_pending>(); }

		int flush() {
			return call<Rpc_flush>(); }
	};
}

#endif /* _INCLUDE__TS_INPUT_SESSION_H__CLIENT_H_ */