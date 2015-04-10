/*
 * \brief  Board definitions for the Freescale i.MX6
 * \author Nikolay Golikov <nik@ksyslabs.org>
 * \author Josef Soentgen
 * \author Martin Stein
 * \date   2014-02-25
 */

/*
 * Copyright (C) 2014-2015 Ksys Labs LLC
 * Copyright (C) 2014-2015 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _DRIVERS__BOARD_BASE_H_
#define _DRIVERS__BOARD_BASE_H_

namespace Imx6
{
	/**
	 * i.MX6 motherboard
	 */
	struct Board_base;
}

struct Imx6::Board_base
{
	enum {
		/* device IO memory */
		MMIO_BASE = 0x00000000,
		MMIO_SIZE = 0x10000000,

		/* timer */
		EPIT_2_IRQ       = 89,
		EPIT_2_MMIO_BASE = 0x020d4000,
		EPIT_2_MMIO_SIZE = 0x00004000,

		/* ARM IP Bus control */
		AIPS_1_MMIO_BASE = 0x0207c000,
		AIPS_1_MMIO_SIZE = 0x00004000,
		AIPS_2_MMIO_BASE = 0x0217c000,
		AIPS_2_MMIO_SIZE = 0x00004000,

		/* CPU */
		CORTEX_A9_PRIVATE_MEM_BASE  = 0x00a00000,
		CORTEX_A9_PRIVATE_MEM_SIZE  = 0x00002000,
		CORTEX_A9_PRIVATE_TIMER_CLK = 395037500,

		/* CPU cache */
		CACHE_LINE_SIZE_LOG2 = 5,

		/* whether board provides ARM security extension */
		SECURITY_EXTENSION = 1,
	};
};

#endif /* _DRIVERS__BOARD_BASE_H_ */
