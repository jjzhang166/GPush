
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
$(warning $(LOCAL_PATH))

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../client


LOCAL_CPPFLAGS += -frtti -fexceptions
LOCAL_CPPFLAGS += -fPIC

#LOCAL_CPPFLAGS += -D_DEBUG
LOCAL_CPPFLAGS += -Wl,-Map=test.map -g 

LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE := clientsdk

LOCAL_MODULE_TAGS := optional

LOCAL_PROGUARD_ENABLED:= disabled

LOCAL_CPP_EXTENSION := .cc .cpp

LOCAL_SHARED_LIBRARIES := liblog libcutils libandroid_runtime libnativehelper  libclient

LOCAL_SRC_FILES := androidclient.cpp \
client_jni.cpp

#LOCAL_STATIC_LIBRARIES := libjsoncpp libprotobuflite libefnfw

LOCAL_LDLIBS+= -L$(SYSROOT)/usr/lib -llog

include $(BUILD_SHARED_LIBRARY) 