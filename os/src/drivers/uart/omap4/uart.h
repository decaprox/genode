#ifndef _UART_H_
#define _UART_H_

/* Genode includes */
#include <util/mmio.h>

struct Uart : Genode::Mmio
{
	Uart(Genode::addr_t const mmio_base) : Genode::Mmio(mmio_base) { }

	struct THR    : Register<0x00, 8> {};
	struct RHR    : Register<0x00, 8> {};
	struct DLL    : Register<0x00, 8> {};
	struct IER    : Register<0x04, 8> {
		enum {
			RHR_IT          = (1<<0),
			LINE_ST_IT      = (1<<2),
		};
	};
	struct IIR    : Register<0x08, 8> {
		enum {
			IT_PENDING      = (1<<0),
		};
	};
	struct DLH    : Register<0x04, 8> {};
	struct FCR    : Register<0x08, 8> {
		enum {
			TX_FIFO_CLEAR   = (1<<2),
			RX_FIFO_CLEAR   = (1<<1),
			FIFO_EN         = (1<<0),
		};
	};
	struct EFR    : Register<0x08, 8> {
		enum {
			ENCHANCED_EN    = (1<<4),
		};
	};
	struct LCR    : Register<0x0c, 8> {
		enum {
			DIV_EN          = (1<<7),
			BREAK_EN        = (1 << 6),
			PARITY_EN       = (1 << 3),
			PARITY_DIS      = (0 << 3),
			NB_STOP_1       = (0 << 2),
			NB_STOP_2       = (1 << 2),
			CHAR_LENGTH_5   = (0 << 0),
			CHAR_LENGTH_6   = (1 << 0),
			CHAR_LENGTH_7   = (2 << 0),
			CHAR_LENGTH_8   = (3 << 0),
		};
	};
	struct MCR    : Register<0x10, 8> {
		enum {
			RTS             = (1<<1),
			OUT2            = (1<<3),
		};
	};
	struct LSR    : Register<0x14, 8> {
		struct RX_FIFO_E: Bitfield<0, 1> {};
		enum {
			TX_SR_E         = (1<<6),
			TX_FIFO_E       = (1<<5),
		};
	};
	struct TCR    : Register<0x18, 8> {};
	struct MSR    : Register<0x18, 8> {};
	struct SPR    : Register<0x1c, 8> {};
	struct TLR    : Register<0x1c, 8> {};
	struct MDR1   : Register<0x20, 8> {
		enum {
			DISABLE        = (7<<0),
			UART16X        = (0<<0),
			UART13X        = (3<<0),
		};
	};
	struct MDR2   : Register<0x24, 8> {};
	struct SFLSR  : Register<0x28, 8> {};
	struct TXFLL  : Register<0x28, 8> {};
	struct RESUME : Register<0x2c, 8> {};
	struct TXFLH  : Register<0x2c, 8> {};
	struct SFREGL : Register<0x30, 8> {};
	struct RXFLL  : Register<0x30, 8> {};
	struct SFREGH : Register<0x34, 8> {};
	struct RXFLH  : Register<0x34, 8> {};
	struct BLR    : Register<0x38, 8> {};
	struct UASR   : Register<0x38, 8> {};
};

#endif // _UART_H_