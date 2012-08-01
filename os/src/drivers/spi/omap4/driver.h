/*
 * \brief  McSPI driver for the OMAP4
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

#ifndef _DRIVER_H_
#define _DRIVER_H_

/* Genode includes */
#include <os/attached_io_mem_dataspace.h>
#include <timer_session/connection.h>
#include <util/mmio.h>

/* local includes */
#include "mcspi.h"


namespace Spi {
	using namespace Genode;
	class Driver;
}

static int verbose = 0;

class Spi::Driver
{
	private:

		struct Timer_delayer : Timer::Connection, Mmio::Delayer
		{
			/**
			 * Implementation of 'Delayer' interface
			 */
			void usleep(unsigned us)
			{
				/* polling */
				if (us == 0)
					return;

				unsigned ms = us / 1000;
				if (ms == 0)
					ms = 1;

				Timer::Connection::msleep(ms);
			}
		} _delayer;

		/* memory map */
		enum {
			MCSPI1_MMIO_BASE = 0x48098000,
			MCSPI1_MMIO_SIZE = 0x1000,

			MCSPI2_MMIO_BASE = 0x4809a000,
			MCSPI2_MMIO_SIZE = 0x1000,

			MCSPI3_MMIO_BASE = 0x480b8000,
			MCSPI3_MMIO_SIZE = 0x1000,

			MCSPI4_MMIO_BASE = 0x480ba000,
			MCSPI4_MMIO_SIZE = 0x1000,

			NR_MCSPIS        = 4,
		};


		Attached_io_mem_dataspace _mcspi1_mmio;
		McSpi                     _mcspi1;
		Attached_io_mem_dataspace _mcspi2_mmio;
		McSpi                     _mcspi2;
		Attached_io_mem_dataspace _mcspi3_mmio;
		McSpi                     _mcspi3;
		Attached_io_mem_dataspace _mcspi4_mmio;
		McSpi                     _mcspi4;

		McSpi                    *_mcspi_bank[NR_MCSPIS];



	public:

		Driver();

		void mcspi_set_module_ctrl(int module, uint32_t value)
		{
			if (verbose)
				PDBG("module=%d, value=%08x", module, value);

			McSpi *spi = _get_mcspi_bank(module);
			spi->write<McSpi::Modulectrl>(value);
		}

		uint32_t mcspi_get_module_ctrl(int module)
		{
			McSpi *spi = _get_mcspi_bank(module);
			uint32_t value = spi->read<McSpi::Modulectrl>();
			if (verbose)
				PDBG("module=%d, value=%08x", module, value);
			return value;
		}

		void mcspi_set_channel_config(int module, int channel, uint32_t value)
		{
			if (verbose)
				PDBG("module=%d, channel=%d, value=%08x", module, channel, value);

			McSpi *spi = _get_mcspi_bank(module);

			if (channel != 0)
			{
				PERR("Only channel 0 supported yet");
				return;
			}
			spi->write<McSpi::Ch0conf>(value);
		}

		uint32_t mcspi_get_channel_config(int module, int channel)
		{
			McSpi *spi = _get_mcspi_bank(module);

			if (channel != 0)
			{
				PERR("Only channel 0 supported yet");
				return -1;
			}
			uint32_t value = spi->read<McSpi::Ch0conf>();
			if (verbose)
				PDBG("module=%d, channel=%d, value=%08x", module, channel, value);
			return value;
		}

		void mcspi_set_channel_control(int module, int channel, uint32_t value)
		{
			if (verbose)
				PDBG("module=%d, channel=%d, value=%08x", module, channel, value);

			McSpi *spi = _get_mcspi_bank(module);

			if (channel != 0)
			{
				PERR("Only channel 0 supported yet");
				return;
			}
			spi->write<McSpi::Ch0ctrl>(value);
		}

		uint32_t mcspi_get_channel_control(int module, int channel)
		{
			McSpi *spi = _get_mcspi_bank(module);

			if (channel != 0)
			{
				PERR("Only channel 0 supported yet");
				return -1;
			}
			uint32_t value = spi->read<McSpi::Ch0ctrl>();
			if (verbose)
				PDBG("module=%d, channel=%d, value=%08x", module, channel, value);
			return value;
		}

		uint32_t mcspi_write(int module, int channel, uint32_t value)
		{
			if (verbose)
				PDBG("module=%d, channel=%d, value=%08x", module, channel, value);

			McSpi *spi = _get_mcspi_bank(module);

			if (channel != 0)
			{
				PERR("Only channel 0 supported yet");
				return -1;
			}
			//spi->write<McSpi::Ch0conf::Force>(1);
			spi->write<McSpi::Ch0ctrl::Enable>(1);
			spi->write<McSpi::Tx0>(value);

			if (!spi->wait_for<McSpi::Ch0stat::RxTxS>(3, _delayer)) {
				PERR("MCSPI%d write timed out", module);
				//spi->write<McSpi::Ch0conf::Force>(0);
				spi->write<McSpi::Ch0ctrl::Enable>(0);
				return -1;
			}
			uint32_t result = spi->read<McSpi::Rx0>();
			spi->write<McSpi::Ch0ctrl::Enable>(0);
			//spi->write<McSpi::Ch0conf::Force>(0);
			return result;
		}

	private:
		McSpi *_get_mcspi_bank(int module)  { return _mcspi_bank[module]; }

		Timer::Connection _timer;

};


Spi::Driver::Driver()
:
	_mcspi1_mmio(MCSPI1_MMIO_BASE, MCSPI1_MMIO_SIZE),
	_mcspi1((addr_t)_mcspi1_mmio.local_addr<void>()),
	_mcspi2_mmio(MCSPI2_MMIO_BASE, MCSPI2_MMIO_SIZE),
	_mcspi2((addr_t)_mcspi2_mmio.local_addr<void>()),
	_mcspi3_mmio(MCSPI3_MMIO_BASE, MCSPI3_MMIO_SIZE),
	_mcspi3((addr_t)_mcspi3_mmio.local_addr<void>()),
	_mcspi4_mmio(MCSPI4_MMIO_BASE, MCSPI4_MMIO_SIZE),
	_mcspi4((addr_t)_mcspi4_mmio.local_addr<void>())
{
	_mcspi_bank[0] = &_mcspi1;
	_mcspi_bank[1] = &_mcspi2;
	_mcspi_bank[2] = &_mcspi3;
	_mcspi_bank[3] = &_mcspi4;

	for(int i = 0; i < NR_MCSPIS; ++i)
	{
		if (verbose)
			PDBG("Init McSPI%d", i);

		_mcspi_bank[i]->write<McSpi::Sysconfig::Softreset>(1);
		if (!_mcspi_bank[i]->wait_for<McSpi::Sysstatus::Resetdone>(McSpi::Sysstatus::Resetdone::COMPLETED, _delayer)) {
			PERR("MCSPI%d reset timed out", i);
			return;
		}

		if (verbose)
		{
			uint32_t ver = _mcspi_bank[i]->read<McSpi::Revision>();
			PDBG("OMAP4 McSPI%d revision %d", i, ver);
		}

		//_mcspi_bank[i]->write<McSpi::Sysconfig>((3<<8));
	}
}

#endif // _DRIVER_H_
