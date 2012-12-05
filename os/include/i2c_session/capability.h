/*
 * \brief  I2C bus driver session capability type
 * \author Alexander Tarasikov <tarasikov@ksyslabs.org> 
 * \date   2012-09-18
 */

/*
 * Copyright (C) 2012 Ksys Labs LLC
 * Copyright (C) 2012 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _INCLUDE__I2C_SESSION__CAPABILITY_H_
#define _INCLUDE__I2C_SESSION__CAPABILITY_H_

#include <base/capability.h>
#include <i2c_session/i2c_session.h>

namespace I2C { typedef Genode::Capability<Session> Session_capability; }

#endif /* _INCLUDE__I2C_SESSION__CAPABILITY_H_ */
