#ifndef _GPIO_H_
#define _GPIO_H_

/* Genode includes */
#include <util/mmio.h>

struct Gpio_reg : Genode::Mmio
{
	Gpio_reg(Genode::addr_t const mmio_base) : Genode::Mmio(mmio_base) { }

	struct Oe : Register<0x134, 32> {};
	struct Cleardataout : Register<0x190, 32> {};
	struct Setdataout : Register<0x194, 32> {};
};

#endif
