TARGET   = gpio_drv
REQUIRES = am33xx
SRC_CC   = main.cc
LIBS     = base
INC_DIR += $(PRG_DIR) $(PRG_DIR)/../

vpath main.cc $(PRG_DIR)

