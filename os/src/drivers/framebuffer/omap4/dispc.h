/*
 * \brief  Display controller
 * \author Martin Stein
 * \author Norman Feske
 * \date   2012-06-11
 */

#ifndef _DISPC_H_
#define _DISPC_H_

/* Genode includes */
#include <util/mmio.h>

struct Dispc : Genode::Mmio
{
	struct Revision : Register<0x00, 32> { };
	struct Sysconfig : Register<0x10, 32> { };
	/**
	 * Configures the display controller module for outputs LCD 1 and TV
	 */
	struct Control1 : Register<0x40, 32>
	{
		struct Tv_enable : Bitfield<1, 1> { };

		struct Go_tv : Bitfield<6, 1>
		{
			enum { HW_UPDATE_DONE    = 0x0,   /* set by HW after updating */
			       REQUEST_HW_UPDATE = 0x1 }; /* must be set by user */
		};
	};

	/**
	 * Configures the display controller module for outputs LCD 1 and TV
	 */
	struct Config1 : Register<0x44, 32>
	{
		/**
		 * Loading of palette/gamma table
		 */
		struct Load_mode : Bitfield<1, 2>
		{
			enum { DATA_EVERY_FRAME = 0x2, };
		};
	};

	struct Default_color1 : Register<0x50, 32> { };
	struct Trans_color1 : Register<0x58, 32> { };

	struct Size_tv : Register<0x78, 32>
	{
		struct Width  : Bitfield<0, 11>  { };
		struct Height : Bitfield<16, 11> { };
	};

	/**
	 * Configures base address of the graphics buffer
	 */
	struct Gfx_ba1 : Register<0x80, 32> { };
	struct Gfx_ba2 : Register<0x84, 32> { };

	/**
	 * Configures the size of the graphics window
	 */
	struct Gfx_size : Register<0x8c, 32>
	{
		struct Sizex : Bitfield<0,11>  { };
		struct Sizey : Bitfield<16,11> { };
	};

	/**
	 * Configures the graphics attributes
	 */
	struct Gfx_attributes : Register<0xa0, 32>
	{
		struct Enable : Bitfield<0, 1> { };

		struct Format : Bitfield<1, 5>
		{
			enum { RGB16  = 0x6,
			       ARGB32 = 0xc,
			       RGBA32 = 0xd };
		};

		struct Replication : Bitfield<5, 1>
		{
		};

		/**
		 * Select GFX channel output
		 */
		struct Channelout : Bitfield<8, 1>
		{
			enum { TV = 0x1 };
		};

		struct Channelout2 : Bitfield<30, 2>
		{
			enum { PRIMARY_LCD = 0, SECONDARY_LCD = 1 };
		};
	};

	struct Gfx_buf_threshold : Register<0xa4, 32>{};

	struct Gfx_row_inc : Register<0xac, 32> { };
	struct Gfx_pixel_inc : Register<0xb0, 32> { };

	struct Vid1_attributes : Register<0xcc, 32>
	{
		struct Channelout : Bitfield<16, 1>
		{
			enum { TV = 0x1 };
		};

		struct Channelout2 : Bitfield<30, 2>
		{
			enum { PRIMARY_LCD = 0, SECONDARY_LCD = 1 };
		};
	};

	struct Vid2_attributes : Register<0x15c, 32>
	{
		struct Channelout : Bitfield<16, 1>
		{
			enum { TV = 0x1 };
		};

		struct Channelout2 : Bitfield<30, 2>
		{
			enum { PRIMARY_LCD = 0, SECONDARY_LCD = 1 };
		};
	};


	struct Vid1_conv_coef0 : Register<0x130, 32> { };
	struct Vid1_conv_coef1 : Register<0x134, 32> { };
	struct Vid1_conv_coef2 : Register<0x138, 32> { };
	struct Vid1_conv_coef3 : Register<0x13c, 32> { };
	struct Vid1_conv_coef4 : Register<0x140, 32> { };

	struct Vid2_conv_coef0 : Register<0x1c0, 32> { };
	struct Vid2_conv_coef1 : Register<0x1c4, 32> { };
	struct Vid2_conv_coef2 : Register<0x1c8, 32> { };
	struct Vid2_conv_coef3 : Register<0x1cc, 32> { };
	struct Vid2_conv_coef4 : Register<0x1d0, 32> { };

	struct Control2 : Register<0x238, 32> {

		struct Lcd_enable : Bitfield<0, 1> { };

		struct Stntft : Bitfield<3, 1>
		{
			enum { ACTIVE=1, PASSIVE=0 };
		};

		struct Go_lcd : Bitfield<5, 1>
		{
			enum { HW_UPDATE_DONE    = 0x0,   /* set by HW after updating */
			REQUEST_HW_UPDATE = 0x1 }; /* must be set by user */
		};


		struct Tftdatalines : Bitfield<8, 2>
		{
			enum {
				BIT_12=0,
				BIT_16=1,
				BIT_18=2,
				BIT_24=3,
			};
		};
	};

	struct Default_color2 : Register<0x3ac, 32> { };

	struct Trans_color2 : Register<0x3b0, 32> { };

	struct Size_lcd2 : Register<0x3cc, 32> {
		struct Sizex : Bitfield<0,11>  { };
		struct Sizey : Bitfield<16,11> { };
	};


	struct Timing_h2 : Register<0x400, 32> { };
	struct Timing_v2 : Register<0x404, 32> { };

	struct Divisor2 : Register<0x40c, 32>
	{
		struct Pcd    : Bitfield<0, 7> { };
		struct Lcd    : Bitfield<16, 7>  { };
	};

	struct Global_buffer : Register<0x800, 32> { };


	struct Divisor : Register<0x804, 32>
	{
		struct Enable : Bitfield<0, 1>  { };
		struct Lcd    : Bitfield<16, 8> { };
	};

	/**
	 * Constructor
	 *
	 * \param mmio_base  base address of DISPC MMIO
	 */
	Dispc(Genode::addr_t const mmio_base)
	:
		Mmio(mmio_base)
	{ }
};

#endif /* _DISPC_H_ */
