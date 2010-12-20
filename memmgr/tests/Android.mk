LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_PRELINK_MODULE := false
LOCAL_ARM_MODE := arm
LOCAL_SRC_FILES := memmgr_test.c ../../utils/testlib.c
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/.. \
	$(LOCAL_PATH)/../../utils
LOCAL_SHARED_LIBRARIES := libmemmgr
LOCAL_MODULE := memmgr_test
LOCAL_MODULE_TAGS := optional tests
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_PRELINK_MODULE := false
LOCAL_ARM_MODE := arm
LOCAL_SRC_FILES := utils_test.c ../../utils/testlib.c
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/.. \
	$(LOCAL_PATH)/../../utils
LOCAL_SHARED_LIBRARIES := libmemmgr
LOCAL_MODULE    := utils_test
LOCAL_MODULE_TAGS := optional tests
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_PRELINK_MODULE := false
LOCAL_ARM_MODE := arm
LOCAL_SRC_FILES := tiler_ptest.c
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/.. \
	$(LOCAL_PATH)/../../utils
LOCAL_SHARED_LIBRARIES := libmemmgr
LOCAL_MODULE    := tiler_ptest
LOCAL_MODULE_TAGS := optional tests
include $(BUILD_EXECUTABLE)

