/*
 * \brief  IOMUX controller register description
 * \author Stefan Kalkowski <stefan.kalkowski@genode-labs.com>
 * \date   2013-04-29
 */

/*
 * Copyright (C) 2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _DRIVERS__PLATFORM__IMX6__IOMUX_H_
#define _DRIVERS__PLATFORM__IMX6__IOMUX_H_

/* Genode includes */
#include <util/mmio.h>
#include <os/attached_io_mem_dataspace.h>


class Iomux : public Genode::Attached_io_mem_dataspace,
               Genode::Mmio
{
	private:

		struct Gpr_9 : Register<0x24, 32> { };
		struct Gpr_10 : Register<0x28, 32> { };

		
	public:

		
		Iomux()
		: Genode::Attached_io_mem_dataspace(Genode::Board_base::IOMUXC_BASE,
		                                    Genode::Board_base::IOMUXC_SIZE),
		Genode::Mmio((Genode::addr_t)local_addr<void>()) {
			write<Gpr_9>(0x3);	
		}

		
};

#endif /* _DRIVERS__PLATFORM__IMX6__IOMUX_H_ */
