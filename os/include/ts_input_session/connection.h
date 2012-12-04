/*
 * \brief  Connection to Ts_input session
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

#ifndef _INCLUDE__TS_INPUT_SESSION__CONNECTION_H_
#define _INCLUDE__TS_INPUT_SESSION__CONNECTION_H_

#include <ts_input_session/ts_input_session.h>
#include <base/connection.h>

namespace Ts_input {

	struct Connection : Genode::Connection<Session>, Session_client
	{
		Connection()
		:
			Genode::Connection<Ts_input::Session>(session("ram_quota=4K")),
			Session_client(cap())
		{ }
	};
}

#endif /* _INCLUDE__TS_INPUT_SESSION__CONNECTION_H_ */
