/**
 * \brief  OMAP3 frame-buffer driver
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

/* Genode includes */
#include <base/printf.h>
#include <base/sleep.h>
#include <base/rpc_server.h>
#include <io_mem_session/connection.h>
#include <cap_session/connection.h>
#include <dataspace/client.h>
#include <timer_session/connection.h>
#include <framebuffer_session/framebuffer_session.h>
#include <root/component.h>
#include <os/ring_buffer.h>
#include <os/config.h>

/* device configuration */
#include <omap3fb_defs.h>
#include <video_memory.h>


/***********************************************
 ** Implementation of the framebuffer service **
 ***********************************************/

namespace Framebuffer
{

	enum {
		SCR_WIDTH           = 480,
		SCR_HEIGHT          = 272,

		BYTES_PER_PIXEL     = 2,
		FRAMEBUFFER_SIZE    = SCR_WIDTH*SCR_HEIGHT*BYTES_PER_PIXEL,
	};

	class Session_component : public Genode::Rpc_object<Session>
	{
		private:

			Genode::Dataspace_capability _fb_ds_cap;
			Genode::Dataspace_client     _fb_ds;
			Genode::addr_t               _regs_base;
			Genode::addr_t               _sys_regs_base;
			Timer::Connection            _timer;

			enum {
				GPIO_OE                         = 0x34,
				GPIO_SETDATAOUT                 = 0x94,

				OVERO_GPIO_LCD_EN               = (1<<16),
				OVERO_GPIO_LCD_BL               = (1<<17),
			};

			enum {
				DSS_CONTROL                     = 0x40,
				DSS_REVISIONNUMBER              = 0x00,
				DSS_SYSCONFIG                   = 0x10,
				DSS_SYSSTATUS                   = 0x14,

				DISPC_REVISION                  = 0x400,
				DISPC_SYSCONFIG                 = 0x410,
				DISPC_SYSSTATUS                 = 0x414,
				DISPC_IRQSTATUS                 = 0x418,
				DISPC_IRQENABLE                 = 0x41C,
				DISPC_CONTROL                   = 0x440,
				DISPC_CONFIG                    = 0x444,
				DISPC_CAPABLE                   = 0x448,
				DISPC_DEFAULT_COLOR_0           = 0x44C,
				DISPC_DEFAULT_COLOR_1           = 0x450,
				DISPC_TRANS_COLOR_0             = 0x454,
				DISPC_TRANS_COLOR_1             = 0x458,
				DISPC_LINE_STATUS               = 0x45C,
				DISPC_LINE_NUMBER               = 0x460,
				DISPC_TIMING_H                  = 0x464,
				DISPC_TIMING_V                  = 0x468,
				DISPC_POL_FREQ                  = 0x46C,
				DISPC_DIVISOR                   = 0x470,
				DISPC_SIZE_DIG                  = 0x478,
				DISPC_SIZE_LCD                  = 0x47C,
				DISPC_GFX_BA0                   = 0x480,
				DISPC_GFX_BA1                   = 0x484,
				DISPC_GFX_POSITION              = 0x488,
				DISPC_GFX_SIZE                  = 0x48C,
				DISPC_GFX_ATTRIBUTES            = 0x4A0,
				DISPC_GFX_FIFO_THRESHOLD        = 0x4A4,
				DISPC_GFX_ROW_INC               = 0x4AC,
				DISPC_GFX_PIXEL_INC             = 0x4B0,

				DISPC_GLOBAL_ALPHA              = 0x474,
				DISPC_VID1_CONV_COEF0           = 0x530,
				DISPC_VID1_CONV_COEF1           = 0x534,
				DISPC_VID1_CONV_COEF2           = 0x538,
				DISPC_VID1_CONV_COEF3           = 0x53c,
				DISPC_VID1_CONV_COEF4           = 0x540,
				DISPC_VID1_ATTRIBUTES           = 0x4cc,

				// DSS_SYSCONFIG
				DSS_SOFTRESET                   = (1 << 2),

				// DSS_SYSSTATUS
				RESETDONE                       = (1 << 0),

				// DISPC_SYSCONFIG
				AUTOIDLE                        = (1 << 0),
				SOFTRESET                       = (1 << 1),
				ENWAKEUP                        = (1 << 2),
				SIDLEMODE_FORCE_IDLE            = (0 << 3),
				SIDLEMODE_NO_IDLE               = (1 << 3),
				SIDLEMODE_SMART_IDLE            = (2 << 3),
				MIDLEMODE_FORCE_STANDBY         = (0 << 12),
				MIDLEMODE_NO_STANDBY            = (1 << 12),
				MIDLEMODE_SMART_STANDBY         = (2 << 12),

				// DSS_CONTROL
				DISPC_CLK_SWITCH                = (1 << 0),

				// DISPC_CONFIG
				LOADMODE_0                      = (0 << 1),
				LOADMODE_1                      = (1 << 1),
				LOADMODE_2                      = (2 << 1),
				LOADMODE_3                      = (3 << 1),
				FUNCGATED                       = (1 << 9),

				// DISPC_CONTROL
				DIGITALENABLE                   = (1 <<  1),
				LCDENABLE                       = (1 <<  0),
				TFTDATALINES_24BIT              = (3 <<  8),
				GPOUT0                          = (1 << 15),
				GPOUT1                          = (1 << 16),
				STALLMODE                       = (1 << 11),
				GOLCD                           = (1 <<  5),
				STNTFT                          = (1 <<  3),

				// DISPC_IRQSTATUS
				FRAMEDONE                       = (1 <<  0),

				// DISPC_TIMING_H
				HBP_SHIFT                       = 20,
				HFP_SHIFT                       = 8,
				HSW_SHIFT                       = 0,

				// DISPC_TIMING_V
				VBP_SHIFT                       = 20,
				VFP_SHIFT                       = 8,
				VSW_SHIFT                       = 0,

				// DISPC_POL_FREQ
				RF                              = (1 << 16),
				ONOFF                           = (1 << 17),
				IPC                             = (1 << 14),
				IHS                             = (1 << 13),
				IVS                             = (1 << 12),

				// DISPC_SIZE_LCD
				LCDSIZE_LPP_SHIFT               = 16,
				LCDSIZE_PPL_SHIFT               = 0,

				// DISPC_GFX_SIZE
				GFXSIZE_LPP_SHIFT               = 16,
				GFXSIZE_PPL_SHIFT               = 0,

				// DISPC_DIVISOR
				LCD_SHIFT                       = 16,
				PCD_SHIFT                       = 0,

				// DISPC_GFX_ATTRIBUTES
				GFXENABLE                       = (1 << 0),
				GFXFORMAT_RGB16                 = (6 << 1),
				GFXREPLICATION_ENABLE           = (1 << 5),
				GFXBURSTSIZE_2                  = (2 << 6),
			};

			enum
			{
				DEFAULT_COLOUR_MASK           = 0x0FFFF,
				TRANSPARENCY_COLOUR_MASK      = 0x0FFFF,
			};


			void _gpio_reg_write(unsigned reg, long value) {
				*(volatile long *)(_sys_regs_base + reg) = value; }

			long _gpio_reg_read(unsigned reg) {
				return *(volatile long *)(_sys_regs_base + reg); }

			void _reg_write(unsigned reg, long value) {
				*(volatile long *)(_regs_base + reg) = value; }

			long _reg_read(unsigned reg) {
				return *(volatile long *)(_regs_base + reg); }

		public:

			/**
			 * Constructor
			 */
			Session_component(void *regs_base, void *sys_regs_base,
			                  Genode::Dataspace_capability fb_ds_cap)
			: _fb_ds_cap(fb_ds_cap), _fb_ds(_fb_ds_cap),
			  _regs_base((Genode::addr_t)regs_base),
			  _sys_regs_base((Genode::addr_t)sys_regs_base)
			{
				using namespace Genode;

				// overo enable lcd
				uint32_t r = _gpio_reg_read(GPIO_OE);
				r &= ~(OVERO_GPIO_LCD_EN | OVERO_GPIO_LCD_BL);
				_gpio_reg_write(GPIO_OE, r);

				// set dataout
				r = _gpio_reg_read(GPIO_SETDATAOUT);
				r |= (OVERO_GPIO_LCD_EN | OVERO_GPIO_LCD_BL);
				_gpio_reg_write(GPIO_SETDATAOUT, r);

				uint32_t val = _reg_read(DSS_REVISIONNUMBER);
				printf("OMAP3 DSS revision %x.%x\n", ((val >> 4) & 0x0F), (val & 0xF));

				// reset dss
				_reg_write(DSS_SYSCONFIG, DSS_SOFTRESET);
				while( !(_reg_read(DSS_SYSSTATUS) & RESETDONE) )
					_timer.msleep(1);

				val = _reg_read(DSS_CONTROL);
				val &= ~(DISPC_CLK_SWITCH);
				_reg_write(DSS_CONTROL, val);

				// reset display controller
				val = _reg_read(DISPC_CONTROL);
				if (val & (DIGITALENABLE | LCDENABLE))
				{
					// Disable the lcd output and digital output
					val &= ~(DIGITALENABLE | LCDENABLE);
					_reg_write(DISPC_CONTROL, val);
					val = _reg_read(DISPC_CONTROL);
					_reg_write(DISPC_IRQSTATUS, FRAMEDONE);
					_timer.msleep(1);
					if (!((val=_reg_read(DISPC_IRQSTATUS)) & FRAMEDONE))
					{
						while (!((val=_reg_read(DISPC_IRQSTATUS)) & FRAMEDONE))
						{
							//printf("OMAP3 LCD: Disable DSS timeout.\n");
							_timer.msleep(1);
						}
					}
				}

				// configure_dss
				val = _reg_read(DISPC_REVISION);
				printf("OMAP3 DISPC revision %x.%x\n", ((val >> 4) & 0x0F), (val & 0xF));

				val = AUTOIDLE | ENWAKEUP | SIDLEMODE_SMART_IDLE | MIDLEMODE_SMART_STANDBY;
				_reg_write(DISPC_SYSCONFIG, val);

				val = LOADMODE_2 | FUNCGATED;
				_reg_write(DISPC_CONFIG, val);

				// this data from linux kernel
				_reg_write(DISPC_VID1_CONV_COEF0, 0x199012a);
				_reg_write(DISPC_VID1_CONV_COEF1, 0x12a0000);
				_reg_write(DISPC_VID1_CONV_COEF2, 0x79c0730);
				_reg_write(DISPC_VID1_CONV_COEF3, 0x12a);
				_reg_write(DISPC_VID1_CONV_COEF4, 0x205);
				_reg_write(DISPC_VID1_ATTRIBUTES, 0x00);

				_reg_write(DISPC_DEFAULT_COLOR_0, 0x00);
				_reg_write(DISPC_DEFAULT_COLOR_1, 0x00);
				_reg_write(DISPC_TRANS_COLOR_0, 0x00);
				_reg_write(DISPC_TRANS_COLOR_1, 0x00);

				// disable interrupts
				_reg_write(DISPC_IRQSTATUS, 0x00);
				_reg_write(DISPC_IRQENABLE, 0x00);

				val = IVS | IHS;
				_reg_write(DISPC_POL_FREQ, val);

				val = (1 << LCD_SHIFT) | (0x13 << PCD_SHIFT);
				_reg_write(DISPC_DIVISOR, val);

				val = (0x28 << HSW_SHIFT) |
					(0x07 << HFP_SHIFT) |
					(0x03 << HBP_SHIFT);
				_reg_write(DISPC_TIMING_H, val);

				val = (0x09 << VSW_SHIFT) |
					(0x04 << VFP_SHIFT) |
					(0x02 << VBP_SHIFT);
				_reg_write(DISPC_TIMING_V, val);


				val = ((SCR_WIDTH - 1) << LCDSIZE_PPL_SHIFT) |
					((SCR_HEIGHT - 1) << LCDSIZE_LPP_SHIFT);
				_reg_write(DISPC_SIZE_LCD, val);



				val = GFXENABLE | GFXFORMAT_RGB16 | GFXBURSTSIZE_2 | GFXREPLICATION_ENABLE;
				_reg_write(DISPC_GFX_ATTRIBUTES, val);

				_reg_write(DISPC_GFX_BA0, _fb_ds.phys_addr());
				_reg_write(DISPC_GFX_BA1, _fb_ds.phys_addr());

				_reg_write(DISPC_GFX_ROW_INC, 1);
				_reg_write(DISPC_GFX_PIXEL_INC, 1);

				_reg_write(DISPC_GFX_POSITION, 0);

				val = ((SCR_WIDTH - 1) << GFXSIZE_PPL_SHIFT) |
					((SCR_HEIGHT - 1) << GFXSIZE_LPP_SHIFT);
				_reg_write(DISPC_GFX_SIZE, 0x010f01df);

				_reg_write(DISPC_GLOBAL_ALPHA, 0xff);

				_reg_write(DISPC_GFX_FIFO_THRESHOLD, 0x3ff03c0);

				val = LCDENABLE | STNTFT | GOLCD | TFTDATALINES_24BIT | GPOUT0 | GPOUT1;
				_reg_write(DISPC_CONTROL, val);

				_timer.msleep(1);
				int err_cnt = 10;
				while (_reg_read(DISPC_CONTROL) & GOLCD)
				{
					_timer.msleep(1);
					if (err_cnt-- == 0)
					{
						printf("OMAP3 Info: Update DISPC timeout %x.\n",val);
						while(1)
							;
					}
				}
			}

			Genode::Dataspace_capability dataspace() { return _fb_ds_cap; }

			void release() { }

			Mode mode() const { return Mode(SCR_WIDTH, SCR_HEIGHT, Mode::RGB565); }

			void mode_sigh(Genode::Signal_context_capability) { }

			void refresh(int x, int y, int w, int h) { }
	};


	class Root : public Genode::Root_component<Session_component>
	{
		private:

			void                        *_lcd_regs_base;
			void                        *_sys_regs_base;
			Genode::Dataspace_capability _fb_ds_cap;

		protected:

			Session_component *_create_session(const char *args) {
				return new (md_alloc()) Session_component(_lcd_regs_base,
				                                          _sys_regs_base,
				                                          _fb_ds_cap); }

		public:

			Root(Genode::Rpc_entrypoint *session_ep, Genode::Allocator *md_alloc,
			     void *lcd_regs_base, void *sys_regs_base,
			     Genode::Dataspace_capability fb_ds_cap)
			: Genode::Root_component<Session_component>(session_ep, md_alloc),
			  _lcd_regs_base(lcd_regs_base), _sys_regs_base(sys_regs_base),
			  _fb_ds_cap(fb_ds_cap) { }
	};
}


using namespace Genode;
using namespace Framebuffer;

int main(int, char **)
{
	printf("--- omap3fb framebuffer driver ---\n");

#if 0
	static char lcd_type[32] = "";

 	try {
 		config()->xml_node().sub_node("lcd").attribute("type").value(lcd_type, sizeof(lcd_type));
		printf("lcd config type: %s\n",lcd_type);
 	} catch(...) {

 	}
#endif

	/* locally map LCD control registers */
	Io_mem_connection lcd_io_mem(OMAP3_FB_PHYS, OMAP3_FB_SIZE);
	void *lcd_base = env()->rm_session()->attach(lcd_io_mem.dataspace());

	/* locally map GPIO */
	Io_mem_connection gpio_mem(GPIO_PHYS, GPIO_SIZE);
	void *gpio_base = env()->rm_session()->attach(gpio_mem.dataspace());

	enum { STACK_SIZE = 4*1024 };
	static Cap_connection cap;
	static Rpc_entrypoint ep(&cap, STACK_SIZE, "fb_ep");

	Dataspace_capability fb_ds_cap = Framebuffer::alloc_video_memory(Framebuffer::FRAMEBUFFER_SIZE);

	/*
	 * Let the entry point serve the framebuffer and input root interfaces
	 */
	static Framebuffer::Root fb_root(&ep, env()->heap(), lcd_base, gpio_base, fb_ds_cap);
	env()->parent()->announce(ep.manage(&fb_root));

	sleep_forever();

	return 0;
}
