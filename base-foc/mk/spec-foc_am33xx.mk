SPECS += foc_arm platform_am33xx

include $(call select_from_repositories,mk/spec-platform_am33xx.mk)
include $(call select_from_repositories,mk/spec-foc_arm.mk)
