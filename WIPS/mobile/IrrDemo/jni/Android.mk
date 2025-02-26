LOCAL_PATH := $(call my-dir)/..
IRRLICHT_PROJECT_PATH := /home/Dev/android-sdk-linux/EXTRAstuff/ogl-es

include $(CLEAR_VARS)
LOCAL_MODULE := ogg
LOCAL_SRC_FILES := libs/libogg/android/lib/armeabi/libogg.a
#$(TARGET_ARCH_ABI)
include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := Irrlicht
LOCAL_SRC_FILES := $(IRRLICHT_PROJECT_PATH)/lib/Android/libIrrlicht.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := IrrDemo

LOCAL_CFLAGS := -D_IRR_ANDROID_PLATFORM_ -pipe -fno-exceptions -fno-rtti -fstrict-aliasing

ifndef NDEBUG
LOCAL_CFLAGS += -g -D_DEBUG
else
LOCAL_CFLAGS += -fexpensive-optimizations -O3
endif

ifeq ($(TARGET_ARCH_ABI),x86)
LOCAL_CFLAGS += -fno-stack-protector
endif


LOCAL_C_INCLUDES := $(IRRLICHT_PROJECT_PATH)/include

LOCAL_SRC_FILES := main.cpp android_tools.cpp 

LOCAL_LDLIBS := -lEGL -llog -lGLESv1_CM -lGLESv2 -lz -landroid

LOCAL_STATIC_LIBRARIES := Irrlicht android_native_app_glue ogg
#LOCAL_STATIC_LIBRARIES := freetype tremor ogg Irrlicht android_native_app_glue
#LOCAL_SHARED_LIBRARIES := openal freealut



include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)

# copy Irrlicht data to assets

$(shell mkdir -p $(IRRLICHT_PROJECT_PATH)/assets)
$(shell mkdir -p $(IRRLICHT_PROJECT_PATH)/assets/media)
$(shell mkdir -p $(IRRLICHT_PROJECT_PATH)/assets/media/Shaders)
$(shell mkdir -p $(IRRLICHT_PROJECT_PATH)/src)
$(shell cp $(IRRLICHT_PROJECT_PATH)/media/Shaders/*.* $(IRRLICHT_PROJECT_PATH)/assets/media/Shaders/)
$(shell cp $(IRRLICHT_PROJECT_PATH)/media/irrlichtlogo3.png $(IRRLICHT_PROJECT_PATH)/assets/media/)
$(shell cp $(IRRLICHT_PROJECT_PATH)/media/dwarf.x $(IRRLICHT_PROJECT_PATH)/assets/media/)
$(shell cp $(IRRLICHT_PROJECT_PATH)/media/dwarf.jpg $(IRRLICHT_PROJECT_PATH)/assets/media/)
$(shell cp $(IRRLICHT_PROJECT_PATH)/media/axe.jpg $(IRRLICHT_PROJECT_PATH)/assets/media/)
$(shell cp $(IRRLICHT_PROJECT_PATH)/media/fonthaettenschweiler.bmp $(IRRLICHT_PROJECT_PATH)/assets/media/)
$(shell cp $(IRRLICHT_PROJECT_PATH)/media/bigfont.png $(IRRLICHT_PROJECT_PATH)/assets/media/)

