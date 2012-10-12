/*
 * \brief  Connection to Smartcard session
 * \author Ivan Loskutov <ivan.loskutov@ksyslabs.org>
 * \date   2012-10-13
 */

/*
 * Copyright (C) 2012 Ksys Labs LLC
 * Copyright (C) 2012 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _INCLUDE__SMARTCARD_SESSION__CONNECTION_H_
#define _INCLUDE__SMARTCARD_SESSION__CONNECTION_H_

#include <smartcard_session/client.h>
#include <base/connection.h>

namespace Smartcard {

	struct Connection : Genode::Connection<Session>, Session_client
	{
			Connection()
			:
				Genode::Connection<Session>(session("ram_quota=4K")),
				Session_client(cap())
			{ }
	};
}

#endif /* _INCLUDE__SMARTCARD_SESSION__CONNECTION_H_ */
