LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_PRELINK_MODULE := false 
LOCAL_ARM_MODE := arm

LOCAL_SRC_FILES := \
		phase1_d2c_remap.c \


LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../utils \
	$(LOCAL_PATH)/../memmgr \
	hardware/ti/syslink/syslink/api/include \	
#
	
#LOCAL_CFLAGS += -pipe -fomit-frame-pointer -Wall  -Wno-trigraphs -Werror-implicit-function-declaration  -fno-strict-aliasing -mapcs -mno-sched-prolog -mabi=aapcs-linux -mno-thumb-interwork -msoft-float -Uarm -DMODULE -D__LINUX_ARM_ARCH__=7  -fno-common -DLINUX -fpic

#-DOMAP_3430

LOCAL_MODULE    := libd2cmap

LOCAL_SHARED_LIBRARIES := libsysmgr

include $(BUILD_SHARED_LIBRARY)

