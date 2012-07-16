/**
 * \brief  Touch screen adapter
 * \author Ivan Loskutov
 * \date   2012-03-29
 */

/*
 * Copyright 2012 Ksys Labs LLC
 * Contact: <ivan.loskutov@ksyslabs.org>
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/*
 * This server convert raw data from touch screen to screen coordinates.
 * Constants for conversion is compatible with tslib format ( /etc/pointercal)
 *
 */

/* Genode includes */
#include <base/sleep.h>
#include <base/rpc_server.h>

#include <timer_session/connection.h>
#include <cap_session/connection.h>
#include <input_session/connection.h>

#include <dataspace/client.h>

#include <input_session/input_session.h>
#include <input/event.h>

#include <ts_input_session/ts_input_session.h>

#include <root/root.h>
#include <root/component.h>

#include <os/config.h>
#include <os/attached_ram_dataspace.h>

#include <os/config.h>
#include <util/token.h>

#include "event_queue.h"

static Event_queue ev_queue;

struct cal_const {
	int a[7];
};

class CalibrationData
{
public:
	CalibrationData() {
		_cal_data.a[0] = 1; _cal_data.a[1] = 0; _cal_data.a[2] = 0;
		_cal_data.a[3] = 0; _cal_data.a[4] = 1; _cal_data.a[5] = 0;
		_cal_data.a[6] = 1;
	}

	~CalibrationData() {}

	void data(const cal_const& data)
	{
		_cal_data = data;
	}

	cal_const data() { return _cal_data; }

private:
	cal_const _cal_data;
};


/**
 * Root interface that hands out a statically created session
 */
template <typename SESSION_TYPE>
class Static_root : public Genode::Rpc_object<Genode::Typed_root<SESSION_TYPE> >
{
private:

	typedef Genode::Capability<SESSION_TYPE> Session_capability;

	Session_capability _session;

public:

	/**
	 * Constructor
	 *
	 * \param session  session to be provided to the client
	 */
	Static_root(Session_capability session) : _session(session) { }


	/********************
	 * * Root interface **
	 ********************/

	Genode::Session_capability session(Genode::Root::Session_args const &args) { return _session; }
	void upgrade(Genode::Session_capability, Genode::Root::Upgrade_args const &) { }
	void close(Genode::Session_capability) { }
};

namespace Input {

	/**
	 * Input session applying a position offset to absolute motion events
	 */
	class Session_component : public Genode::Rpc_object<Session>
	{
	private:

		/**
		 * Input session, from which we fetch events
		 */
		Input::Session              *_from_input;
		Genode::Dataspace_capability _from_input_ds;
		Genode::size_t               _from_ev_buf_size;
		Input::Event                *_from_ev_buf;

		/**
		 * Input session, to which to provide events
		 */
		Genode::Dataspace_capability _to_input_ds;
		Input::Event                *_to_ev_buf;

		CalibrationData             &_cal_data;

		/**
		 * Shortcut for mapping an event buffer locally
		 */
		Input::Event *_map_ev_buf(Genode::Dataspace_capability ds_cap) {
			return Genode::env()->rm_session()->attach(ds_cap); }

	public:

		/**
		 * Constructor
		 *
		 * \param from_input  input session from where to get input events
		 */
		Session_component(Input::Session *from_input, CalibrationData &cal_data)
		:
		_from_input(from_input),
		_from_input_ds(from_input->dataspace()),
		_from_ev_buf_size(Genode::Dataspace_client(_from_input_ds).size()),
		_from_ev_buf(_map_ev_buf(_from_input_ds)),
		_to_input_ds(Genode::env()->ram_session()->alloc(_from_ev_buf_size)),
		_to_ev_buf(_map_ev_buf(_to_input_ds)),
		_cal_data(cal_data)
		{ }


		/*****************************
		 * * Input session interface **
		 *****************************/

		Genode::Dataspace_capability dataspace() { return _to_input_ds; }

		bool is_pending() const { return _from_input->is_pending(); }

		int flush()
		{
			/* flush events at input session */
			int num_events = _from_input->flush();

			/* copy events from input buffer to client buffer */
			for (int i = 0; i < num_events; i++) {
				Input::Event e = _from_ev_buf[i];

				if ( (e.type() == Input::Event::MOTION) ||
					(e.type() == Input::Event::PRESS) ||
					(e.type() == Input::Event::RELEASE) )
				{
					cal_const c = _cal_data.data();

					float x = e.ax();
					float y = e.ay();
					float abs_x = ( c.a[2] + c.a[0]*x + c.a[1]*y ) / c.a[6];
					float abs_y = ( c.a[5] + c.a[3]*x + c.a[4]*y ) / c.a[6];

					e = Event(e.type(), e.keycode(),   abs_x, abs_y, 0, 0);

					ev_queue.add(e);
				}
				_to_ev_buf[i] = e;
			}
			return num_events;
		}
	};
}

// Buffer for copy raw input data
enum { MAX_COPY_EVENTS = 1000 };
static Genode::Dataspace_capability cp_ev_ds_cap;
static Input::Event *cp_ev_ds_buf;

namespace Ts_input {

	struct Session_component : Genode::Rpc_object<Session>
	{
	private:
		CalibrationData             &_cal_data;

	public:
		Session_component(CalibrationData &cal_data)
			:
				_cal_data(cal_data)
		{ }

		Genode::Dataspace_capability dataspace() { return cp_ev_ds_cap; }

		bool is_pending() const { return !ev_queue.empty(); }

		int flush()
		{
			int i;
			for (i = 0; (i < MAX_COPY_EVENTS) && !ev_queue.empty(); ++i)
				cp_ev_ds_buf[i] = ev_queue.get();

			/* return number of flushed events */
			return i;
		}

		void set_calibration(int a0, int a1, int a2, int a3, int a4, int a5, int a6)
		{
			PDBG("ts_input_ctrl set_calibration\n"
				"Calibration data: \n"
				"a0=%d, a1=%d, a2=%d, a3=%d, a4=%d, a5=%d, a6=%d",
				a0, a1, a2, a3, a4, a5, a6
			);

			cal_const cal_data;
			cal_data.a[0] = a0;
			cal_data.a[1] = a1;
			cal_data.a[2] = a2;
			cal_data.a[3] = a3;
			cal_data.a[4] = a4;
			cal_data.a[5] = a5;
			cal_data.a[6] = a6;

			_cal_data.data(cal_data);
		}
	};

	class Root_component : public Genode::Root_component<Session_component>
	{
	private:
		CalibrationData             &_cal_data;

	protected:

		Ts_input::Session_component *_create_session(const char *args)
		{
			return new (md_alloc()) Session_component(_cal_data);
		}

	public:

		Root_component(Genode::Rpc_entrypoint *ep,
						Genode::Allocator *allocator,
						CalibrationData &cal_data
		)
		: Genode::Root_component<Session_component>(ep, allocator),
		_cal_data(cal_data)
		{
		}
	};
}

struct Scanner_policy_number
{
	static bool identifier_char(char c, unsigned  i )
	{
		return (c == '-') || Genode::is_digit(c);
	}
};

int main(int argc, char **argv)
{
	using namespace Genode;

	/* create dataspace for event buffer that is shared with the client */
	try {
		static Attached_ram_dataspace ev_ds(env()->ram_session(),
											MAX_COPY_EVENTS*sizeof(Input::Event));
		cp_ev_ds_cap = ev_ds.cap();
		cp_ev_ds_buf = ev_ds.local_addr<Input::Event>();
	} catch (...) {
		PERR("Could not allocate dataspace for event buffer");
		return 1;
	}

	ev_queue.enable();

	enum { STACK_SIZE = 4096 };
	static Cap_connection           cap;

	static Rpc_entrypoint           ep(&cap, STACK_SIZE, "ts_input_ep");
	static Rpc_entrypoint           ctr_ep(&cap, STACK_SIZE, "ts_ctrl_ep");

	static Input::Connection        input;
	static Timer::Connection        timer;

	static CalibrationData          cal_data;

	static char calstr[256] = {0};

	try {
		config()->xml_node().sub_node("pointercal").attribute("value").value(calstr, sizeof(calstr));
		PDBG("Calibration constants: %s\n",calstr);
	} catch(...) {

	}

	typedef ::Genode::Token<Scanner_policy_number> Token;

	if (strlen(calstr))
	{
		cal_const   conf_data;
		Token       t(calstr);
		char        tmpstr[256];
		int         cnt = 0;

		while(t)
		{
			if (t.type() == Token::WHITESPACE) {
				t = t.next();
				continue;
			}

			t.string(tmpstr, 256);

			long tmpc = 0;
			ascii_to(tmpstr, &tmpc, 10);
			conf_data.a[cnt] = tmpc;

			t = t.next();

			if (cnt == 6)
				break;
			cnt++;
		}
		if (cnt == 6)
			cal_data.data(conf_data);
	}

	static Input::Session_client    ts_input(input);
	static Input::Session_component input_session(&ts_input, cal_data);

	static Static_root<Input::Session> input_root(ep.manage(&input_session));

	static Sliced_heap              sliced_heap(env()->ram_session(), env()->rm_session());
	static Ts_input::Root_component ts_root_ctrl(&ctr_ep, &sliced_heap, cal_data);

	/*
	 * Announce services
	 */
	env()->parent()->announce(ep.manage(&input_root));
	env()->parent()->announce(ep.manage(&ts_root_ctrl));

	sleep_forever();

	return 0;
}

