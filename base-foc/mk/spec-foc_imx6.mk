SPECS += foc_arm platform_imx6

include $(call select_from_repositories,mk/spec-platform_imx6.mk)
include $(call select_from_repositories,mk/spec-foc_arm.mk)
