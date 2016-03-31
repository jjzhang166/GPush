
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)



 LOCAL_CPPFLAGS += -frtti -fexceptions
 
 
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE := efnfw

LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES := libcrypto
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../openssl-android/include

LOCAL_SRC_FILES := net/ef_sock.cpp \
base/ef_loop_buf.cpp \
base/ef_thread.cpp \
base/ef_utility.cpp \
base/ef_md5.cpp \
base/ef_hex.cpp \
base/ef_rsa.cpp \
base/ef_aes.cpp \
base/ef_base64.cpp \
base/rijndael-alg-fst.cpp \
base/rijndael-api-fst.cpp


include $(BUILD_SHARED_LIBRARY)
