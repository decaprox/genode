/*
 * \brief  Gpio driver for the OMAP4
 * \author Ivan Loskutov
 * \date   2012-06-16
 */

/*
 * Copyright 2012 Ksys Labs LLC
 * Contact: <ivan.loskutov@ksyslabs.org>
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/* Genode includes */
#include <os/attached_io_mem_dataspace.h>
#include <timer_session/connection.h>
#include <util/mmio.h>

/* local includes */
#include <gpio.h>


namespace Gpio {
	using namespace Genode;
	class Driver;
}


class Gpio::Driver
{
	public:

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
			GPIO1_MMIO_BASE = 0x4a310000,
			GPIO1_MMIO_SIZE = 0x1000,

			GPIO2_MMIO_BASE = 0x48055000,
			GPIO2_MMIO_SIZE = 0x1000,

			GPIO3_MMIO_BASE = 0x48057000,
			GPIO3_MMIO_SIZE = 0x1000,

			GPIO4_MMIO_BASE = 0x48059000,
			GPIO4_MMIO_SIZE = 0x1000,

			GPIO5_MMIO_BASE = 0x4805b000,
			GPIO5_MMIO_SIZE = 0x1000,

			GPIO6_MMIO_BASE = 0x4805d000,
			GPIO6_MMIO_SIZE = 0x1000,

			NR_GPIOS        = 6,
		};


		Attached_io_mem_dataspace _gpio1_mmio;
		Gpio_reg                  _gpio1;
		Attached_io_mem_dataspace _gpio2_mmio;
		Gpio_reg                  _gpio2;
		Attached_io_mem_dataspace _gpio3_mmio;
		Gpio_reg                  _gpio3;
		Attached_io_mem_dataspace _gpio4_mmio;
		Gpio_reg                  _gpio4;
		Attached_io_mem_dataspace _gpio5_mmio;
		Gpio_reg                  _gpio5;
		Attached_io_mem_dataspace _gpio6_mmio;
		Gpio_reg                  _gpio6;

		Gpio_reg                  *_gpio_bank[NR_GPIOS];

	public:

		Driver();

		bool set_gpio_direction(int gpio, int is_input);
		bool set_gpio_dataout(int gpio, int enable);

	private:
		Gpio_reg *_get_gpio_bank(int gpio)  { return _gpio_bank[gpio >> 5]; }
		bool      _gpio_valid(int gpio)     { return (gpio < 192) ? true : false; }
		int       _get_gpio_index(int gpio) { return gpio & 0x1f; }
};


Gpio::Driver::Driver()
:
	_gpio1_mmio(GPIO1_MMIO_BASE, GPIO1_MMIO_SIZE),
	_gpio1((addr_t)_gpio1_mmio.local_addr<void>()),
	_gpio2_mmio(GPIO2_MMIO_BASE, GPIO2_MMIO_SIZE),
	_gpio2((addr_t)_gpio2_mmio.local_addr<void>()),
	_gpio3_mmio(GPIO3_MMIO_BASE, GPIO3_MMIO_SIZE),
	_gpio3((addr_t)_gpio3_mmio.local_addr<void>()),
	_gpio4_mmio(GPIO4_MMIO_BASE, GPIO4_MMIO_SIZE),
	_gpio4((addr_t)_gpio4_mmio.local_addr<void>()),
	_gpio5_mmio(GPIO5_MMIO_BASE, GPIO5_MMIO_SIZE),
	_gpio5((addr_t)_gpio5_mmio.local_addr<void>()),
	_gpio6_mmio(GPIO6_MMIO_BASE, GPIO6_MMIO_SIZE),
	_gpio6((addr_t)_gpio6_mmio.local_addr<void>())
{
	_gpio_bank[0] = &_gpio1;
	_gpio_bank[1] = &_gpio2;
	_gpio_bank[2] = &_gpio3;
	_gpio_bank[3] = &_gpio4;
	_gpio_bank[4] = &_gpio5;
	_gpio_bank[5] = &_gpio6;
}

bool Gpio::Driver::set_gpio_direction(int gpio, int is_input)
{
	if (!_gpio_valid(gpio))
		return false;

	Gpio_reg *gpio_reg = _get_gpio_bank(gpio);

	uint32_t value = gpio_reg->read<Gpio_reg::Oe>();
	if (is_input)
		value |= (1 << _get_gpio_index(gpio));
	else
		value &= ~(1 << _get_gpio_index(gpio));
	gpio_reg->write<Gpio_reg::Oe>(value);

	return true;
}

bool Gpio::Driver::set_gpio_dataout(int gpio, int enable)
{
	if (!_gpio_valid(gpio))
		return false;

	Gpio_reg *gpio_reg = _get_gpio_bank(gpio);

	if (enable)
		gpio_reg->write<Gpio_reg::Setdataout>(1 << _get_gpio_index(gpio));
	else
		gpio_reg->write<Gpio_reg::Cleardataout>(1 << _get_gpio_index(gpio));

	return true;
}

