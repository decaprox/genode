#
# \brief  Build-system configurations specific to i.MX6
# \author Nikolay Golikov
# \date   20123-02-13
#

# denote wich specs are also fullfilled by this spec
SPECS += cortex_a9 imx6 imx epit

# add repository relative include paths
REP_INC_DIR += include/platform/imx6

# include implied specs
include $(call select_from_repositories,mk/spec-cortex_a9.mk)

