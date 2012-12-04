#
# Enable peripherals of the platform
#
SPECS += omap3fb ads7846

#
# Pull in CPU specifics
#
SPECS += arm_v7a

#
# Add device parameters to include search path
#
REP_INC_DIR += include/platform/overo

include $(call select_from_repositories,mk/spec-arm_v7a.mk)
