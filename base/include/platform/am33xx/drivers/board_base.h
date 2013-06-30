/*
 * \brief  Driver for the am33xx
 * \author Nikolay Golikov <nik@ksyslabs.org>
 * \date   2013-06-30
 */

/*
 * Copyright (C) 2013 Ksys Labs LLC
 * Copyright (C) 2011-2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _INCLUDE__DRIVERS__BOARD_BASE_H_
#define _INCLUDE__DRIVERS__BOARD_BASE_H_

namespace Genode
{
	struct Board_base
	{
		enum
		{
			/* device IO memory */
			MMIO_0_BASE = 0x44c00000,
			MMIO_0_SIZE = 0x00400000,
			MMIO_1_BASE = 0x48000000,
			MMIO_1_SIZE = 0x00400000,

			/* normal RAM */
			RAM_0_BASE = 0x80000000,
			RAM_0_SIZE = 0x10000000,

			/* GPIO */
			GPIO_BANKS      = 4,
			GPIO_PINS       = 32,
			
			GPIO0_MMIO_BASE = MMIO_0_BASE + 0x207000,
			GPIO1_MMIO_BASE = MMIO_1_BASE + 0x04c000,
			GPIO2_MMIO_BASE = MMIO_1_BASE + 0x1ac000,
			GPIO3_MMIO_BASE = MMIO_1_BASE + 0x1ae000,

			GPIO0_MMIO_SIZE = 0x1000,
			GPIO1_MMIO_SIZE = 0x1000,
			GPIO2_MMIO_SIZE = 0x1000,
			GPIO3_MMIO_SIZE = 0x1000,

			GPIO0_IRQ       = 96,
			GPIO1_IRQ       = 98,
			GPIO2_IRQ       = 32,
			GPIO3_IRQ       = 62,

			/* wether board provides security extension */
			SECURITY_EXTENSION = 0,

		};
	};
}

#endif /* _INCLUDE__DRIVERS__BOARD_BASE_H_ */

