#
# \brief   Platform implementations specific for base-hw and VEA9X4
# \author  Stefan Kalkowski
# \date    2012-10-04
#

# add include paths
INC_DIR += $(REP_DIR)/src/core/include \
           $(BASE_DIR)/src/core/include

SRC_CC = platform_support.cc

vpath platform_support.cc $(REP_DIR)/src/core/vea9x4