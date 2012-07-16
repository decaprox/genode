SPECS += foc_arm platform_overo

LD_TEXT_ADDR = 0x80140000

include $(call select_from_repositories,mk/spec-platform_overo.mk)
include $(call select_from_repositories,mk/spec-foc_arm.mk)
