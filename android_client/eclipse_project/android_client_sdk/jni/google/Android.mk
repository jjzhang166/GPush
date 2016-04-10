LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

 LOCAL_CPPFLAGS += -frtti -fexceptions
 LOCAL_CPPFLAGS += -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY
LOCAL_MODULE    := protobuflite

LOCAL_C_INCLUDES += $(LOCAL_PATH)/..
LOCAL_SRC_FILES := protobuf/io/zero_copy_stream_impl_lite.cc \
protobuf/io/zero_copy_stream.cc \
protobuf/stubs/common.cc \
protobuf/generated_message_util.cc \
protobuf/wire_format_lite.cc \
protobuf/repeated_field.cc \
protobuf/stubs/once.cc \
protobuf/message_lite.cc \
protobuf/io/coded_stream.cc \
protobuf/extension_set.cc \
protobuf/stubs/atomicops_internals_x86_gcc.cc

include $(BUILD_SHARED_LIBRARY)