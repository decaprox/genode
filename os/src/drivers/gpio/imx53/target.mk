
TARGET   = imx53_gpio_drv
REQUIRES = imx53
SRC_CC   = main.cc
LIBS     = cxx env server signal
INC_DIR += $(PRG_DIR)

vpath main.cc $(PRG_DIR)

