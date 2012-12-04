/*
 * \brief  TDA8029 driver
 * \author Ivan Loskutov <ivan.loskutov@ksyslabs.org>
 * \date   2012-10-13
 */

/*
 * Copyright (C) 2012 Ksys Labs LLC
 * Copyright (C) 2012 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _DRIVER_H_
#define _DRIVER_H_

/* Genode includes */
#include <base/printf.h>
// #include <base/signal.h>
#include <timer_session/connection.h>
#include <terminal_session/connection.h>

/* local includes */
#include "smartcard_driver.h"
#include "alpar.h"

using namespace Genode;

static int verbose = 1;

class Driver: public Smartcard::Driver
{
	Alpar _reader;

public:

	Driver();

	void get_reader_status(Smartcard::ReaderStatus *status);

	bool is_card_present();
	bool activate_card();
	bool deactivate_card();

	size_t transmit(const void *cmd_buf, Genode::size_t cmd_length, void *resp_buf);

private:
	void init();
};


Driver::Driver()
{
	if (!_reader.open_reader())
	{
		PERR("Open reader error");
	}
}

bool Driver::is_card_present()
{
	return _reader.card_present();
}

bool Driver::activate_card()
{
	return _reader.card_activate();
}

bool Driver::deactivate_card()
{
	return _reader.card_deactivate();
}

size_t Driver::transmit(const void *cmd_buf, Genode::size_t cmd_length, void *resp_buf)
{
	unsigned received;
	_reader.card_command(cmd_buf, cmd_length, resp_buf, &received);

	return received;
}

void Driver::get_reader_status(Smartcard::ReaderStatus *status)
{
	_reader.get_reader_status(status);
}



#endif // _DRIVER_H_
