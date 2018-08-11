LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL
SDL_MIXER_PATH := ../SDL2_mixer

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include \
                    $(LOCAL_PATH)/$(SDL_MIXER_PATH)/include

# Add your application source files here...
LOCAL_SRC_FILES := main.cpp shader.cpp

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_mixer

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog -landroid

LOCAL_CPPFLAGS := -std=c++14 -frtti

include $(BUILD_SHARED_LIBRARY)
