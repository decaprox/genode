#
# Enable peripherals of the platform
#
SPECS += cortex_a8 am33xx omap gpio

#
# Pull in CPU specifics
#
SPECS += arm_v7a

#
# Add device parameters to include search path
#
REP_INC_DIR += include/platform/am33xx

include $(call select_from_repositories,mk/spec-cortex_a8.mk)
