/*
 * \brief  General display subsystem registers
 * \author Norman Feske
 * \date   2012-06-11
 */

#ifndef _DSS_H_
#define _DSS_H_

/* Genode includes */
#include <util/mmio.h>

struct Dss : Genode::Mmio
{
	Dss(Genode::addr_t const mmio_base) : Genode::Mmio(mmio_base) { }

	struct Revision : Register<0x00, 32> { };

	struct Sysstatus : Register<0x14, 32> {
		struct Reset : Bitfield<0, 1>
		{
			enum { RESETDONE = 1 };
		};
	};

	struct Ctrl : Register<0x40, 32>
	{
		struct Lcd2_ts_sel : Bitfield<17, 1>
		{
			enum { LCD2=0, TV = 1 };
		};
		struct Venc_hdmi_switch : Bitfield<15, 1>
		{
			enum { HDMI = 1 };
		};
	};
	struct Status : Register<0x5c, 32> { };
};

#endif /* _DSS_H_ */
