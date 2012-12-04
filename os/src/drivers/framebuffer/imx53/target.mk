#
# \brief  Framebuffer driver specific for i.MX3 systems
# \author Nikolay Golikov
# \date   2012-
#

TARGET   = imx53_fb_drv
REQUIRES = imx53
SRC_CC   = main.cc
LIBS     = cxx env server signal
INC_DIR += $(PRG_DIR)

vpath main.cc $(PRG_DIR)

