/*
 * \brief  Client-side SPI session interface
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

#ifndef _INCLUDE__SPI_SESSION_H__CLIENT_H_
#define _INCLUDE__SPI_SESSION_H__CLIENT_H_

#include <spi_session/capability.h>
#include <base/rpc_client.h>

namespace Spi {

	struct Session_client : Genode::Rpc_client<Session>
	{
		explicit Session_client(Session_capability session)
		: Genode::Rpc_client<Session>(session) { }

		void set_module_ctrl(int module, Genode::uint32_t value)
		{
			call<Rpc_set_module_ctrl>(module, value);
		}

		Genode::uint32_t get_module_ctrl(int module)
		{
			return call<Rpc_get_module_ctrl>(module);
		}

		void set_channel_config(int module, int channel, Genode::uint32_t value)
		{
			call<Rpc_set_channel_config>(module, channel, value);
		}

		Genode::uint32_t get_channel_config(int module, int channel)
		{
			return call<Rpc_get_channel_config>(module, channel);
		}

		void set_channel_control(int module, int channel, Genode::uint32_t value)
		{
			call<Rpc_set_channel_control>(module, channel, value);
		}

		Genode::uint32_t get_channel_control(int module, int channel)
		{
			return call<Rpc_get_channel_control>(module, channel);
		}

		Genode::uint32_t write(int module, int channel, Genode::uint32_t value)
		{
			return call<Rpc_write>(module, channel, value);
		}

		Genode::uint32_t read(int module, int channel)
		{
			return call<Rpc_read>(module, channel);
		}
	};
}

#endif /* _INCLUDE__SPI_SESSION_H__CLIENT_H_ */