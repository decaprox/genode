/*
 * \brief  Signal handler for GPIO IRQ
 * \author Ivan Loskutov <ivan.loskutov@ksyslabs.org>
 * \date   2012-06-23
 */

/*
 * Copyright (C) 2012 Ksys Labs LLC
 * Copyright (C) 2012 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _SIGNAL_HANDLER_H_
#define _SIGNAL_HANDLER_H_

/* Genode includes */
#include <base/thread.h>
#include <base/signal.h>

#include <base/printf.h>


/* local includes */
#include "driver.h"

class Signal_handler : Genode::Thread<4096>
{
private:

	Driver          &_driver;
	Signal_receiver *_receiver;

public:

	Signal_handler(Driver &driver)
	:
	_driver(driver)
	{
		_receiver = _driver.get_signal_receiver();
		start();
	}

	void entry()
	{
		while (1) {
			_receiver->wait_for_signal();
			_driver.handle_event();
		}
	}
};

#endif /* _SIGNAL_HANDLER_H_ */
