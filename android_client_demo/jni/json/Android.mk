LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)


LOCAL_C_INCLUDES += $(LOCAL_PATH)/..

 LOCAL_CPPFLAGS += -frtti -fexceptions
LOCAL_MODULE    := libjsoncpp


LOCAL_SRC_FILES := json_value.cpp json_reader.cpp json_writer.cpp 

include $(BUILD_SHARED_LIBRARY)