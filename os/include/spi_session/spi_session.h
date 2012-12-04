/*
 * \brief  SPI session interface
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

#ifndef _INCLUDE__SPI_SESSION__SPI_SESSION_H_
#define _INCLUDE__SPI_SESSION__SPI_SESSION_H_

#include <base/signal.h>
#include <dataspace/capability.h>
#include <session/session.h>

namespace Spi {

	struct Session : Genode::Session
	{
		static const char *service_name() { return "Spi"; }

		virtual ~Session() { }

		virtual void     set_module_ctrl(int module, Genode::uint32_t value) = 0;
		virtual Genode::uint32_t get_module_ctrl(int module) = 0;
		virtual void     set_channel_config(int module, int channel, Genode::uint32_t value) = 0;
		virtual Genode::uint32_t get_channel_config(int module, int channel) = 0;
		virtual void     set_channel_control(int module, int channel, Genode::uint32_t value) = 0;
		virtual Genode::uint32_t get_channel_control(int module, int channel) = 0;
		virtual Genode::uint32_t write(int module, int channel, Genode::uint32_t value) = 0;
		virtual Genode::uint32_t read(int module, int channel) = 0;

		/*******************
		 ** RPC interface **
		 *******************/

		GENODE_RPC(Rpc_set_module_ctrl,     void,     set_module_ctrl,     int, Genode::uint32_t);
		GENODE_RPC(Rpc_get_module_ctrl,     Genode::uint32_t, get_module_ctrl,     int);
		GENODE_RPC(Rpc_set_channel_config,  void,     set_channel_config,  int, int, Genode::uint32_t);
		GENODE_RPC(Rpc_get_channel_config,  Genode::uint32_t, get_channel_config,  int, int);
		GENODE_RPC(Rpc_set_channel_control, void,     set_channel_control, int, int, Genode::uint32_t);
		GENODE_RPC(Rpc_get_channel_control, Genode::uint32_t, get_channel_control, int, int);
		GENODE_RPC(Rpc_write,               Genode::uint32_t, write,               int, int, Genode::uint32_t);
		GENODE_RPC(Rpc_read,                Genode::uint32_t, read,                int, int);
		GENODE_RPC_INTERFACE(
			Rpc_set_module_ctrl,
			Rpc_get_module_ctrl,
			Rpc_set_channel_config,
			Rpc_get_channel_config,
			Rpc_set_channel_control,
			Rpc_get_channel_control,
			Rpc_write,
			Rpc_read
		);
	};
}

#endif /* _INCLUDE__SPI_SESSION__SPI_SESSION_H_ */
