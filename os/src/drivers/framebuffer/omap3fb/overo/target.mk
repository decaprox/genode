TARGET   = omap3fb_drv
REQUIRES = omap3fb platform_overo
SRC_CC   = main.cc video_memory.cc
LIBS     = cxx env server
INC_DIR += $(PRG_DIR)/..

vpath %.cc $(PRG_DIR)/..
