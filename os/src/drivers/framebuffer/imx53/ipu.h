/*
 * \brief  Image Processing Utit registers
 * \author Nikolay Golikov <nik@ksyslabs.org>
 * \date   2012-11-10
 */

/*
 * Copyright (C) 2012 Ksys Labs LLC
 * Copyright (C) 2012 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _IPU_H_
#define _IPU_H_

/* Genode includes */
#include <util/mmio.h>
#include <util/string.h>


using Genode::uint8_t;


struct Ipu : Genode::Mmio
{
	enum {
		REGS_OFF   = 0x06000000,
		CPMEM_OFF  = 0x01000000,
		IDMAC_CHAN = 23,
	};

	/**
	 * IDMAC cannel enable register
	 */
	struct Idmac_ch_en : Register_array<0x8004, 32, 32, 1>
	{
		struct Ch : Bitfield<0, 1> { };
	};

	/**
	 * IDMAC channel parametrs memory structure
	 */
	struct Cp_mem 
	{
		Genode::uint32_t Data[5];
		Genode::uint32_t Resetrved[3];
	} _ch1_cpmem[2];

	
	void cpmem_set_field(Genode::uint8_t word, Genode::uint8_t bit,
		   Genode::uint8_t size, Genode::uint32_t value) 
	{                                   
		int i = (bit) / 32;
		int off = (bit) % 32;
    	_ch1_cpmem[word].Data[i] |= (value) << off; \
		if (((bit) + (size) - 1) / 32 > i) 
		{
			_ch1_cpmem[word].Data[i + 1] |= (value) >> (off ? (32 - off) : 0);
		}
	}   

	/**
	 * IPU DMA controller initialization
	 */
	void init_idmac(Genode::uint8_t ch,
		   	Genode::uint16_t width, Genode::uint16_t height,
			Genode::uint32_t stride,
			Genode::addr_t phys_base)
	{
		write<Idmac_ch_en::Ch>(0, ch);

		cpmem_set_field(0, 125, 13, width - 1);
		cpmem_set_field(0, 138, 12, height - 1);
		cpmem_set_field(1, 102, 14, stride - 1 ); 
		cpmem_set_field(1, 0, 29, 0);
		cpmem_set_field(1, 29, 29, phys_base >> 3);

		/* bits/pixel */
		cpmem_set_field(0, 107, 3, 3);

		/* pixel format RGB565 */
		cpmem_set_field(1, 85, 4, 7);

		/* burst size */
		cpmem_set_field(1, 78, 7, 15);

		/**
		 *  set packing 
		*/
		/* red */
		cpmem_set_field(1, 116, 3, 4);
		cpmem_set_field(1, 128, 5, 0);
		/* green */
		cpmem_set_field(1, 119, 3, 5);
		cpmem_set_field(1, 133, 5, 5);
		/* blue */
		cpmem_set_field(1, 122, 3, 4);
		cpmem_set_field(1, 138, 5, 11);
		/* alpha */
		cpmem_set_field(1, 125, 3, 7);
		cpmem_set_field(1, 143, 5, 16);

		cpmem_set_field(0, 46, 22, 0);
		cpmem_set_field(0, 68, 22, 0);

		Genode::memcpy((void *)(base + CPMEM_OFF + sizeof(_ch1_cpmem) * ch),
				(void *)&_ch1_cpmem, sizeof(_ch1_cpmem));

		write<Idmac_ch_en::Ch>(1, ch);
	}

	/**
	 * Constructor
	 *
	 * \param mmio_base base address of IPU
	 */
	Ipu(Genode::addr_t mmio_base) 
		:
			Genode::Mmio(mmio_base + REGS_OFF) 
	{ }

};

#endif /* _IPU_H_ */
