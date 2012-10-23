/*
 * \brief  Platform support specific to ARM
 * \author Norman Feske
 * \date   2011-05-02
 */

/*
 * Copyright (C) 2011-2012 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/* Genode includes */
#include <base/service.h>

/* Core includes */
#include <platform.h>
#include <io_port_root.h>
#include <core_env.h>

void Genode::Platform::_setup_io_port_alloc() { }

void Genode::Platform::setup_irq_mode(unsigned, unsigned, unsigned) { }

void Genode::Platform::add_local_services(Genode::Rpc_entrypoint*,
                                          Genode::Sliced_heap*,
                                          Genode::Core_env*,
                                          Genode::Service_registry*) { }
