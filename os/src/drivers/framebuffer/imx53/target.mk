#
# \brief  Framebuffer driver specific for i.MX3 systems
# \author Nikolay Golikov <nik@ksyslabs.org>
# \date   2012-11-10
#

#
# Copyright (C) 2012 Ksys Labs LLC
# Copyright (C) 2012 Genode Labs GmbH
#
# This file is part of the Genode OS framework, which is distributed
# under the terms of the GNU General Public License version 2.
#

TARGET   = imx53_fb_drv
REQUIRES = imx53
SRC_CC   = main.cc
LIBS     = cxx env server signal
INC_DIR += $(PRG_DIR)

vpath main.cc $(PRG_DIR)

