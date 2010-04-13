# only include if running on an omap4 platform
ifeq ($(TARGET_BOARD_PLATFORM),omap4)
include $(call all-subdir-makefiles)
endif
