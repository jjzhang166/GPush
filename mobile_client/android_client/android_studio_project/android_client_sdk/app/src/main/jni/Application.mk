APP_STL := stlport_static
#APP_STL := gnustl_static

#APP_STL := stl

#CPU架构	ABI
#ARMv5	armeabi	32位，从2010年
#ARMv7	armeabi-v7a	32位，从2010年
#x86	x86	32位，从2011年
#MIPS	mips	32位，从2012年
#ARMv8	arm64-v8a	64位，从2014年
#MIPS64	mips64	64位，从2014年
#x86_64	x86_64	64位，从2014年

APP_PLATFORM := android-8
#LOCAL_C_INCLUDES := ${ANDROID_NDK_ROOT}/sources/cxx-stl/stlport/stlport

TARGET_CPU_API := armeabi  arm64-v8a  x86 x86_64 armeabi-v7a
APP_ABI := armeabi  arm64-v8a  x86 x86_64 armeabi-v7a

#TARGET_CPU_API :=  armeabi  arm64-v8a  x86 x86_64 mips64
#APP_ABI := armeabi  arm64-v8a  x86 x86_64 mips64

#全平台
#TARGET_CPU_API :=  armeabi，armeabi-v7a，x86，mips，arm64-v8a，mips64，x86_64
#APP_ABI := armeabi，armeabi-v7a，x86，mips，arm64-v8a，mips64，x86_64

#APP_OPTIM := debug  
APP_OPTIM := release

#TARGET_CPU_API := armeabi  arm64-v8a  x86 x86_64 mips64
#APP_ABI := armeabi-v7a
