/*
 * \brief  Virtual Machine Monitor
 * \author Stefan Kalkowski
 * \date   2012-06-25
 */

/*
 * Copyright (C) 2008-2012 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/* Genode includes */
#include <base/env.h>
#include <base/sleep.h>
#include <base/thread.h>
#include <drivers/board_base.h>
#include <drivers/trustzone.h>

/* local includes */
#include <vm.h>
#include <atag.h>
#include <tsc_380.h>
#include <iomux.h>

using namespace Genode;

enum {
	KERNEL_OFFSET    = 0x1000,
	MACH_TYPE_TABLET = 3011,
	MACH_TYPE_QSB    = 3769,//3273,
	BOARD_REV_TABLET = 0x53321,
};


static const char* cmdline_tablet = "noinitrd console=ttymxc0,115200,earlyprintk loglevel=10";


namespace Vmm {
	class Vmm;
}


class Vmm::Vmm : public Thread<8192>
{
	private:

		enum Devices {
			FRAMEBUFFER,
			INPUT,
		};

		Iomux                     _iomux;
		Signal_receiver           _sig_rcv;
		Signal_context            _vm_context;
		Vm                       *_vm;
		Io_mem_connection         _tsc_380_io_mem;
		Tsc_380                   _tsc_380;

		void _handle_hypervisor_call()
		{
			/* check device number*/
			switch (_vm->state()->r0) {
			case FRAMEBUFFER:
			case INPUT:
				break;
			default:
				PERR("Unknown hypervisor call!");
				_vm->dump();
			};
		}

		bool _handle_data_abort()
		{
			_vm->dump();
			return false;
		}

		bool _handle_vm()
		{
			/* check exception reason */
			switch (_vm->state()->cpu_exception) {
			case Cpu_state::DATA_ABORT:
				if (!_handle_data_abort()) {
					PERR("Could not handle data-abort will exit!");
					return false;
				}
				break;
			case Cpu_state::SUPERVISOR_CALL:
				_handle_hypervisor_call();
				return false;
				break;
			default:
				PERR("Curious exception occured");
				_vm->dump();
				return false;
			}
			return true;
		}

	protected:

		void entry()
		{
			_vm->dump();
			_vm->sig_handler(_sig_rcv.manage(&_vm_context));
			_vm->start();
			_vm->run();
			while (true) {
				Signal s = _sig_rcv.wait_for_signal();
				if (s.context() == &_vm_context) {
					if (_handle_vm())
						_vm->dump();
						_vm->run();
				} else {
					PWRN("Invalid context");
					continue;
				}
			}
		};

	public:

		Vmm(Vm *vm)
		: Thread<8192>("vmm"),
		  _vm(vm),
		  _iomux(),
		  _tsc_380_io_mem(Board_base::TZASC_BASE,Board_base::TZASC_SIZE),
		  _tsc_380((addr_t)env()->rm_session()->attach(_tsc_380_io_mem.dataspace()))
		{
			
		}
};


int main()
{
	static Vm vm("linux", "initrd.gz", cmdline_tablet,
	             Trustzone::NONSECURE_RAM_BASE, Trustzone::NONSECURE_RAM_SIZE,
	             KERNEL_OFFSET, MACH_TYPE_QSB);
	static Vmm::Vmm vmm(&vm);

	PINF("Start virtual machine ...");
	vmm.start();

	sleep_forever();
	return 0;
}
