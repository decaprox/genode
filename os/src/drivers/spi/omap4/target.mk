
TARGET   = omap4_mcspi_drv
REQUIRES = omap4
SRC_CC   = main.cc
LIBS     = cxx env server
INC_DIR += $(PRG_DIR)

vpath main.cc $(PRG_DIR)

