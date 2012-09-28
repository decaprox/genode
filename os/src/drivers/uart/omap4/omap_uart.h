/*
 * \brief  Driver for OMAP4 UARTs
 * \author Ivan Loskutov <ivan.loskutov@ksyslabs.org>
 * \date   2012-09-28
 */

/*
 * Copyright (C) 2012 Ksys Labs LLC
 * Copyright (C) 2011-2012 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _OMAP_UART_H_
#define _OMAP_UART_H_

/* Genode includes */
#include <base/env.h>
#include <base/lock.h>
#include <base/printf.h>
#include <os/irq_activation.h>
#include <os/attached_io_mem_dataspace.h>
#include <util/mmio.h>
#include <timer_session/connection.h>

/* local includes */
#include "terminal_driver.h"

#include "uart.h"

class Omap_uart : public Terminal::Driver, public Genode::Irq_handler
{
private:

	Genode::Attached_io_mem_dataspace _uart_mmio;
	Uart                              _uart;
	Timer::Connection         _timer;

	void _clear_fifos()
	{
		_uart.write<Uart::FCR>(Uart::FCR::FIFO_EN);
		_uart.write<Uart::FCR>(Uart::FCR::RX_FIFO_CLEAR | Uart::FCR::TX_FIFO_CLEAR | Uart::FCR::FIFO_EN);
		_uart.write<Uart::FCR>(0);
	}

	void _init_comport(unsigned baud)
	{
		_clear_fifos();
		_uart.write<Uart::MCR>(Uart::MCR::RTS);

		_uart.read<Uart::LSR>();
		if ( _uart.read<Uart::LSR::RX_FIFO_E>() )
			_uart.read<Uart::RHR>();
		_uart.read<Uart::IIR>();
		_uart.read<Uart::MSR>();

		_uart.write<Uart::LCR>(Uart::LCR::CHAR_LENGTH_8);

		_uart.write<Uart::MCR>(Uart::MCR::OUT2);

		_uart.write<Uart::IER>(Uart::IER::RHR_IT);

		_uart.write<Uart::MDR1>(Uart::MDR1::DISABLE);

		_uart.write<Uart::LCR>(0xbf);
		_uart.write<Uart::EFR>(Uart::EFR::ENCHANCED_EN);

		_uart.write<Uart::LCR>(0x80);
		_uart.write<Uart::DLL>(baud & 0xff);
		_uart.write<Uart::DLH>(baud >> 8);

		_uart.write<Uart::LCR>(0xbf);
		_uart.write<Uart::EFR>(0);

		_uart.write<Uart::LCR>(Uart::LCR::CHAR_LENGTH_8 | Uart::LCR::PARITY_DIS | Uart::LCR::NB_STOP_1);

		_uart.write<Uart::MDR1>(0);
	}

	Terminal::Char_avail_callback &_char_avail_callback;

	enum { IRQ_STACK_SIZE = 4096 };
	Genode::Irq_activation _irq_activation;

public:

	/**
		* Constructor
		*/
	Omap_uart(Genode::addr_t mmio_base, Genode::size_t mmio_size, int irq_number, unsigned baud,
			Terminal::Char_avail_callback &callback)
	:
	_uart_mmio(mmio_base, mmio_size),
	_uart((Genode::addr_t)_uart_mmio.local_addr<void>()),
	_char_avail_callback(callback),
	_irq_activation(irq_number, *this, IRQ_STACK_SIZE)
	{
		_init_comport(baud);
	}

	/***************************
		* * IRQ handler interface **
		***************************/

	void handle_irq(int irq_number)
	{
		/* inform client about the availability of data */
		unsigned int iir = _uart.read<Uart::IIR>();
		if (iir & Uart::IIR::IT_PENDING)
			return;
		_uart.read<Uart::LSR>();
		_char_avail_callback();
	}

	/***************************
		* * UART driver interface **
		***************************/

	void put_char(char c)
	{
		/* wait until serial port is ready */
		while (!(_uart.read<Uart::LSR>() & (Uart::LSR::TX_SR_E | Uart::LSR::TX_FIFO_E)));

		/* output character */
		_uart.write<Uart::THR>(c);
	}

	bool char_avail()
	{
		return _uart.read<Uart::LSR::RX_FIFO_E>();
	}

	char get_char()
	{
		return _uart.read<Uart::RHR>();
	}
};

#endif // _OMAP_UART_H_
