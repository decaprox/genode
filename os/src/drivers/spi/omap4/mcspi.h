#ifndef _MCSPI_H_
#define _MCSPI_H_

/* Genode includes */
#include <util/mmio.h>

struct McSpi : Genode::Mmio
{
	McSpi(Genode::addr_t const mmio_base) : Genode::Mmio(mmio_base) { }

	struct Revision : Register<0x100, 32> {};

	struct Sysconfig : Register<0x110, 32> {
		struct Softreset: Bitfield<1, 1> {};
	};

	struct Sysstatus : Register<0x114, 32> {
		struct Resetdone: Bitfield<0, 1> { enum { COMPLETED = 1 }; };
	};

	struct Modulectrl : Register<0x128, 32> {};

	struct Irqstatus : Register<0x118, 32> {
		struct Tx0empty: Bitfield<0, 1> {};
	};
	struct Irqenable : Register<0x1c, 32> {};

	struct Ch0conf : Register<0x12c, 32> {
		struct Trm: Bitfield<12, 2> {
			enum { TxRx = 0, Rx = 1, Tx = 2 };
		};
		struct Force: Bitfield<20,1> {};
	};
	struct Ch0stat : Register<0x130, 32> {
		struct Eot: Bitfield<2, 1> {};
		struct RxTxS: Bitfield<0, 2> {};
	};
	struct Ch0ctrl : Register<0x134, 32> {
		struct Enable: Bitfield<0, 1> {};
	};
	struct Tx0 : Register<0x138, 32> {};
	struct Rx0 : Register<0x13c, 32> {};

};

#endif
