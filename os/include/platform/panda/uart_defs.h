/*
 * \brief  OMAP4 UART definitions
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

#ifndef _INCLUDE__PLATFORM__PANDABOARD__UART_DEFS_H_
#define _INCLUDE__PLATFORM__PANDABOARD__UART_DEFS_H_

enum {

	/** Number of UARTs */
	UARTS_NUM = 4,

	/**
	 * MMIO regions
	 */
	UART0_MMIO_BASE = 0x4806a000,  /* base for UART 0 */
	UART1_MMIO_BASE = 0x4806c000,  /* base for UART 1 */
	UART2_MMIO_BASE = 0x48020000,  /* base for UART 2 */
	UART3_MMIO_BASE = 0x4806e000,  /* base for UART 3 */
	UARTS_MMIO_SIZE = 0x400,       /* size of each MMIO region */

	/**
	 * Interrupt lines
	 */
	UART0_IRQ = 72 + 32,      /* UART 0 */
	UART1_IRQ = 73 + 32,      /* UART 1 */
	UART2_IRQ = 74 + 32,      /* UART 2 */
	UART3_IRQ = 70 + 32,      /* UART 3 */

	NS16550_CLK = 48000000,
};


static struct Omap_uart_cfg {
	Genode::addr_t mmio_base;
	Genode::size_t mmio_size;
	int            irq_number;
} omap_uart_cfg[UARTS_NUM] = {
	{ UART0_MMIO_BASE, UARTS_MMIO_SIZE, UART0_IRQ },
	{ UART1_MMIO_BASE, UARTS_MMIO_SIZE, UART1_IRQ },
	{ UART2_MMIO_BASE, UARTS_MMIO_SIZE, UART2_IRQ },
	{ UART3_MMIO_BASE, UARTS_MMIO_SIZE, UART3_IRQ },
};

#endif /* _INCLUDE__PLATFORM__PANDABOARD__UART_DEFS_H_ */
