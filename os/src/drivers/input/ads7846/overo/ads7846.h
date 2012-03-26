/**
 * \brief  Interface to ADS7846
 * \author Ivan Loskutov
 * \date   2012-02-16
 */

/*
 * Copyright 2012 Ksys Labs LLC 
 * Contact: <ivan.loskutov@ksyslabs.org>
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _ADS7846_H_
#define _ADS7846_H_

#include <base/env.h>
#include <base/printf.h>
#include <input/keycodes.h>
#include <os/attached_io_mem_dataspace.h>
#include <timer_session/connection.h>

#include "event_queue.h"
#include "input_driver.h"

class ADS7846 : public Input_driver
{
	enum {
		LEFT                = 0,
		NUM_BUTTONS,
	};

	enum {
		MAX_SAMPLES         = 8,
	};

	enum {
		// GPIO registers
		GPIO_IRQSTATUS1     = 0x18,
		GPIO_IRQENABLE1     = 0x1C,
		GPIO_DATAIN         = 0x38,

		GPIO_FALLINGDETECT  = 0x4C,

		GPIO_DEBOUNCENABLE  = 0x50,
		GPIO_DEBOUNCINGTIME = 0x54,

		PIN_18              = (1 << 18),
	};

	enum {
		// MCSPI registers
		MCSPI_SYSCONFIG     = 0x10,
		MCSPI_SYSSTATUS     = 0x14,
		MCSPI_IRQENABLE     = 0x1c,
		MCSPI_MODULCTRL     = 0x28,
		MCSPI_CH0CONF       = 0x2c,

		MCSPI_CH0STAT       = 0x30,
		MCSPI_CH0CTRL       = 0x34,
		MCSPI_TX0           = 0x38,
		MCSPI_RX0           = 0x3C,

		// MCSPI_SYSCONFIG
		SOFTRESET           = (1 << 1),

		// MCSPI_SYSSTATUS
		RESETDONE           = (1 << 0),

		// MCSPI_MODULCTRL
		SINGLE              = (1 << 0),

		// MCSPI_CHxCTRL
		EN                  = (1 << 0),

		// MCSPI_CHxSTAT
		EOT                 = (1 << 2),

		// MCSPI_CHxCONF
		CLKD_32             = (0x05 << 2),
		WL_8BIT             = (0x07 << 7),
		EPOL_LOW            = (1 << 6),
		DPE0                = (1 << 16),
		FORCE               = (1 << 20),
	};

	enum {
		// Bit positions for ADS7846E Control byte
		ADS_START           = (1 << 7),     // Start bit, always 1
		ADS_8_BIT           = (1 << 3),     // 0 = 12-bit conversion, 1 = 8-bits
		ADS_12_BIT          = (0 << 3),
		ADS_SER             = (1 << 2),     // 0 = Differential, 1 = Single ended
		ADS_DFR             = (0 << 2),

		ADS_A2A1A0_d_y      = (0x01 << 4),  // Y position measurement
		ADS_A2A1A0_d_z1     = (0x03 << 4),  // pressure measurement 1
		ADS_A2A1A0_d_z2     = (0x04 << 4),  // pressure measurement 2
		ADS_A2A1A0_d_x      = (0x05 << 4),  // X position measurement

		ADS_PD10_PDOWN      = (0 << 0),     // low power mode + penirq
		ADS_PD10_ADC_ON     = (1 << 0),     // ADC on
		ADS_PD10_REF_ON     = (2 << 0),     // vREF on + penirq
		ADS_PD10_ALL_ON     = (3 << 0),     // ADC + vREF on
	};

	private:

		static const bool                   verbose = false;

		Event_queue                         &_ev_queue;
		Timer::Connection                   _timer;

		Genode::Attached_io_mem_dataspace   _gpio_io_mem;
		Genode::addr_t volatile             _gpio_base;
		Genode::Attached_io_mem_dataspace   _spi_io_mem;
		Genode::addr_t volatile             _spi_base;


		int                                 _bufx[MAX_SAMPLES];
		int                                 _bufy[MAX_SAMPLES];

		int                                 _empty_cnt;
		int                                 _valid_cnt;
		bool                                _pen_down;

		int                                 _abs_x;
		int                                 _abs_y;

		void _reg_write(Genode::addr_t reg, Genode::uint32_t value) {
			*(Genode::uint32_t volatile *)(reg) = value; }

		Genode::uint32_t _reg_read(Genode::addr_t reg) {
			return *(Genode::uint32_t volatile *)(reg); }

		Genode::uint16_t _ads7846_wr(Genode::uint16_t d)
		{
			Genode::uint32_t    tmp0;
			Genode::uint32_t    tmp1;

			_reg_write( _spi_base + MCSPI_TX0, d);

			while ( !(_reg_read( _spi_base + MCSPI_CH0STAT) & EOT) )
				;

			_timer.msleep(1);

			tmp0 = _reg_read( _spi_base + MCSPI_RX0);

			_reg_write( _spi_base + MCSPI_TX0, 0);
			while ( !(_reg_read( _spi_base + MCSPI_CH0STAT) & EOT) )
				;

			tmp0 = _reg_read( _spi_base + MCSPI_RX0);

			_reg_write( _spi_base + MCSPI_TX0, 0);
			while ( !(_reg_read( _spi_base + MCSPI_CH0STAT) & EOT) )
				;

			tmp1 = _reg_read( _spi_base + MCSPI_RX0);

			return ((tmp0 & 0x7f) << 5) + (tmp1 >> 3);
		}

		void _init()
		{
			if (verbose)
				Genode::printf("Init Touchscreen.\n");

			// Configure GPIO
			_reg_write( _gpio_base + GPIO_DEBOUNCINGTIME, 100);
			_reg_write( _gpio_base + GPIO_DEBOUNCENABLE, PIN_18);

			_reg_write( _gpio_base + GPIO_FALLINGDETECT, PIN_18);
			_reg_write( _gpio_base + GPIO_IRQENABLE1, PIN_18);

			// Configure SPI module
			// reset module
			int r = _reg_read( _spi_base + MCSPI_SYSCONFIG);
			r |= SOFTRESET;
			_reg_write( _spi_base + MCSPI_SYSCONFIG, r);

			while( !(_reg_read( _spi_base + MCSPI_SYSSTATUS) & RESETDONE) )
			{
				_timer.msleep(1);
			}

			// disable channel
			_reg_write( _spi_base + MCSPI_CH0CTRL, 0);

			// disable interrupts
			_reg_write( _spi_base + MCSPI_IRQENABLE, 0);

			r = ( DPE0 | EPOL_LOW | CLKD_32 | WL_8BIT );
			_reg_write( _spi_base + MCSPI_CH0CONF, r);

			// enable
			_reg_write( _spi_base + MCSPI_MODULCTRL, SINGLE);

			r = _reg_read( _spi_base + MCSPI_CH0CONF);
			r |= FORCE;
			_reg_write( _spi_base + MCSPI_CH0CONF, r);

			r = _reg_read( _spi_base + MCSPI_CH0CTRL);
			r |= EN;
			_reg_write( _spi_base + MCSPI_CH0CTRL, r);

			r = _ads7846_wr( ADS_START);
		}

		void _tsc_get_pen_position(int *x, int *y, int *Rt)
		{
			unsigned short  ts_y, ts_x, ts_z1, ts_z2;
			unsigned int    ts_rt;

			// read y
			ts_y = _ads7846_wr( ADS_START | ADS_PD10_ADC_ON | ADS_PD10_REF_ON | ADS_A2A1A0_d_y);
			// read x
			ts_x = _ads7846_wr( ADS_START | ADS_PD10_ADC_ON | ADS_PD10_REF_ON | ADS_A2A1A0_d_x);
			// read z1
			ts_z1 = _ads7846_wr( ADS_START | ADS_PD10_ADC_ON | ADS_PD10_REF_ON | ADS_A2A1A0_d_z1);
			// read z2
			ts_z2 = _ads7846_wr( ADS_START | ADS_PD10_ADC_ON | ADS_PD10_REF_ON | ADS_A2A1A0_d_z2);

			if ( (ts_z1 > 0) && (ts_x > 0)) {
				// compute touch pressure resistance
				ts_rt = ts_z2;
				ts_rt -= ts_z1;
				ts_rt *= ts_x;
				ts_rt *= 180;
				ts_rt /= ts_z1;
				ts_rt = (ts_rt + 2047) >> 12;
			}
			else {
				ts_rt = -1;
			}

			//Genode::printf ("Info: (ts_x,ts_y,ts_z1,ts_z2,ts_rt)=(%d, %d, %d, %d, %lui)\n", ts_x, ts_y, ts_z1, ts_z2, ts_rt);
			if (ts_rt < 250)
			{
				*x = ts_x;
				*y = ts_y;
				*Rt = ts_rt;
			} else {
				*x = -1;
				*y = -1;
			}
		}

		void _create_motion_event()
		{
			int x = 0;
			int y = 0;
			int Rt = 0;

			_tsc_get_pen_position(&x, &y, &Rt);

			if (x==-1 || y==-1)
			{
				_empty_cnt++;
				return;
			}

			_timer.msleep(1);
			if ( _valid_cnt<MAX_SAMPLES)
			{
				// todo: check
				_bufx[_valid_cnt] = x;
				_bufy[_valid_cnt] = y;
				_valid_cnt++;
				return;
			}
			else {
				// shift buffers
				for(int i = 1; i != MAX_SAMPLES; ++i)
				{
					_bufx[i-1] = _bufx[i];
					_bufy[i-1] = _bufy[i];
				}
				_bufx[MAX_SAMPLES-1] = x;
				_bufy[MAX_SAMPLES-1] = y;
			}

			int sumx = 0;
			int sumy = 0;
			for(int i = 0; i != MAX_SAMPLES; ++i)
			{
				sumx += _bufx[i];
				sumy += _bufy[i];
			}

			_abs_x = sumx/MAX_SAMPLES;
			_abs_y = sumy/MAX_SAMPLES;

			if (verbose)
				Genode::printf ("create_motion_event ts_x=%d, ts_y=%d Rt=%d\n", x, y, Rt);

			if (verbose)
				Genode::printf("post MOTION, abs_x = %d, abs_y = %d\n", _abs_x, _abs_y);

			_ev_queue.add(Input::Event(Input::Event::MOTION,
									0, _abs_x, _abs_y, 0, 0));

			// in first generate key press
			if ( _pen_down)
			{
				if (verbose)
					Genode::printf("post %s, BTN_LEFT x=%d y=%d\n", "PRESS", _abs_x, _abs_y);

				_ev_queue.add(Input::Event(Input::Event::PRESS, Input::BTN_LEFT, _abs_x, _abs_y, 0, 0));

				_pen_down = false;
			}
		}

	public:

        ADS7846(Event_queue &ev_queue, int irq_number)
		:
			_ev_queue(ev_queue),
			_gpio_io_mem(OMAP3_GPIO4_PHYS, OMAP3_GPIO4_SIZE),
			_gpio_base((Genode::addr_t)_gpio_io_mem.local_addr<unsigned volatile>()),
			_spi_io_mem(OMAP3_SPI1_PHYS, OMAP3_SPI1_SIZE),
			_spi_base((Genode::addr_t)_spi_io_mem.local_addr<unsigned volatile>())
		{
			_init();
		}

		void handle_event()
		{
			Genode::uint32_t r = _reg_read( _gpio_base + GPIO_IRQENABLE1 );
			_reg_write( _gpio_base + GPIO_IRQENABLE1, r & ~PIN_18);

			if ( !(_reg_read( _gpio_base + GPIO_DATAIN ) & PIN_18) )
            {
				if (verbose)
					Genode::printf("handle_event\n");

				_empty_cnt = 0;
				_valid_cnt = 0;
				_pen_down = true;

				_ads7846_wr( ADS_START | ADS_A2A1A0_d_x |  ADS_PD10_ADC_ON | ADS_PD10_REF_ON);
				_timer.msleep(1);

				while(1)
				{
					_create_motion_event();
					if ( _empty_cnt > 10)
					{
						// pwr down
						_ads7846_wr( ADS_START);
						_timer.msleep(5);
						if( (_reg_read( _gpio_base + GPIO_DATAIN) & PIN_18) )
							break;

						_ads7846_wr( ADS_START | ADS_A2A1A0_d_x |  ADS_PD10_ADC_ON | ADS_PD10_REF_ON);
						_timer.msleep(1);

						_empty_cnt = 0;
					}
				}

				if (!_pen_down)
				{
					if (verbose)
						Genode::printf("post %s, BTN_LEFT x=%d y=%d\n", "RELEASE", _abs_x, _abs_y);

					_ev_queue.add(Input::Event(Input::Event::RELEASE, Input::BTN_LEFT, _abs_x, _abs_y, 0, 0));
				}
			}

			_reg_write( _gpio_base + GPIO_IRQSTATUS1, PIN_18);
			_reg_write( _gpio_base + GPIO_IRQENABLE1, PIN_18);
		}
};

#endif /* _ADS7846_H_ */
