/*
 * \brief  Alpar protocol implementation
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

#ifndef _ALPAR_H_
#define _ALPAR_H_

#include <base/printf.h>
#include <base/signal.h>
#include <timer_session/connection.h>
#include <terminal_session/connection.h>
#include <smartcard_session/smartcard_session.h>

/* local includes */
#include "readerexceptions.h"

using Genode::uint8_t ;
using Genode::uint16_t;
using Genode::uint32_t;
using Genode::memcpy;
using Genode::snprintf;


class AlparProtocol
{
public:
	enum {
		BAUDRATE_115200         = 0x06,
	};

	enum Commands {
		CARD_COMMAND            = 0x00,
		CARD_PRESENCE           = 0x09,
		SEND_NUM_MASK           = 0x0a,
		SET_SERIAL_BAUD_RATE    = 0x0d,
		POWER_OFF               = 0x4d,
		POWER_UP_ISO            = 0x69,
	};

	enum {
		ALPAR_ACK               = 0x60,
		ALPAR_NAK               = 0xe0,
	};

	AlparProtocol() {}

	AlparProtocol(uint8_t command, uint8_t *data = 0, uint16_t length = 0)
	: _command(command), _data(data), _length(length)
	{
		if (_length > MAX_PACKET_DATA)
			throw DataTooLong();

		_buffer[0] = ALPAR_ACK;
		_buffer[1] = (_length & 0xFF00) >> 8 ;
		_buffer[2] = _length & 0x00FF;
		_buffer[3] = _command;

		if(_length)
			memcpy(_buffer + 4, _data, _length);

		_buffer[_length + 4] = _lrc(_length + 4);
	}

	void parse_buffer()
	{
		PDBG("%s", _buffer[0] == ALPAR_ACK ? "Successfull Ack(0x60)" :
				_buffer[0] == ALPAR_NAK ? "Error Nak(0xE0)" :
				"Unknown header signature!!\n"
		);

		if((_buffer[0] != ALPAR_ACK) && (_buffer[0] != ALPAR_NAK) )
			throw UnknownHeaderSignature();

		_command = _buffer[4];
		_length = ( _buffer[1] << 8 ) | ( _buffer[2] );

		if (_length > MAX_PACKET_DATA)
			throw DataTooLong();


		if (_length > 0) {
			_data = _buffer + 4;
		} else {
			_data = 0;
		}

		if ( _lrc(_length + 5) )
			throw BadLRC();

		if ( _buffer[0] != ALPAR_ACK )
		{
			PDBG("Error: %s", _error_str(_buffer[5]));
		}
	}

	uint8_t *buffer() { return _buffer; }

 	const uint8_t *buffer() const { return _buffer; }

	uint16_t buffer_length() const { return _length + 5; }

	uint8_t *data() const { return _data; }
	uint16_t length() const { return _length; }
	uint16_t command() const { return _command; }
	uint16_t ack() const { return _buffer[0]; }

private:
	uint8_t _lrc(uint32_t length) {
		uint8_t lrc = _buffer[0];

		for(unsigned i = 1; i < length; i++)
			lrc = lrc ^ _buffer[i];

		return lrc;
	}

	const char* _error_str(uint8_t status)
	{
		switch(status)
		{
			case 0x08: return "Length of the data buffer too short";
			case 0x0a: return "3 consecutive errors from the card in T=1 protocol";

			case 0x20: return "Wrong APDU";
			case 0x21: return "Too short APDU";
			case 0x22: return "Card mute now (during T=1 exchange)";
			case 0x24: return "Bad NAD";
			case 0x25: return "Bad LRC";
			case 0x26: return "Resynchronized";
			case 0x27: return "Chain aborted";
			case 0x28: return "Bad PCB";
			case 0x29: return "Overflow from card";

			case 0x30: return "Non negotiable mode (TA2 present)";
			case 0x31: return "Protocol is neither T=0 nor T=1 (negotiate command)";
			case 0x32: return "T=1 is not accepted (negotiate command)";
			case 0x33: return "PPS answer is different from PPS request";
			case 0x34: return "Error on PCK (negotiate command)";
			case 0x35: return "Bad parameter in command";
			case 0x38: return "TB3 absent";
			case 0x39: return "PPS not accepted (no answer from card)";
			case 0x3B: return "Early answer of the card during the activation";

			case 0x55: return "Unknown command";

			case 0x80: return "Card mute (after power on)";
			case 0x81: return "Time out (waiting time exceeded)";
			case 0x83: return "5 parity errors in reception";
			case 0x84: return "5 parity errors in transmission";
			case 0x86: return "Bad FiDi";
			case 0x88: return "ATR duration greater than 19200 etus (E.M.V.)";
			case 0x89: return "CWI not supported (E.M.V.)";
			case 0x8A: return "BWI not supported (E.M.V.)";
			case 0x8B: return "WI (Work waiting time) not supported (E.M.V.)";
			case 0x8C: return "TC3 not accepted (E.M.V.)";
			case 0x8D: return "Parity error during ATR";

			case 0x90: return "3 consecutive parity errors in T=1 protocol";
			case 0x91: return "SW1 different from 6X or 9X";
			case 0x92: return "Specific mode byte TA2 with b5 byte=1";
			case 0x93: return "TB1 absent during a cold reset (E.M.V.)";
			case 0x94: return "TB1 different from 00 during a cold reset (E.M.V.)";
			case 0x95: return "IFSC<10H or IFSC=FFH";
			case 0x96: return "Wrong TDi";
			case 0x97: return "TB2 is present in the ATR (E.M.V.)";
			case 0x98: return "TC1 is not compatible with CWT";
			case 0x9B: return "Not T=1 card";

			case 0xA0: return "Procedure byte error";
			case 0xA1: return "Card deactivated due to a hardware problem";

			case 0xC0: return "Card absent";
			case 0xC3: return "Checksum error";
			case 0xC4: return "TS is neither 3B nor 3F";
			case 0xC6: return "ATR not supported";
			case 0xC7: return "VPP is not supported";

			case 0xE1: return "Card clock frequency not accepted (after a set_clock_card command)";
			case 0xE2: return "UART overflow";
			case 0xE3: return "Supply voltage drop-off";
			case 0xE4: return "Temperature alarm";
			case 0xE9: return "Framing error";

			case 0xF0: return "Serial LRC error";
			case 0xFF: return "Serial time out";

			default:   return "Unknown error";
		}
	}

private:
	uint8_t   _command;
	uint8_t  *_data;
	uint16_t  _length;

	enum {
		MAX_PACKET_DATA = 506,
		READ_BUFFER_SIZE = MAX_PACKET_DATA + 5,
	};
	uint8_t   _buffer[READ_BUFFER_SIZE];
};

class Alpar
{
	Smartcard::ReaderStatus _status;

	unsigned                _timeout;

	Timer::Connection       _timer;
	Terminal::Connection    _terminal;

public:
	Alpar() : _timeout(1000) {}

	bool open_reader();
	bool card_activate();
	bool card_deactivate();
	bool card_present();
	bool card_command(const void *cmd_buf, unsigned cmd_length, void *resp_buf, unsigned *resp_len);

	void wait_card();

	void get_reader_status(Smartcard::ReaderStatus *status)
	{
		*status = _status;
	}

private:
	/* alpar cmds */
	bool send_num_mask(char *firmware_str);
	bool set_serial_baud_rate(unsigned char baud);

	bool check_presence_card();

	bool power_up_iso();
	bool power_off();

	/* internal functions */
	void _set_timeout(unsigned msec) { _timeout = msec; }

	void _transmit(const AlparProtocol& cmd);
	AlparProtocol _recv();
	void _read_terminal(uint8_t *buf, unsigned length);
	void _flush_terminal();
};

bool Alpar::open_reader()
{
	char firmware[256];
	unsigned baud = 38400;

	_terminal.set_baudrate(baud);
	_timer.msleep(200);

	PDBG("Open serial port with baud %d.", baud);

	_set_timeout(500);

	while ( !send_num_mask(firmware) )
	{
		baud = 115200;
		_terminal.set_baudrate(baud);
		_timer.msleep(200);
		PDBG("Reopen serial port with baud %d.", baud);
		PDBG("failed");
		_timer.msleep(1000);
	}

	if (baud != 115200)
	{
		if ( !set_serial_baud_rate(AlparProtocol::BAUDRATE_115200) )
			return false;

		baud = 115200;
		_terminal.set_baudrate(baud);
		_timer.msleep(200);

		PDBG("Reopen serial port with baud %d.", baud);
	}

	if ( !send_num_mask(firmware) ) return false;

	snprintf(_status._reader_name, 255, "TDA8029 (%s)", firmware);
	PDBG("%s", _status._reader_name);

	_set_timeout(1000);

	return true;
}

bool Alpar::card_activate()
{
	try {
		return power_up_iso();
	}
	catch(const ReaderException& e)
	{
		PERR("Error: %s", e.what());
	}
	return false;
}

bool Alpar::card_deactivate()
{
	try {
		power_off();
		return true;
	}
	catch(const ReaderException& e)
	{
		PERR("Error: %s", e.what());
	}
	return false;
}

bool Alpar::card_present()
{
	try {
		return check_presence_card();
	}
	catch(const ReaderException& e)
	{
		PERR("Error: %s", e.what());
	}
	return false;
}

void Alpar::wait_card()
{
	Genode::Signal_receiver sig_rec;
	Genode::Signal_context  sig_ctx;

	_terminal.read_avail_sigh(sig_rec.manage(&sig_ctx));

	while(!_status._card_connected)
	{
		sig_rec.wait_for_signal();

		AlparProtocol response = _recv();
		if (response.length() == 1)
		{
			if (response.command() == 0xa0)
			{
				_status._card_connected = response.data()[0] == 0x01;
			}
		}
	}
}

bool Alpar::card_command(const void *cmd, unsigned cmd_len, void *resp, unsigned *resp_len)
{
	try {

		_transmit( AlparProtocol(AlparProtocol::CARD_COMMAND, (uint8_t*)cmd, cmd_len) );

		AlparProtocol response = _recv();

		memcpy(resp, response.data(), response.length());
		*resp_len = response.length();
		return true;
	}
	catch(const ReaderException& e)
	{
		*resp_len = 0;
		PERR("Error: %s", e.what());
	}
	return false;
}

//
bool Alpar::send_num_mask(char *firmware_str)
{
	_transmit( AlparProtocol(AlparProtocol::SEND_NUM_MASK) );

	try {
		AlparProtocol response = _recv();

		memcpy(firmware_str, response.data(), response.length());
		firmware_str[response.length()]=0;
	}
	catch(Timeout)
	{
		return false;
	}

	return true;
}

bool Alpar::set_serial_baud_rate(unsigned char baud)
{
	_transmit( AlparProtocol(AlparProtocol::SET_SERIAL_BAUD_RATE, &baud, 1) );
	_recv();

	return true;
}

bool Alpar::check_presence_card()
{
	unsigned char presence;

	_transmit( AlparProtocol(AlparProtocol::CARD_PRESENCE) );
	AlparProtocol response = _recv();

	if(response.length() != 1)
		return false;

	presence = response.data()[0] == 1;

	return presence;
}

bool Alpar::power_up_iso()
{
	_transmit( AlparProtocol(AlparProtocol::POWER_UP_ISO) );
	AlparProtocol response = _recv();

	if(response.data()[0] == 0xc0)
	{
		PDBG("Card absent\n");
		return false;
	}
	// TODO: add to status

	memcpy(_status._atr.data, response.data(), response.length());
	_status._atr.length = response.length();

	return true;
}

bool Alpar::power_off()
{
	_transmit( AlparProtocol(AlparProtocol::POWER_OFF) );
	_recv();

	return true;
}

void Alpar::_transmit(const AlparProtocol& cmd)
{
	_flush_terminal();
	_terminal.write(cmd.buffer(), cmd.buffer_length());
}

AlparProtocol Alpar::_recv()
{
	AlparProtocol response;
	uint16_t length;

	_read_terminal(response.buffer(), 4);

	length = ( response.buffer()[1] << 8 ) | ( response.buffer()[2] );

	_read_terminal(response.buffer() + 4, length + 1);

	// check error
	try {
		response.parse_buffer();
	}
	catch (AlparNac)
	{
		PERR("NAC");
	}
	return response;
}

void Alpar::_read_terminal(uint8_t *buf, unsigned length)
{
	unsigned ms_count = 0;
	unsigned readed = 0;
	unsigned pos = 0;

	while(length)
	{
		if (!_terminal.avail())
		{
			_timer.msleep(1);
			if (++ms_count >= _timeout)
				throw Timeout();
		}
		else
		{
			readed = _terminal.read(buf + pos, length);
			pos += readed;
			length -= readed;
		}

	}
}

void Alpar::_flush_terminal()
{
	while(_terminal.avail())
	{
		unsigned char dummy;
		_terminal.read(&dummy, 1);
	}
}



#endif /* _ALPAR_H_ */
