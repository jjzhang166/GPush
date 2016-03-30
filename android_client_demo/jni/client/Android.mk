
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../json
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../efnfw
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../proto
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../openssl-android/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/..


LOCAL_CPPFLAGS += -frtti -fexceptions
LOCAL_CPPFLAGS += -fPIC

#LOCAL_CPPFLAGS += -D_DEBUG
LOCAL_CPPFLAGS += -Wl,-Map=test.map -g 

LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE := client

LOCAL_MODULE_TAGS := optional

LOCAL_PROGUARD_ENABLED:= disabled

LOCAL_CPP_EXTENSION := .cc .cpp

LOCAL_SHARED_LIBRARIES := libjsoncpp libprotobuflite libefnfw

LOCAL_SRC_FILES :=  ../proto/message.pb.cpp \
../proto/pair.pb.cpp \
../proto/session.pb.cpp \
../proto/gpush.pb.cpp \
eventloop.cpp \
opbase.cpp \
ops.cpp \
client_conn.cpp \
client.cpp \
client_log.cpp 

#LOCAL_STATIC_LIBRARIES := libjsoncpp libprotobuflite libefnfw

LOCAL_LDLIBS+= -L$(SYSROOT)/usr/lib -llog

include $(BUILD_SHARED_LIBRARY) 