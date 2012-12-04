/*
 * \brief  Connection to SPI session
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

#ifndef _INCLUDE__SPI_SESSION__CONNECTION_H_
#define _INCLUDE__SPI_SESSION__CONNECTION_H_

#include <spi_session/client.h>
#include <base/connection.h>

namespace Spi {

	class Connection : public Genode::Connection<Session>, public Session_client
	{
		public:
			Connection()
			:
				Genode::Connection<Session>(session("ram_quota=4K")),
				Session_client(cap())
			{ }
	};
}

#endif /* _INCLUDE__SPI_SESSION__CONNECTION_H_ */
