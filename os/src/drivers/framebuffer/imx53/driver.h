/*
 * \brief  Frame-buffer driver for the i.MX53 display-subsystem with MCIMX28LCD display
 * \author Nikolay Golikov <nik@ksyslabs.org>
 * \date 2012-11-10
 */

/*
 * Copyright (C) 2012 Ksys Labs LLC
 * Copyright (C) 2012 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/* Genode includes */
#include <drivers/board.h>
#include <os/attached_io_mem_dataspace.h>
#include <io_mem_session/connection.h>
#include <util/mmio.h>

/* local includes */
#include <ipu.h>


namespace Framebuffer {
	using namespace Genode;
	class Driver;
};


class Framebuffer::Driver
{
	private:

		/* Image processing unit memory */
		Attached_io_mem_dataspace _ipu_mmio;
		Ipu                       _ipu;

	public:

		enum {
			REFRESH          = 60,
			WIDTH            = 800,
			HEIGHT           = 480,
			PIX_CLK          = 29850,
			ROUND_PIX_CLK    = 38000,
			LEFT_MARGIN      = 89,
			RIGHT_MARGIN     = 104,
			UPPER_MARGIN     = 10,
			LOWER_MARGIN     = 10,
			VSYNC_LEN        = 10,
			HSYNC_LEN        = 10,
			BYTES_PER_PIXEL  = 2,
			FRAMEBUFFER_SIZE = WIDTH * HEIGHT * BYTES_PER_PIXEL,
		};


		Driver()
		:
			_ipu_mmio(Board::IPU_BASE, Board::IPU_SIZE),
			_ipu((addr_t)_ipu_mmio.local_addr<void>())
		{ }


		bool init(addr_t phys_base)
		{
			_ipu.init_idmac(23, WIDTH, HEIGHT, WIDTH * BYTES_PER_PIXEL, phys_base);

			return true;
		}

};

