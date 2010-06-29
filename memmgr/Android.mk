LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_PRELINK_MODULE := false
LOCAL_ARM_MODE := arm

LOCAL_SRC_FILES := \
		memmgr.c \
		tilermgr.c \


LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../utils \


#LOCAL_CFLAGS += -pipe -fomit-frame-pointer -Wall  -Wno-trigraphs -Werror-implicit-function-declaration  -fno-strict-aliasing -mapcs -mno-sched-prolog -mabi=aapcs-linux -mno-thumb-interwork -msoft-float -Uarm -DMODULE -D__LINUX_ARM_ARCH__=7  -fno-common -DLINUX -fpic

#-DOMAP_3430

LOCAL_MODULE    := libmemmgr

include $(BUILD_SHARED_LIBRARY)

include $(LOCAL_PATH)/tests/Android.mk
