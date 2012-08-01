/*
 * \brief  SPI session capability type
 * \author Ivan Loskutov <ivan.loskutov@ksyslabs.org>
 * \date   2012-08-01
 */

/*
 * Copyright (C) 2012 Ksys Labs LLC
 * Copyright (C) 2012 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _INCLUDE__SPI_SESSION__CAPABILITY_H_
#define _INCLUDE__SPI_SESSION__CAPABILITY_H_

#include <base/capability.h>
#include <spi_session/spi_session.h>

namespace Spi { typedef Genode::Capability<Session> Session_capability; }

#endif /* _INCLUDE__SPI_SESSION__CAPABILITY_H_ */
