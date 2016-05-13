LOCAL_PATH := $(call my-dir)

subdirs := $(LOCAL_PATH)/json/Android.mk
subdirs += $(LOCAL_PATH)/efnfw/Android.mk
subdirs += $(LOCAL_PATH)/google/Android.mk
subdirs += $(LOCAL_PATH)/client/Android.mk
subdirs += $(LOCAL_PATH)/aclient/Android.mk

include $(subdirs)